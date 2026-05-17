# SkyRocket 3D Usage Guide (English)

## 1. Start the Game

1. Build the project from the repository root:
   - `premake5.exe vs2019`
   - Open `Rocket3D.sln`
   - Build `Release | x64`
2. Set the startup project to `main` in Visual Studio.
3. Run with `Ctrl + F5`.

## 2. Basic Controls

| Key | Action |
|---|---|
| `W/A/S/D` | Move forward/left/backward/right |
| `Q/E` | Move up/down |
| `SPACE/CTRL` | Vertical movement |
| `SHIFT` | Speed boost |
| `Mouse` | Look around |
| `Scroll` | Zoom |
| `F` | Launch animation |
| `V` | Toggle split-screen |
| `P` | Toggle neon background |
| `R` | Reset launch animation |
| `ESC` | Exit |

## 3. Assets and Paths

- Models: `assets/models/`
- Textures: `assets/textures/`
- Shaders: `assets/shaders/`

Run from the repository root so relative asset paths resolve correctly.

## 4. Troubleshooting

- Black screen: verify shaders in `assets/shaders/` are present.
- Missing models/textures: verify the `assets/` folder is copied or reachable.
- Startup crash: update GPU drivers and rebuild `x64`.

## 5. Runtime Modes

- Play mode (default): `main` executable without extra arguments.
- Profile mode: run with `--profile` to enable per-frame GPU timing output.

## 6. Tests (Separated from Gameplay)

Run tests using the standalone `vmlib-test` executable. Tests are not run inside the game loop.

## 7. Mission Levels

Mini-campaign flow (3 rounds):
1. Round 1: launch and land near the far pad
2. Round 2: launch, toggle split view (`V`), then land
3. Round 3: launch, switch camera at least once (`C`), then land

Gameplay rules:
- Each round has a countdown timer (harder rounds have less time).
- Round clear gives base points plus time bonus.
- Timeout or missing required objective applies a score penalty and retries the same round.

Buttons now include glyph labels:
- Red button (`L`): launch
- Green button (`R`): full campaign reset
