# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Particle effect system for rocket thrust
- Score tracking system
- Split-screen mode for dual control
- Multiple camera modes
- Speed modulation controls
- Landing pad collision detection
- GitHub Actions CI workflow for Windows (`Debug` + `Release`) builds on every push/PR.
- Unit tests for start-menu hit-testing and game configuration reset behavior.
- More resilient OBJ loading fallback behavior for missing normals/UV/material assignments.
- Reusable telemetry overlay module for HUD analytics (`main/telemetry_overlay.hpp`) with risk scoring, sparkline geometry generation, and radar marker generation.
- New unit test coverage for telemetry overlay and gameplay mechanics lifecycle scenarios.
- New menu boundary test suite for robust start-screen hit detection edge validation.
- Tri-lingual best-practices guides (`EN`/`zh-CN`/`ja`) for engineering, testing, and CI quality gates.
- Dedicated GitHub Actions coverage workflow with Cobertura export and artifact upload.
- Telemetry analytics helpers for average and trend detection in HUD/overlay diagnostics.
- Recorder rolling-buffer behavior and clear/reset support for replay/session robustness.
- Replay CSV export and timestamp validation helpers for deterministic post-run analysis.
- Expanded mechanics/telemetry tests for particles, scoring, challenge timeout progression, waypoint sequencing, and analytics edge cases.

### Changed
- Improved rocket model rendering pipeline
- Enhanced shader performance
- Optimized texture loading
- CI now executes `vmlib-test` in both `Debug` and `Release` configurations.
- Material texture path resolution now follows the loaded OBJ directory instead of relying on a fixed `assets/` prefix.
- GPU frame timing in test mode now uses non-blocking query polling and reports values in milliseconds.
- Timed power-up effects now automatically expire and expose remaining duration query support.
- CI workflow upgraded to matrix-based configuration with JUnit report export and artifact publishing.
- Coverage pipeline now enforces a minimum line-coverage threshold in GitHub Actions.
- CI and coverage workflows now use concurrency cancellation and timeout guards for faster, cleaner pipeline feedback.
- Coverage gate now supports adaptive thresholds (`measured` vs `fallback`) to keep Action reliability while nudging quality upward.

### Fixed
- Camera jitter issues
- Texture coordinate mapping bugs
- Input handling edge cases
- Startup/HUD text rendering call argument mismatches.
- Boss-gate completion state accumulation typo causing progression logic/build issues.
- Texture loading compatibility by removing unsupported anisotropy symbols for the current toolchain.

## [1.0.0] - 2024-01-15

### Added
- Initial public release
- Core rocket flight simulation
- 3D rendering with OpenGL 4.3
- OBJ model loading
- Texture mapping system
- Basic physics simulation
- GLFW-based input handling
- Multi-viewport rendering
- Landing pad system

### Features
- Interactive 3D rocket model
- Dynamic camera system
- Rocket flight controls (WASD, SPACE, CTRL)
- Mouse camera look-around
- Split-screen mode (Q key)
- Speed boost (SHIFT key)
- Position reset (R key)

### Technical
- Built with C++17
- OpenGL 4.3 core profile
- RapidOBJ for 3D model loading
- STB image library for textures
- GLFW3 for window management

---

## Version History

### Future Versions (Planned)

#### v1.1.0 (Planned)
- Advanced particle system
- Sound effects and music
- Controller input support
- Improved physics engine
- Custom rocket builder

#### v1.2.0 (Planned)
- Procedurally generated terrain
- Flight replay system
- Leaderboard system
- Multiple game modes

#### v2.0.0 (Planned)
- Multiplayer support
- VR support
- Cross-platform support (macOS, Linux)
- Advanced rendering features (PBR, shadows)

---

For migration guides and detailed technical changes, see individual version documentation.
