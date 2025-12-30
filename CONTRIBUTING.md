# Contributing to Fluxion

First of all, thank you for considering contributing to Fluxion 
Fluxion is a low-level deterministic dataflow / reactive pipeline framework in C, and every contribution helps improve its stability, clarity, and power.

This document defines the rules, expectations, and best practices for contributing.

---

## 📌 Table of Contents

* Project Philosophy
* Ways to Contribute
* Code Style Rules
* Project Architecture Rules
* Commit Guidelines
* Pull Request Process
* Reporting Bugs
* Feature Requests
* Testing and Validation
* License

---

##  Project Philosophy

Fluxion follows a strict and intentional design:

* Deterministic execution
* No hidden concurrency
* Explicit data propagation
* Minimal runtime overhead
* Clear separation between:

  * Node logic
  * Runtime orchestration
  * Tooling and observability

 Any contribution must respect these principles.
If a feature introduces ambiguity, hidden state, or implicit behavior, it will most likely be rejected.

---

##  Ways to Contribute

You can contribute in several ways:

### 1. Code Contributions

* Bug fixes
* Performance improvements
* New tools (visualization, inspection, logging)
* Runtime safety improvements
* Memory correctness fixes

### 2. Documentation

* Improve README clarity
* Add usage examples
* Explain advanced patterns (multi-branch, cycles, policies)
* Comment code (English only)

### 3. Testing

* Add new pipeline examples
* Stress-test complex graphs
* Validate edge cases

### 4. Design Discussions

* Propose new concepts via GitHub issues
* Review architectural decisions

---

##  Code Style Rules

Fluxion uses plain C (C99) with strict discipline.

### General Rules

* Language: C99
* No C++ features
* No macros with hidden side effects
* No global mutable state (except controlled static internals)

### Formatting

* 4 spaces indentation (no tabs)
* Braces on the same line
* Clear section separators using:
  `/* ============================================================================ */`
* English comments only

### Naming Conventions

| Element      | Convention               |
| ------------ | ------------------------ |
| Functions    | `fluxion_*`              |
| Structures   | `FluxionSomething`       |
| Enumerations | `FLUXION_*`              |
| Macros       | `FLUXION_*` / DSL limits |
| Files        | `fluxion_*.c / .h`       |

---

##  Project Architecture Rules

Fluxion is modular by design.

### Core Files (DO NOT MIX RESPONSIBILITIES)

| File                | Responsibility                       |
| ------------------- | ------------------------------------ |
| `fluxion_node.*`    | Node structure, state, and lifecycle |
| `fluxion_runtime.*` | Execution, pulses, propagation       |
| `fluxion_tools.*`   | Visualization, logging, inspection   |

 DO NOT:

* Add visualization code to the runtime
* Add execution logic to tools
* Add side effects inside node headers

---

##  Commit Guidelines

Use clear and atomic commits.

### Commit Message Format

```
type(scope): short description
```

### Examples

```
fix(runtime): prevent double execution on same pulse
feat(tools): add CSV runtime logger
docs(readme): clarify execution policy section
refactor(node): simplify state cleanup logic
```

### Accepted Types

* feat
* fix
* refactor
* docs
* test
* perf
* chore

---

## Pull Request Process

1. Fork the repository
2. Create a dedicated branch:

   ```
   git checkout -b feature/my-improvement
   ```
3. Make focused changes
4. Ensure:

   * Code compiles without warnings
   * No regression in examples
   * Comments are in English
5. Submit a pull request with:

   * Clear description
   * Motivation
   * Potential trade-offs

### Pull Request Checklist

* Code follows Fluxion philosophy
* No unnecessary complexity
* No breaking changes (unless discussed beforehand)
* Documentation updated if needed
* Example provided when applicable

---

## 🐞 Reporting Bugs

Please open a GitHub issue with:

* Fluxion version
* Platform (OS, compiler)
* Minimal reproducible example
* Expected behavior vs actual behavior

 Bug reports without code examples may be ignored.

---

##  Feature Requests

Feature suggestions are welcome but must include:

* A clear use case
* Why it fits Fluxion philosophy
* Why it should be part of the core (and not external)

Large features must be discussed before implementation.

---

##  Testing and Validation

Before submitting:

```bash
gcc -std=c99 -Wall -Wextra -Iinclude \
    src/fluxion_node.c src/fluxion_runtime.c src/fluxion_tools.c \
    examples/basic_pipeline.c -o fluxion_app
```

Then run and verify:

* No crashes
* Deterministic output
* Correct node states
* No memory leaks (recommended: Valgrind)

---

## 📜 License

By contributing to Fluxion, you agree that your contributions will be licensed under the MIT License, the same license as the project.

---

##  Final Note

Fluxion is a serious low-level framework.
Quality, clarity, and discipline matter more than feature count.

If in doubt, open an issue before coding.

Thank you for helping improve Fluxion 
