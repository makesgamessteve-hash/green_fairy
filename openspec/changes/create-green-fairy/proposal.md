## Why

Green Fairy needs a first playable instrument for evolving pads, textures, and bass sounds. A native C engine and editable Lua patch bank let musicians extend the instrument after installation.

## What Changes

- Add a polyphonic C synthesizer with oscillator morphing, envelopes, filtering, modulation, stereo delay, and optional WAV layers.
- Build an original 3D instrument panel around a sprite field, mesh, patch selector, performance controls, and playable keys.
- Package external graphics, sounds, and scripts on desktop and web.
- Add repeatable platform builds, unit coverage enforcement, and automated end-to-end checks.

## Capabilities

### New Capabilities

- `synth-engine` supports playable polyphony and editable Lua patches.
- `instrument-interface` supports performance with a computer keyboard, pointer, and touch.
- `platform-support` supports SDL3 desktop, Emscripten web, and immersive WebXR sessions.

### Modified Capabilities

None.

## Impact

Adds an application, external asset tree, CMake targets, tests, and a GitHub Actions workflow.
