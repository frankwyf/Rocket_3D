# Rocket 3D Best Practices Upgrade (System Guide)

This guide consolidates proven practices for C++17 real-time visualization/game projects and maps them to this repository.

## 1) Architecture and code boundaries

- Keep **rendering**, **gameplay rules**, and **input** decoupled.
- Prefer pure functions for gameplay calculations so logic is testable without OpenGL context.
- Keep low-level graphics code isolated from mission logic.

## 2) Data-driven gameplay

- Store level/challenge parameters in structured config (arrays/JSON) instead of hard-coded branches.
- Keep balancing numbers grouped and documented.
- Add deterministic helper modules for telemetry and scoring.

## 3) Visual analytics and observability

- Record short-window telemetry (altitude/speed/fuel) per frame.
- Render compact overlays (sparklines/risk color) for fast tuning.
- Keep analytics generation independent from rendering APIs.

## 4) Test strategy (C++17)

- Unit-test gameplay and utility modules first.
- Add boundary tests for UI hit-testing.
- Validate risk/score calculations and lifecycle state transitions.
- Keep tests executable independent from the runtime game app.

## 5) CI/CD quality gates

- Build and run tests in both `Debug` and `Release`.
- Export machine-readable test reports (`JUnit XML`).
- Add line-coverage collection in GitHub Actions.
- Enforce a minimum coverage threshold to prevent regressions.
- Upload artifacts for traceability (binaries, reports, coverage XML).

## 6) Practical rollout sequence

1. Add testable gameplay/telemetry modules.
2. Add/expand unit tests.
3. Run local debug + release validation.
4. Add CI matrix and coverage workflow.
5. Raise threshold gradually as test depth increases.

## 7) GitHub Actions verification checklist

- `CI` workflow: all matrix jobs pass.
- `Coverage` workflow: threshold check passes.
- Artifacts available:
  - test reports
  - coverage report (`coverage.xml`)
  - executable outputs
