## ADDED Requirements

### Requirement: Desktop and browser distribution
The project SHALL build a desktop application for Windows, Linux, and macOS and a WebAssembly application for WebGL 2 browsers. Each distribution SHALL load its scripts, graphics, and sounds from an external asset directory.

#### Scenario: Load deployed browser assets
- **WHEN** the browser starts Green Fairy over HTTP
- **THEN** it fetches the asset files beside the WebAssembly application and initializes the instrument

### Requirement: WebXR performance
The web application SHALL expose immersive VR on browsers that support it.

#### Scenario: Play an immersive key
- **WHEN** the player enters immersive VR and selects a virtual key using a tracked input ray
- **THEN** the instrument renders the stereo views and audibly plays the selected note
