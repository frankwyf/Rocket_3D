# SkyRocket 3D Knowledge Base

This document consolidates key project knowledge into repository-native Markdown for long-term maintenance.

## Architecture Snapshot

- Main loop and rendering orchestration are implemented in `main/main.cpp`.
- Rendering relies on OpenGL draw calls with VAO/VBO/EBO resource management.
- Math primitives and transformations are provided by `vmlib/`.
- Supporting modules (`support/`) handle shader program setup, debug output, and error handling.

## Runtime Systems

- **Rendering**: Mesh submission, texture binding, shader uniforms, camera matrices.
- **Input**: GLFW keyboard and mouse callbacks.
- **Assets**: OBJ mesh loading (`loadobj.*`) and image textures (`load_texture.*`).
- **Gameplay**: Rocket state, launch/reset behavior, mode toggles.

## Build and Tooling

- C++ standard: `C++17`
- Project generation: `premake5`
- IDE/toolchain: Visual Studio on Windows x64
- Solution file: `Rocket3D.sln`

## Open-source Readiness Checklist

- [x] Clear top-level `README` with build and usage references
- [x] Contribution and conduct docs (`CONTRIBUTING.md`, `CODE_OF_CONDUCT.md`)
- [x] Third-party license disclosure (`third_party.md`)
- [x] Changelog (`CHANGELOG.md`)
- [x] Multilingual usage guides (`docs/usage.*.md`)

## Notes

- No `.pdf`, `.doc`, or `.docx` report files were found in the repository at the time of consolidation.
- This file should be updated whenever architecture or build workflows change.
