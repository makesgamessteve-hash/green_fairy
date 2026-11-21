## ADDED Requirements

### Requirement: Polyphonic synthesis
The synth SHALL render stereo audio with multiple simultaneous notes and support performance changes to oscillator morph, filter color, and delay.

#### Scenario: Play and release a chord
- **WHEN** the player presses several notes and releases them
- **THEN** the engine mixes their voices and finishes each release envelope

### Requirement: Lua patch bank
The synth SHALL load an ordered bank of 1 to 100 patches from an external Lua file and support optional external WAV layers. The distributed bank SHALL contain 100 playable presets.

#### Scenario: Apply a valid edit
- **WHEN** a valid Lua patch bank is loaded
- **THEN** the application offers each patch in order and applies its configured sound when selected

#### Scenario: Load the factory presets
- **WHEN** the synth opens the distributed bank
- **THEN** it loads all 100 presets and each preset produces audio when played

#### Scenario: Reject invalid data
- **WHEN** a replacement Lua bank fails to load or validate
- **THEN** the loader reports the error and preserves the previous bank

### Requirement: Per-voice subtractive filtering
A preset SHALL be able to select a resonant low-pass, band-pass, or high-pass filter for each voice. Filter cutoff SHALL respond to its own ADSR envelope, note velocity, and keyboard tracking.

#### Scenario: Play a chord with independent filter motion
- **WHEN** two notes in a resonant preset start with different pitches or velocities
- **THEN** each has its own filter state, cutoff, and envelope, and releasing one note does not release the other

### Requirement: Modulation routing
A preset SHALL support up to eight signed modulation routes and an optional per-voice multisegment envelope that can loop while a note is held.

#### Scenario: Route a looping shape
- **WHEN** the player holds a note in a preset that routes a looping shape to oscillator Morph
- **THEN** the oscillator blend follows the segment targets and repeats while the note is held

### Requirement: Granular WAV playback
A preset SHALL be able to layer grains from an external WAV, with independently configurable grain duration, density, start position, and spray.

#### Scenario: Sustain a granular texture
- **WHEN** a note is held beyond the duration of the underlying WAV
- **THEN** overlapping, windowed grains continue at note pitch until the voice releases
