## ADDED Requirements

### Requirement: Retro 3D instrument
The instrument SHALL display an animated 3D scene from replaceable external files. Each factory preset SHALL specify a distinct visual profile in the Lua bank.

#### Scenario: Play a visible patch
- **WHEN** the player changes the patch and plays a note
- **THEN** the selected patch, keys, and output meter reflect the performance

#### Scenario: Switch to another preset
- **WHEN** the player selects a different factory preset
- **THEN** the signal field adopts that preset's mesh, palette, and motion settings while continuing to respond to sound

#### Scenario: Load a custom bank or a missing model
- **WHEN** a patch has no scene data or its custom OBJ cannot be loaded
- **THEN** the signal field uses the default prism and the patch remains playable

### Requirement: Keyboard, pointer, and touch
The instrument SHALL support physical key bindings and on-screen controls, and SHALL select a larger touch layout on mobile browsers.

#### Scenario: Browse a full preset bank
- **WHEN** the player browses presets with the on-screen arrows or keyboard shortcuts
- **THEN** the controls move one or ten presets, wrap around at the ends of the bank, and show the current position

#### Scenario: Play on a mobile browser
- **WHEN** a touch device opens the web instrument and taps a key
- **THEN** the layout provides touch-sized keys and the instrument plays and releases the note

### Requirement: Preset type filters and favorites
The instrument SHALL provide a preset type dropdown and a star control for the active preset. It SHALL save favorites locally using stable patch identifiers.

#### Scenario: Browse one sound family
- **WHEN** the player chooses a preset type
- **THEN** the preset list, navigation, and count reflect only that sound family

#### Scenario: Recall starred sounds
- **WHEN** the player stars a preset, restarts the instrument, and chooses Favorites
- **THEN** the preset appears in the filtered bank and can be played or unstarred

### Requirement: Scale-locked performance keyboard
The instrument SHALL map the 26 letters and 10 digits to consecutive pitches in the selected tuning. Mouse, touch, and XR pads SHALL use the same mapping and display each pitch.

#### Scenario: Play a multi-octave scale
- **WHEN** the player selects C Major and plays Z through 0 in the default register
- **THEN** the notes ascend by C-major degrees from C2 to C7

#### Scenario: Select an Arabic tuning
- **WHEN** the player chooses D Bayati from the dropdown
- **THEN** the keyboard plays the quarter-tone scale, including the second degree 150 cents above the root

### Requirement: Custom tuning builder
The player SHALL be able to select a root and toggle 50-cent intervals. The instrument SHALL save the selected tuning, register, and applied custom tuning across sessions.

#### Scenario: Apply and recall a custom tuning
- **WHEN** the player enables at least four degrees including the root and applies the draft
- **THEN** the keyboard adopts the custom pitches and restores the selection on the next launch

#### Scenario: Keep all keys in range
- **WHEN** the player selects a sparse tuning or shifts the register beyond its limits
- **THEN** all 36 key pitches stay inside the supported MIDI-equivalent pitch range
