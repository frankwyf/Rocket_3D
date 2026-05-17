# SkyRocket 3D

An interactive 3D rocket flight simulator built with `C++17`, `OpenGL 4.3`, and `GLFW`.

## Documentation

- English usage guide: [`docs/usage.en.md`](docs/usage.en.md)
- How to play (English): [`docs/HOW_TO_PLAY.md`](docs/HOW_TO_PLAY.md)
- 中文使用说明: [`docs/usage.zh-CN.md`](docs/usage.zh-CN.md)
- 日本語ガイド: [`docs/usage.ja.md`](docs/usage.ja.md)
- Development notes: [`DEVELOPMENT.md`](DEVELOPMENT.md)
- Project knowledge base (consolidated from report content): [`docs/knowledge-base.md`](docs/knowledge-base.md)
- Contribution guide: [`CONTRIBUTING.md`](CONTRIBUTING.md)
- Third-party licenses: [`third_party.md`](third_party.md)

## Features

- Interactive 3D rocket model and flight controls
- Multiple camera views and free-look navigation
- Split-screen view toggle
- Particle-based visual effects
- Textured assets with OBJ model loading

## Technical Stack

- Language: `C++17`
- Graphics API: `OpenGL 4.3`
- Window/Input: `GLFW3`
- Model loading: `rapidobj`
- Texture loading: `stb_image`
- Build generation: `premake5`

## Build

### Prerequisites

- Windows 10/11 (x64)
- Visual Studio 2019 or later

### Build commands

```bash
premake5.exe vs2019
start Rocket3D.sln
msbuild Rocket3D.sln /p:Configuration=Release /p:Platform=x64
```

## Run Modes

- **Play mode (default):** optimized for gameplay, no per-frame profiling overhead.
- **Profile mode:** run with `--profile` (or `--mode=profile`) to enable GPU timing output.

```bash
# Play mode
bin/main-*.exe

# Profile mode
bin/main-*.exe --profile
```

## Tests

Tests are separated from the game runtime and built as an independent executable:

```bash
bin/vmlib-test-*.exe
```

This avoids test/profiling load while playing.

## Mini-game Progression

The game now starts with a **pre-launch start menu**. Select a mode, then click **START**.

Available modes:
- **Campaign**: progressive objective rounds (launch/land, split-view challenge, camera challenge)
- **Time Attack**: shorter timer rounds, requires at least one bonus pickup before landing
- **Collector**: requires collecting all bonus pickups before landing

Each round has a countdown timer. Successful completion grants base score plus time bonus. Failing objective conditions applies score penalty and retries the round.

`R` or the green button resets the current run.

## Repository Structure

```text
assets/          Runtime assets (models, textures, shaders)
main/            Application and gameplay code
support/         Error handling, shader utilities, debug helpers
vmlib/           Math library (vector/matrix types)
vmlib-test/      Unit tests for math primitives
third_party/     Vendored dependencies
```

## Controls (Quick Reference)

- Movement: `W`, `A`, `S`, `D`, `Q`, `E`
- Vertical: `SPACE`, `CTRL`
- Launch/reset: `F`, `R`
- View modes: `V`, mouse look, wheel zoom
- Exit: `ESC`

## License

Licensed under the MIT License. See [`LICENSE`](LICENSE).
