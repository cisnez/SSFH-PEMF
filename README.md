# SSFH-PEMF

**Spread Spectrum Frequency Hopping – Pulsed Electro-Magnetic Field**

This firmware runs on a **Particle Photon** (or compatible) and drives a **FeelTech FY3200S** (or similar) dual-channel function generator via serial commands.

It generates a continuously evolving therapeutic frequency field by rapidly hopping between:
- **Chakra frequencies** — 7 chakras (Crown to Root), each with 4 intensity levels (clean per-chakra arrays)
- **Solfeggio tones** — aligned with the 7 chakras
- **Nogier frequencies** — low + high paired sets for tissue, nervous system, emotional, and cognitive support

### Timing Model
- Based on an **8/4 time signature at 90 BPM** (one measure ≈ 5333 ms).
- **Master cycle**: 10.368 s — triggers new random indices and LED feedback.
- **Chakra channel**: updates every **648 ms**.
- **SolNog channel**: updates every **1.152 s**.

LED feedback on D7 + RGB:
- **Magenta blink** + D7 toggle = Chakra and SolNog indices synchronized (reseeds RNG).
- **Solid Orange** = SolNog dominant.
- **Solid Green** = Chakra dominant.

### Features
- Fixed critical timing bug (all timers now use single `millis()` capture + `unsigned long`).
- Clean `Timer` helper struct for periodic events.
- Clear, readable frequency tables (one array per chakra, no dense matrices).
- Comprehensive USB serial logging showing chakra + level for every command.
- Improved structure, naming, and documentation.

### Hardware Required
- Particle Photon (or Electron/Core with adaptation)
- FeelTech FY3200S function generator connected via TX/RX (Serial1)
- Optional: external amplifier / coil / PEMF transducer

### Compiling & Flashing
```bash
particle compile photon main.cpp
particle flash --local firmware.bin
```

Or use Particle Workbench / Web IDE.

---

**Note**: The frequency combinations and timing are experimental and intended for personal research in biofield / PEMF therapy. Use at your own risk.
