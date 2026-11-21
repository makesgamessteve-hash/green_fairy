# Green Fairy

Green Fairy is a standalone 12-voice software synthesizer for evolving pads, textures, and basses. The original retro 3D instrument panel runs with SDL3 and OpenGL on desktop, and with WebGL 2 on the web. The C engine handles audio, input, and rendering. Editable Lua scripts define the patch bank.

## Play

| Action | Keyboard | On-screen |
| --- | --- | --- |
| Play C through the next C | `A W S E D F T G Y H U J K` | Click, tap, or slide on the keys |
| Change patches | Up / Down | Patch list or arrow buttons |
| Shift the keyboard one octave | `Z` / `X` | |
| Reload an edited patch bank | `R` on desktop | Refresh on the web |
| Shape the sound | | Drag Morph, Color, or Echo |

The web app uses larger touch keys on mobile. The browser enables audio after the first interaction. **Enter VR** appears when the browser supports immersive WebXR. In VR, aim a controller at the instrument and press the trigger to play or adjust a control.

## Build

Use CMake 3.24 or later and a C11 toolchain. The first configure downloads the pinned SDL3 and Lua sources.

On Debian and Ubuntu, install the X11, OpenGL, and audio development packages.

```sh
sudo apt-get install libasound2-dev libpulse-dev libx11-dev libxext-dev \
  libxrandr-dev libxi-dev libxcursor-dev libxfixes-dev libgl-dev libegl-dev
```

### Linux, Windows, and macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
cmake --install build --config Release --prefix dist
```

Run `dist/green-fairy` on Linux, `dist/green-fairy.exe` on Windows, or `dist/green-fairy.app` on macOS. Desktop requires OpenGL 3.2 with a core profile.

### Web and WebXR

Activate the Emscripten 4.0.23 SDK and build the web distribution.

```sh
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web --parallel
python3 -m http.server 8080 --directory build-web/web
```

Open `http://localhost:8080`. Deploy the contents of `build-web/web` together. Use HTTPS or localhost for immersive WebXR.

## Extend

Edit the installed `assets/` directory alongside the executable on Windows and Linux. On macOS it lives in the app bundle under `Contents/Resources/assets/`. On the web it lives alongside `instrument.wasm`.

- **Scripts.** Edit `assets/scripts/bank.lua`. Return an ordered array of 1–24 patch tables. Omitted fields take their defaults. Lua arithmetic and the `math` library are available.
- **Sounds.** Add a WAV file to `assets/sounds/`, put its filename in `sample_file`, and set the `sample` gain. The engine plays it as a one-shot pitched relative to C4. For example, the bundled Glass Canopy patch layers `canopy.wav`.
- **Graphics.** Replace `assets/graphics/prism.obj` with an OBJ containing up to 256 vertices and 512 triangular faces. Use coordinates from −2 to 2 and plain one-based face indices. Replace `studio.bmp` to reskin the glyphs and sprites. Keep the 192 × 160 dimensions and existing tile locations.
- **Web files.** Add each new asset referenced by a script to `assets/manifest.json`. The web app fetches the files at startup, so a page refresh picks up deployed edits.

| Lua fields | Values |
| --- | --- |
| `name`, `sample_file` | Text up to 31 bytes. Use a filename under `assets/sounds/` for samples. |
| `wave_a`, `wave_b` | `sine`, `triangle`, `saw`, `square`, `noise` |
| `attack`, `decay`, `release` | Seconds. Attack and decay 0–10, release 0–20. |
| `sustain`, `morph`, `color`, `echo`, `motion`, `sample` | 0–1 |
| `detune` | −1200 to 1200 cents |
| `fm` | 0–8 |
| `lfo` | 0–20 Hz |

The entry point and SDL integration are in `src/app.c`. The DSP and Lua loader are in `src/synth.c` and `src/bank.c`. Third-party notices are included under `assets/`.

## Verify

GitHub Actions builds the native targets for Linux, Windows, and macOS, runs desktop and browser integration checks, and enforces 95% unit line and branch coverage across the C DSP and Lua loader. Use the following commands locally with GCC, gcovr, and a built web distribution.

```sh
cmake -S . -B build-core -DGF_BUILD_APP=OFF -DGF_COVERAGE=ON
cmake --build build-core --parallel
ctest --test-dir build-core --output-on-failure
python3 -m gcovr --root . --filter 'src/(synth|bank)\.c$' --fail-under-line 95 --fail-under-branch 95 build-core

SDL_AUDIODRIVER=dummy LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a ./build/green-fairy --smoke
python3 -m pip install playwright==1.65.2
python3 -m playwright install chromium
python3 tests/browser.py build-web/web
```

The browser suite exercises desktop keyboard and pointer input, mobile touch, editable assets, and simulated stereo WebXR rendering and controller input. The OpenSpec change lives under `openspec/changes/create-green-fairy`.

```sh
npm exec --yes --package @fission-ai/openspec@2.1.1 -- openspec validate create-green-fairy --strict
```
