## ADDED Requirements

### Requirement: Retro 3D instrument
The instrument SHALL display an animated 3D mesh and sprites from replaceable external files.

#### Scenario: Play a visible patch
- **WHEN** the player changes the patch and plays a note
- **THEN** the selected patch, keys, and output meter reflect the performance

### Requirement: Keyboard, pointer, and touch
The instrument SHALL support physical key bindings and on-screen controls, and SHALL select a larger touch layout on mobile browsers.

#### Scenario: Play on a mobile browser
- **WHEN** a touch device opens the web instrument and taps a key
- **THEN** the layout provides touch-sized keys and the instrument plays and releases the note
