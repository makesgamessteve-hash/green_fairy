## Context

A single C instrument serves desktop, mobile browsers, and immersive WebXR. SDL3 owns the window, input, and audio stream. Lua supplies patch data. OpenGL and WebGL share renderer geometry and layout.

## Goals / Non-Goals

**Goals** include low-latency polyphony, original retro 3D artwork, editable patches and assets, release builds for desktop and web, and repeatable verification.

**Non-goals** for the first release include DAW plug-in formats, hardware MIDI, and patch automation recording.

## Decisions

- Build the DSP and Lua bank loader as a standalone library. Patch loading and WAV conversion run on the event thread. SDL's audio stream lock protects engine changes and sample ownership.
- Load the same asset tree beside the desktop executable or app bundle. The web shell fetches the manifest and files over HTTP into Emscripten's virtual filesystem before C startup.
- Render sprites and triangle meshes in the SDL scene. WebXR provides stereo projection and tracked input rays to the same instrument.
- Require at least 95% unit line and branch coverage across the synth engine and Lua bank loader. Exercise application input, audio, external assets, and platform integration separately in automated end-to-end tests.

## Risks / Trade-offs

Browser audio starts on user interaction. XR uses immersive VR on compatible browsers with a secure origin. Headless CI simulates XR sessions and controller input. Headset behavior also depends on the device runtime.
