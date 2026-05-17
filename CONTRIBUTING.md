# Contributing to SkyRocket 3D

Thank you for considering contributing to SkyRocket 3D! This document provides guidelines and instructions for contributing.

## How to Contribute

### Reporting Bugs

Before creating bug reports, please check the issue list as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible:

* **Use a clear and descriptive title**
* **Describe the exact steps which reproduce the problem**
* **Provide specific examples to demonstrate the steps**
* **Describe the behavior you observed after following the steps**
* **Explain which behavior you expected to see instead and why**
* **Include screenshots and animated GIFs if applicable**
* **Include your environment details** (OS, Visual Studio version, GPU, etc.)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

* **Use a clear and descriptive title**
* **Provide a step-by-step description of the suggested enhancement**
* **Provide specific examples to demonstrate the steps**
* **Describe the current behavior and expected behavior**
* **Explain why this enhancement would be useful**

### Pull Requests

* Fill in the required template
* Follow the C++ and OpenGL styleguides
* Document new code thoroughly
* End all files with a newline

## Styleguides

### C++ Styleguide

* Use 4 spaces for indentation (no tabs)
* Use `camelCase` for function and variable names
* Use `PascalCase` for class and struct names
* Use `UPPER_CASE` for constants
* Use meaningful variable names
* Keep functions focused and reasonably sized
* Use comments to explain complex logic, not what the code does

### OpenGL/GLSL Styleguide

* Follow the same naming conventions as C++
* Use consistent formatting for shader code
* Comment complex shader algorithms
* Optimize shader performance where possible

### Git Commit Messages

* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line

### Documentation Styleguide

* Use Markdown for documentation
* Include code examples where appropriate
* Keep documentation up-to-date with code changes

## Development Setup

### Prerequisites
- Visual Studio 2019 or later
- Windows 10/11 (x64)
- Git

### Setup Instructions

```bash
# Clone the repository
git clone https://github.com/username/SkyRocket3D.git
cd SkyRocket3D

# Generate project files
premake5.exe vs2019

# Open in Visual Studio
start SkyRocket3D.sln
```

### Building for Development

```bash
# Build debug version (recommended for development)
msbuild SkyRocket3D.sln /p:Configuration=Debug /p:Platform=x64

# Build release version for testing
msbuild SkyRocket3D.sln /p:Configuration=Release /p:Platform=x64
```

## Testing

* Write tests for new functionality
* Run existing tests to ensure nothing breaks
* Test on multiple GPU vendors if possible (NVIDIA, AMD, Intel)

## Project Structure

- `main/` - Main application source code
- `assets/` - Shaders, models, and textures
- `support/` - Support utilities and error handling
- `vmlib/` - Math library (vectors, matrices)
- `third_party/` - External dependencies

## Areas for Contribution

### High Priority
- [ ] Performance optimizations
- [ ] Bug fixes
- [ ] Documentation improvements
- [ ] Cross-platform compatibility (macOS, Linux)

### Medium Priority
- [ ] New visual effects
- [ ] Enhanced particle system
- [ ] Improved physics
- [ ] Camera improvements

### Low Priority
- [ ] UI improvements
- [ ] New rocket models
- [ ] Sound system integration
- [ ] Controller support

## Questions?

Feel free to open an issue with your question or contact the maintainer.

---

Thanks for contributing! ??
