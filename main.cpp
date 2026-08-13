// SSFH-PEMF — Spread Spectrum Frequency Hopping Pulsed Electro-Magnetic Field
// Drives a FeelTech FY3200S function generator from a Particle Photon.
//
// Timing Model (8/4 time signature at 90 BPM):
//   - One musical measure = 5333 ms
//   - Long cycle (master rhythm): 10368 ms (~2 measures)
//   - Chakra updates: every 648 ms
//   - SolNog updates: every 1152 ms
//
// Chakra frequencies are tuned to 432 Hz base. Combines 7 Solfeggio tones
// with Nogier frequencies (low + high pairs) in a randomized hopping pattern.
// LED feedback indicates sync (magenta blink), Chakra-dominant (green), or
// SolNog-dominant (orange) states.

#include "Particle.h"

// All constants and configuration live in an anonymous namespace
namespace
{
    // Hardware
    const pin_t ledPin = D7;

    // Parameter delay after each command sent to FY3200S (ms)
    const unsigned long commandDelayMs = 555;

    // Timing periods (all in milliseconds)
    const unsigned long longPeriodMs   = 10368;  // Master rhythm / random reseed
    const unsigned long chakraPeriodMs = 648;
    const unsigned long solnogPeriodMs = 1152;

    // Main output parameters for FY3200S
    const unsigned char  mainWave      = 0;     // 0 = sine
    const unsigned int   mainDuty      = 500;   // 50% duty cycle (bd500)
    const unsigned int   mainAmplitude = 1900;  // ~19 Vpp (ba1900)

    // RNG seed from hardware
    uint32_t rngSeed = HAL_RNG_GetRandomNumber();

    // Current state for frequency selection and LED feedback
    unsigned char currentChakra       = 0;  // 0–6 (Crown → Root)
    unsigned char currentChakraLevel  = 0;  // 0–3 intensity level
    unsigned char currentSolNogIndex  = 0;  // 0–6 for SolNog table

    // === Frequency Tables ===

    // Nogier frequencies (low + high paired) - indexed 0..6
    const unsigned int NogierLow[7] = {
        228,    // A - Cellular Vitality & repair
        456,    // B - Nutritional Metabolism
        913,    // C - Movement & coordination
        1825,   // D - Bilateral integration
        3650,   // E - Nerves & pain relief
        7341,   // F - Emotional balance
        14681   // G - Intellectual organization
    };

    const unsigned int NogierHigh[7] = {
        29363,   // A high
        58725,   // B high
        117450,  // C high
        234900,  // D high
        937528,  // E high (shared with Vishuddha Throat Level 4)
        934400,  // F high
        1868800  // G high
    };

    // Solfeggio frequencies - aligned with the 7 chakras (index 0..6)
    const unsigned int Solfeggio[7] = {
        28500,  // DO  - Tissue regeneration (Root)
        39600,  // UT  - Releasing guilt/fear (Sacral)
        41700,  // RE  - Facilitating change (Solar Plexus)
        52800,  // MI  - DNA repair & love (Heart)
        63900,  // FA  - Relationships & communication (Throat)
        74100,  // SO  - Intuitive awakening (Third Eye)
        85200   // LA  - Spiritual awakening (Crown) — 96300 used in other contexts
    };

    // === Chakra Frequencies ===
    // One clean array per chakra (easy to read and maintain)

    const unsigned int Sahasrara_Crown[4] = {
        10800,  // Level 1
        21600,  // Level 2
        43200,  // Level 3
        86333   // Level 4
    };

    const unsigned int Ajna_ThirdEye[4] = {
        14400,   // Level 1
        28833,   // Level 2
        57600,   // Level 3
        115500   // Level 4
    };

    const unsigned int Vishuddha_Throat[4] = {
        19266,   // Level 1
        38466,   // Level 2
        76899,   // Level 3
        937528   // Level 4 (also appears in Nogier E-high)
    };

    const unsigned int Anahata_Heart[4] = {
        12833,  // Level 1
        25700,  // Level 2
        51400,  // Level 3
        102800  // Level 4
    };

    const unsigned int Manipura_SolarPlexus[4] = {
        18166,  // Level 1
        36366,  // Level 2
        72666,  // Level 3
        145200  // Level 4
    };

    const unsigned int Svadhisthana_Sacral[4] = {
        15266,  // Level 1
        30533,  // Level 2
        61133,  // Level 3
        122100  // Level 4
    };

    const unsigned int Muladhara_Root[4] = {
        11433,  // Level 1
        22900,  // Level 2
        45733,  // Level 3
        91500   // Level 4
    };

    // Simple lookup: Chakra[chakraIndex] gives the 4-level array for that chakra.
    // Order: 0=Crown, 1=Third Eye, 2=Throat, 3=Heart, 4=Solar Plexus, 5=Sacral, 6=Root
    const unsigned int (* const Chakra[7])[4] = {
        &Sahasrara_Crown,
        &Ajna_ThirdEye,
        &Vishuddha_Throat,
        &Anahata_Heart,
        &Manipura_SolarPlexus,
        &Svadhisthana_Sacral,
        &Muladhara_Root
    };
}

// LED feedback objects (global so they can be used across setup() and loop())
LEDStatus solidOrange(RGB_COLOR_ORANGE, LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus solidGreen (RGB_COLOR_GREEN,  LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus solidRed   (RGB_COLOR_RED,    LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus breathGray (RGB_COLOR_GRAY,   LED_PATTERN_FADE,  LED_SPEED_NORMAL, LED_PRIORITY_IMPORTANT);
LEDStatus blinkMagenta(RGB_COLOR_MAGENTA, LED_PATTERN_BLINK, LED_SPEED_FAST, LED_PRIORITY_CRITICAL);

enum ChannelType {
    CHAKRA,
    SOLNOG
};

// Simple non-blocking timer helper (renamed to avoid conflict with Particle's built-in Timer class)
struct CustomTimer {
    unsigned long interval;
    unsigned long lastTrigger;

    CustomTimer(unsigned long intervalMs) : interval(intervalMs), lastTrigger(0) {}

    bool check(unsigned long now) {
        if (now - lastTrigger >= interval) {
            lastTrigger = now;
            return true;
        }
        return false;
    }
};

SYSTEM_THREAD(ENABLED);

void setup()
{
    Serial.begin(9600);        // USB serial for debugging (use `particle serial monitor`)
    pinMode(ledPin, OUTPUT);

    breathGray.setActive(true);
    delay(1000);               // Brief breathing pause at startup

    randomSeed(rngSeed);       // Seed Arduino random() with hardware RNG

    Serial1.begin(9600);       // Communication with FY3200S generator

    // Initialize state
    currentChakra       = 0;
    currentChakraLevel  = 0;
    currentSolNogIndex  = 0;

    delay(4000);               // Allow generator to stabilize

    breathGray.setActive(false);

    setParameters(commandDelayMs, mainDuty, mainWave, mainAmplitude);
}

// Send frequency command to the FY3200S generator
void setFrequency(ChannelType channel, unsigned char chakra, unsigned char level)
{
    unsigned long freq;

    if (channel == CHAKRA) {
        // chakra: 0–6 (Crown to Root), level: 0–3 intensity
        freq = (*Chakra[chakra % 7])[level % 4];
    } else {
        // SolNog uses the index as lookup into Solfeggio table
        freq = Solfeggio[level % 7];
    }

    String cmd = (channel == CHAKRA) ? "df" : "bf";
    String fullCmd = cmd + String(freq);

    Serial1.println(fullCmd);
    Serial1.flush();

    Serial.printlnf("Sent %s: %lu Hz (chakra/level = %u/%u)",
                    (channel == CHAKRA ? "Chakra" : "SolNog"), freq, chakra, level);
    
    // Publish to Particle cloud console (heavily rate-limited to avoid quota issues)
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 300000) {  // max once every 5 minutes
        Particle.publish("SSFH-PEMF", String::format("%s %luHz (c%u/l%u)", 
                        (channel == CHAKRA ? "CH" : "SN"), freq, chakra, level), PRIVATE);
        lastPublish = millis();
    }
}

// Update RGB LED and D7 LED to reflect current Chakra vs SolNog dominance
void updateLEDs(unsigned char solnogIndex, unsigned char chakraIndex)
{
    // Reset all status LEDs first
    blinkMagenta.setActive(false);
    solidOrange.setActive(false);
    solidGreen.setActive(false);

    if (solnogIndex == chakraIndex) {
        // Sync state — this is the "stand in awe" magenta moment
        digitalWrite(ledPin, !digitalRead(ledPin));
        blinkMagenta.setActive(true);
        reseedRNG();
    }
    else if (solnogIndex > chakraIndex) {
        digitalWrite(ledPin, HIGH);
        solidOrange.setActive(true);
    }
    else {
        digitalWrite(ledPin, LOW);
        solidGreen.setActive(true);
    }
}

void loop()
{
    // Publish initialization event once cloud is ready
    static bool publishedInit = false;
    if (!publishedInit && Particle.connected()) {
        Particle.publish("SSFH-PEMF", "Device initialized - starting frequency hopping", PRIVATE);
        publishedInit = true;
    }

    unsigned long now = millis();

    // Master timer — controls random reseeding and overall rhythm
    // Master timer — controls random reseeding and overall rhythm
    static CustomTimer longTimer(longPeriodMs);
    if (longTimer.check(now)) {
        currentChakra      = random(7);   // 0 = Crown ... 6 = Root
        currentSolNogIndex = random(7);
        updateLEDs(currentSolNogIndex, currentChakra);
    }

    // Chakra frequency hopping
    static CustomTimer chakraTimer(chakraPeriodMs);
    if (chakraTimer.check(now)) {
        currentChakra      = random(7);   // 0 = Crown ... 6 = Root
        currentChakraLevel = random(4);   // intensity level for that chakra
        setFrequency(CHAKRA, currentChakra, currentChakraLevel);
        updateLEDs(currentSolNogIndex, currentChakra);
    }

    // SolNog (Solfeggio + Nogier) frequency hopping
    static CustomTimer solnogTimer(solnogPeriodMs);
    if (solnogTimer.check(now)) {
        currentSolNogIndex = random(3);
        setFrequency(SOLNOG, currentSolNogIndex, currentSolNogIndex);
        updateLEDs(currentSolNogIndex, currentChakra);
    }
}

// Reseed the random number generator using hardware RNG
void reseedRNG()
{
    rngSeed = HAL_RNG_GetRandomNumber();
    randomSeed(rngSeed);
}

// Initialize the FY3200S generator with base waveform, duty, amplitude, etc.
// Sends both "main" and "secondary" channel parameter sets.
void setParameters(unsigned long delayMs, unsigned int duty, unsigned char wave, unsigned int amplitude)
{
    digitalWrite(ledPin, !digitalRead(ledPin));

    // Main channel parameters (bd = duty, bw = wave, ba = amplitude)
    const char* mainCmds[3] = {"bd", "bw", "ba"};
    const unsigned long values[3] = {duty, wave, amplitude};
    LEDStatus* mainLEDs[3] = {&solidOrange, &solidGreen, &solidRed};

    for (int i = 0; i < 3; ++i) {
        mainLEDs[i]->setActive(true);
        String cmd = String(mainCmds[i]) + String(values[i]);
        Serial1.println(cmd);
        Serial1.flush();
        delay(delayMs);
    }

    digitalWrite(ledPin, !digitalRead(ledPin));

    // Secondary channel parameters (different order: da, dw, dd)
    const char* secondaryCmds[3] = {"da", "dw", "dd"};
    LEDStatus* secondaryLEDs[3] = {&solidRed, &solidGreen, &solidOrange};

    for (int i = 0; i < 3; ++i) {
        String cmd = String(secondaryCmds[i]) + String(values[2 - i]);
        Serial1.println(cmd);
        Serial1.flush();
        delay(delayMs);
        secondaryLEDs[i]->setActive(false);
    }

    digitalWrite(ledPin, !digitalRead(ledPin));
}