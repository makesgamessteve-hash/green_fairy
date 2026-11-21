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
