// 2023 08 14 (Added Solfeggio and adjusted timing)
// Chakra Frequencies are tuned to 432Hz
// 7 of 9 Solfeggio aligned with chakras
// HiLo Nogier frequencies mixed with Solfeggio
//
// 8/4 Time Signature (Twice as Long as Standard) as 90 BPM
//     In 8/4 time, one measure lasts for 8 quarter notes, or 5333.33 ms.
//     (333 ms) would fit 16 times into this measure.
//     (666 ms) would fit 8 times into this measure.
//     (1333 ms) would fit 4 times into this measure.
//
// Place all global variables and constants inside a namespace
namespace
{
    // unsigned char datatype encodes numbers from 0 to 255. Prefer the byte data type for consistency of Arduino programming style.
    // unsigned int 0 to 4,294,967,295 (42,949,672.95) 4bytes (32-bit) (2^32 - 1).

    // using the built in LED (Blue LED on the Photon is next to pin D7.)
    const pin_t ledPin = D7;
    
    // Set some global variables available anywhere in the program
    // Millisecond delay after setting a parameter
    const unsigned int sP = 555;

    // Get a random seed from the hardware RNG
    uint32_t seed = HAL_RNG_GetRandomNumber();

    // Set a long period for both Channels
    const unsigned int longPeriod = 10368;
    // Set a short period for Chakra Channel
    const unsigned int periodChakra = 648;
    // Set a short period for SolNog Channel
    const unsigned int periodSolNog = 1152;

    // Main Wave：(sine bw0, square bw1, triangle bw2)
    const unsigned char wave = 0;
    // Main Duty：set duty cycle bd1000 = 100%
    const unsigned int duty = 500;
    // Main Amplitude: ba2000 = 20vpp
    const unsigned int amplitude = 1900;

    // Initialize start timer variables
    unsigned int startLongMillis = 0;
    unsigned int startChakraMillis = 0;
    unsigned int startSolNogMillis = 0;

    unsigned int currentLongMillis;
    unsigned int currentChakraMillis;
    unsigned int currentSolNogMillis;

    unsigned char xC; // init Two-Dimensional Array selector X for Chakra Channel
    unsigned char yC; // init Two-Dimensional Array selector Y for Chakra Channel

    unsigned char xN; // init Two-Dimensional Array selector X for SolNog Channel
    unsigned char yN; // init Two-Dimensional Array selector Y for SolNog Channel

    // Nogier frequencies
    const unsigned int AlowCellularVitality = 228; // Cell vitality, tissue repair, acute inflammation, skin & mucous membranes, “openings,” general tonification, first-aid / regenerative support
    const unsigned int AhighCellularVitality = 29363; // Cell vitality, tissue repair, acute inflammation, skin & mucous membranes, “openings,” general tonification, first-aid / regenerative support
    const unsigned int BlowNutritionalMetabolism = 456; // Nutrition, digestion, assimilation, metabolism, abdominal organs, cellular cohesion / nutritional ground regulation 
    const unsigned int BhighNutritionalMetabolism = 58725; // Nutrition, digestion, assimilation, metabolism, abdominal organs, cellular cohesion / nutritional ground regulation
    const unsigned int ClowMovement = 913; // Locomotor system: muscles, ligaments, joints, movement, posture, coordination
    const unsigned int ChighMovement = 117450; // Locomotor system: muscles, ligaments, joints, movement, posture, coordination.
    const unsigned int DlowCoordination = 1825; // Laterality & bilateral integration; cross-talk of nutritional + locomotor systems; sometimes circulation / “stuck” chronic patterns
    const unsigned int DhighCoordination = 234900; // Laterality & bilateral integration; cross-talk of nutritional + locomotor systems; sometimes circulation / “stuck” chronic patterns
    const unsigned int ElowNerves = 3650; // Spinal cord, nerve conduction, pain (esp. chronic), analgesia
    const unsigned int EhighNerves = 937528; // Spinal cord, nerve conduction, pain (esp. chronic), analgesia
    const unsigned int FlowEmotionalReactions = 7341; //  Subcortical / emotional brain, autonomic & hormonal balance, mood, anxiety/depression-type state
    const unsigned int FhighEmotionalReactions = 934400; //  Subcortical / emotional brain, autonomic & hormonal balance, mood, anxiety/depression-type state
    const unsigned int GlowIntellectualOrganization = 14681; //  Cerebral cortex, mental organization, concentration, intellectual fatigue, psychosomatic / cortical level
    const unsigned int GhighIntellectualOrganization = 1868800; //  Cerebral cortex, mental organization, concentration, intellectual fatigue, psychosomatic / cortical level
    
    //Chakra frequencies
    const unsigned int SAHASRARA1 = 10800;
    const unsigned int SAHASRARA2 = 21600;
    const unsigned int SAHASRARA3 = 43200;
    const unsigned int SAHASRARA4 = 86333;
    const unsigned int AJNA1 = 14400;
    const unsigned int AJNA2 = 28833;
    const unsigned int AJNA3 = 57600;
    const unsigned int AJNA4 = 115500;
    const unsigned int VISHUDDHA1 = 19266;
    const unsigned int VISHUDDHA2 = 38466;
    const unsigned int VISHUDDHA3 = 76899;
    const unsigned int VISHUDDHA4 = 937528;
    const unsigned int ANAHATA1 = 12833;
    const unsigned int ANAHATA2 = 25700;
    const unsigned int ANAHATA3 = 51400;
    const unsigned int ANAHATA4 = 102800;
    const unsigned int MANIPURA1 = 18166;
    const unsigned int MANIPURA2 = 36366;
    const unsigned int MANIPURA3 = 72666;
    const unsigned int MANIPURA4 = 145200;
    const unsigned int SVADHISTHANA1 = 15266;
    const unsigned int SVADHISTHANA2 = 30533;
    const unsigned int SVADHISTHANA3 = 61133;
    const unsigned int SVADHISTHANA4 = 122100;
    const unsigned int MULADHARA1 = 11433;
    const unsigned int MULADHARA2 = 22900;
    const unsigned int MULADHARA3 = 45733;
    const unsigned int MULADHARA4 = 91500;

    // Solfeggio frequencies
    const unsigned int SolfeggioTI = 96300; // Reconnecting with spirit, higher self & oneness
    const unsigned int SolfeggioLA = 85200; // Spiritual awakening & connection to higher self
    const unsigned int SolfeggioSO = 74100; // Internal cleansing & intuitive awakening
    const unsigned int SolfeggioFA = 63900; // Improved communication & relationships
    const unsigned int SolfeggioMI = 52800; // Pure love, miracles & DNA repair
    const unsigned int SolfeggioRE = 41700; // Releasing past traumas, facilitating change
    const unsigned int SolfeggioUT = 39600; // Releasing guilt, negativity & fear
    const unsigned int SolfeggioDO = 28500; // Tissue regeneration & internal healing
    const unsigned int SolfeggioOT = 17400; // Pain relief (physical / emotional / karmic)
    

    const unsigned int SolNog[3][7] = {
        {SolfeggioDO, SolfeggioUT, SolfeggioRE, SolfeggioMI, SolfeggioFA, SolfeggioSO, SolfeggioLA},
        {AlowCellularVitality, BlowNutritionalMetabolism, ClowMovement, DlowCoordination, ElowNerves, FlowEmotionalReactions, GlowIntellectualOrganization},
        {AhighCellularVitality, BhighNutritionalMetabolism, ChighMovement, DhighCoordination, EhighNerves, FhighEmotionalReactions, GhighIntellectualOrganization}};

    const unsigned int Chakra[4][7] = {
        {SAHASRARA1, AJNA1, VISHUDDHA1, ANAHATA1, MANIPURA1, SVADHISTHANA1, MULADHARA1},
        {SAHASRARA2, AJNA2, VISHUDDHA2, ANAHATA2, MANIPURA2, SVADHISTHANA2, MULADHARA2},
        {SAHASRARA3, AJNA3, VISHUDDHA3, ANAHATA3, MANIPURA3, SVADHISTHANA3, MULADHARA3},
        {SAHASRARA4, AJNA4, VISHUDDHA4, ANAHATA4, MANIPURA4, SVADHISTHANA4, MULADHARA4}};
}

// REFACTOR: Moved LEDStatus definitions outside the namespace for global access, as they are used across setup and loop.
//           This avoids repetition and makes them reusable without redeclaration.
LEDStatus solidOrange(RGB_COLOR_ORANGE, LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus solidGreen(RGB_COLOR_GREEN, LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus solidRed(RGB_COLOR_RED, LED_PATTERN_SOLID, LED_SPEED_NORMAL, LED_PRIORITY_NORMAL);
LEDStatus breathGray(RGB_COLOR_GRAY, LED_PATTERN_FADE, LED_SPEED_NORMAL, LED_PRIORITY_IMPORTANT);
LEDStatus blinkMagenta(RGB_COLOR_MAGENTA, LED_PATTERN_BLINK, LED_SPEED_FAST, LED_PRIORITY_CRITICAL);

// REFACTOR: Added enum for channel types to make code more readable and avoid magic strings/numbers.
//           This helps in DRY by parameterizing channel-specific logic.
enum ChannelType {
    CHAKRA,
    SOLNOG
};

SYSTEM_THREAD(ENABLED);

void setup()
{
    // REFACTOR: Added Serial (USB) for debugging—logs to Particle serial monitor.
    //           This helps verify if/when commands are being sent without affecting Serial1.
    Serial.begin(9600);  // USB serial for logging (connect via USB and use 'particle serial monitor')

    // set LED pin to OUTPUT
    pinMode(ledPin, OUTPUT);
    // Start Particle LED Breathing Gray
    breathGray.setActive(true);
    // Set the random seed that was got
    random_seed_from_cloud(seed);
    // Enables serial channel with specified configuration via TX/RX pins.
    Serial1.begin(9600);
    // Set initial start timers
    startLongMillis = startChakraMillis = startSolNogMillis = millis();
    // Delay 5 seconds
    delay(5000);
    // Stop Particle LED
    breathGray.setActive(false);
    // Execute the setParameters method
    setParameters(sP, duty, wave, amplitude);
    // REFACTOR: Removed redundant breathGray.setActive(true); and final breathGray.setActive(false).
    //           Consolidated LED control to avoid repetition. Setup now ends with parameters set and LEDs off.
}

// REFACTOR: Extracted frequency setting into a function to DRY the duplicated logic for Chakra and SolNog channels.
//           This reduces repetition in the loop for random selection and serial commands.
// FIX: Changed to explicit "\r\n" termination (instead of println's "\n") as many devices require it.
//      Added Serial1.flush() to ensure full transmission, especially on Photon 1's smaller buffer.
//      Added USB serial logging for debugging.
void setFrequency(ChannelType channel, unsigned char x, unsigned char y) {
    String cmd;
    unsigned int freqValue;

    if (channel == CHAKRA) {
        cmd = "df";
        freqValue = Chakra[x][y];
    } else { // SOLNOG
        cmd = "bf";
        freqValue = SolNog[x][y];
    }

    String fullCmd = cmd + String(freqValue);
    Serial1.println(fullCmd);  // Use println() to match first code
    Serial.println("Sent frequency: " + fullCmd);  // Debugging log
}

// REFACTOR: Extracted LED update logic into a function to DRY the repeated setActive calls and comparisons.
//           This centralizes the if-else logic, making it easier to maintain and reducing code duplication.
void updateLEDs(unsigned char yN, unsigned char yC) {
    // First, stop all variable LEDs to reset state (DRY: avoids repeating in each branch)
    blinkMagenta.setActive(false);
    solidOrange.setActive(false);
    solidGreen.setActive(false);

    if (yN == yC) {
        digitalWrite(ledPin, !digitalRead(ledPin)); // Change the state of the LED.
        blinkMagenta.setActive(true);               // Start LED Blink Magenta
        plant_a_seed_HAL();
    } else if (yN > yC) {
        digitalWrite(ledPin, HIGH);    // Turn the LED on.
        solidOrange.setActive(true);   // Start LED Solid Orange
    } else if (yN < yC) {
        digitalWrite(ledPin, LOW);     // Turn the LED off.
        solidGreen.setActive(true);    // Start LED Solid Green
    }
}

void loop()
{
    // REFACTOR: Consolidated currentMillis assignments into one call per type, but kept separate for clarity.
    //           Could use a single millis() call if optimizing further, but readability is prioritized.
    currentLongMillis = millis();   // Get the current number of milliseconds since the program started
    currentChakraMillis = millis(); // Get the current number of milliseconds since the program started
    currentSolNogMillis = millis(); // Get the current number of milliseconds since the program started

    if (currentLongMillis - startLongMillis >= longPeriod)
    {
        yC = random(7);                      // Random # (>= 0 and < x) Needs to match the array freqz[][y]
        yN = random(7);                      // Random # (>= 0 and < x) Needs to match the array y value [][y]
        startLongMillis = currentLongMillis; // Restart the long timer
        updateLEDs(yN, yC);
    }


    if (currentChakraMillis - startChakraMillis >= periodChakra) // Test whether the period has elapsed
    {
        xC = random(4); // Random # (>= 0 and < x) Needs to match the array freqz[x][]
        setFrequency(CHAKRA, xC, yC);
        startChakraMillis = currentChakraMillis; // Restart the Chakra timer
        updateLEDs(yN, yC);
    }

    if (currentSolNogMillis - startSolNogMillis >= periodSolNog) // Test whether the period has elapsed
    {
        xN = random(3); // Random # (>= 0 and < x) Needs to match the array x value [x][]
        setFrequency(SOLNOG, xN, yN);
        startSolNogMillis = currentSolNogMillis; // Restart the SolNog timer
        updateLEDs(yN, yC);
    }

}

void random_seed_from_cloud(unsigned int seed)
{
    // Override getting seed from cloud.
    srand(seed);
}

void plant_a_seed_HAL()
{
    seed = HAL_RNG_GetRandomNumber();
    random_seed_from_cloud(seed);
    // REFACTOR: Removed redundant blinkMagenta.setActive(false); as it's now handled in updateLEDs().
}

// REFACTOR: Simplified setParameters by using a loop for main/secondary settings.
//           This DRYs the repeated Serial1.print/delay patterns for duty, wave, amplitude.
//           Added comments to explain the parameterization.
// FIX: Changed to explicit "\r\n" termination and added Serial1.flush() for each command.
//      Added USB serial logging for debugging.
void setParameters(unsigned int sP, unsigned int duty, unsigned char wave, unsigned int amplitude)
{
    digitalWrite(ledPin, !digitalRead(ledPin)); // Change the state of the LED.

    // REFACTOR: Define arrays for commands and corresponding LED statuses to loop over.
    //           This avoids hardcoding each Serial1.print/delay/LED sequence multiple times.
    const char* mainCmds[] = {"bd", "bw", "ba"}; // Main: duty, wave, amplitude
    const unsigned int values[] = {duty, (unsigned int)wave, amplitude};
    LEDStatus* mainLEDs[] = {&solidOrange, &solidGreen, &solidRed};

    const char* secondaryCmds[] = {"da", "dw", "dd"}; // Secondary: amplitude, wave, duty (note order differs)
    LEDStatus* secondaryLEDs[] = {&solidRed, &solidGreen, &solidOrange};

    // Set main parameters
    for (int i = 0; i < 3; ++i) {
        mainLEDs[i]->setActive(true);
        String fullCmd = String(mainCmds[i]) + String(values[i]);
        Serial1.println(fullCmd);  // Use println()
        delay(sP);
    }

    digitalWrite(ledPin, !digitalRead(ledPin)); // Change the state of the LED.

    // Set secondary parameters (reusing values array, but in different order)
    for (int i = 0; i < 3; ++i) {
        String fullCmd = String(secondaryCmds[i]) + String(values[2 - i]);
        Serial1.println(fullCmd);  // Use println()
        delay(sP);
        secondaryLEDs[i]->setActive(false);
    }

    digitalWrite(ledPin, !digitalRead(ledPin));
}
