## ADDED Requirements

### Requirement: Polyphonic synthesis
The synth SHALL render stereo audio with multiple simultaneous notes and support performance changes to oscillator morph, filter color, and delay.

#### Scenario: Play and release a chord
- **WHEN** the player presses several notes and releases them
- **THEN** the engine mixes their voices and finishes each release envelope

### Requirement: Lua patch bank
The synth SHALL load an ordered patch bank from an external Lua file and support optional external WAV layers.

#### Scenario: Apply a valid edit
- **WHEN** a valid Lua patch bank is loaded
- **THEN** the application offers each patch in order and applies its configured sound when selected

#### Scenario: Reject invalid data
- **WHEN** a replacement Lua bank fails to load or validate
- **THEN** the loader reports the error and preserves the previous bank
