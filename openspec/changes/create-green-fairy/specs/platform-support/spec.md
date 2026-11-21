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

### Requirement: GitHub Pages deployment
The repository SHALL publish the tested web distribution to GitHub Pages from `main` when Pages is configured to use GitHub Actions.

#### Scenario: Deploy the verified web build
- **WHEN** a push or manual workflow run on `main` passes the web and coverage checks
- **THEN** the deployment publishes the same web build and its external assets to the project's Pages URL

#### Scenario: Load from a project site
- **WHEN** a visitor opens the instrument under the repository path on GitHub Pages
- **THEN** the scripts, WebAssembly, and runtime assets load from that project path
