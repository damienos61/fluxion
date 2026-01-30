#  Contributing to Fluxion

First of all, thank you for considering contributing to **Fluxion**.

Fluxion is a **low-level deterministic reactive dataflow engine written in C**. Contributions are expected to improve **correctness, clarity, predictability, and observability** — not to add uncontrolled complexity.

This document defines the rules, expectations, and best practices for contributing to the project.

---

##  Table of Contents

* Project Philosophy
* Ways to Contribute
* Code Style Guidelines
* Architecture Rules
* Commit Guidelines
* Pull Request Process
* Reporting Bugs
* Feature Requests
* Testing & Validation
* License

---

##  Project Philosophy

Fluxion follows a **strict and intentional design philosophy**:

* **Deterministic execution**
* **No hidden concurrency**
* **Explicit data propagation**
* **Minimal runtime overhead**
* **Clear separation of concerns** between:

  * Node logic
  * Runtime orchestration
  * Tooling & observability

Any contribution **must respect these principles**.

> If a change introduces ambiguity, hidden state, implicit behavior, or non-determinism, it will most likely be rejected.

---

##  Ways to Contribute

###  Code Contributions

* Bug fixes
* Performance improvements
* Runtime safety enhancements
* New tooling (inspection, visualization, logging)
* Memory correctness improvements

###  Documentation

* Improve README clarity
* Add usage examples
* Explain advanced graph patterns (cycles, branches, policies)
* Code comments (**English only**)

###  Tests & Examples

* New pipeline examples
* Stress testing complex graphs
* Validation of edge cases

###  Design Discussions

* Propose new concepts via GitHub Issues
* Review architectural decisions
* Discuss breaking changes before implementation

---

##  Code Style Guidelines

Fluxion uses **strict C99** with disciplined conventions.

### General Rules

* Language: **C99 only**
* No C++ features
* No macros with hidden side effects
* No mutable global state (except tightly controlled internal statics)

### Formatting

* 4 spaces indentation (no tabs)
* Braces on the same line
* Section separators:

  ```c
  /* ============================================================================ */
  ```
* Comments: **English only**

### Naming Conventions

| Element   | Convention                  |
| --------- | --------------------------- |
| Functions | `fluxion_*`                 |
| Structs   | `FluxionSomething`          |
| Enums     | `FLUXION_*`                 |
| Macros    | `FLUXION_*`                 |
| Files     | `fluxion_*.c / fluxion_*.h` |

---

##  Architecture Rules

Fluxion is **modular by design**.

### Core Modules (Do NOT mix responsibilities)

| Module              | Responsibility                        |
| ------------------- | ------------------------------------- |
| `fluxion_node.*`    | Node structure, state, lifecycle      |
| `fluxion_runtime.*` | Execution engine, pulses, propagation |
| `fluxion_tools.*`   | Inspection, logging, visualization    |

###  Forbidden Patterns

* Adding visualization code to the runtime
* Adding execution logic to tooling modules
* Introducing side effects in node headers
* Cross-layer dependencies

---

##  Commit Guidelines

Commits must be **clear, atomic, and intentional**.

### Commit Message Format

```
type(scope): short description
```

### Examples

* `fix(runtime): prevent double execution during same pulse`
* `feat(tools): add CSV execution logger`
* `docs(readme): clarify execution policy`
* `refactor(node): simplify state cleanup logic`

### Accepted Types

* `feat`
* `fix`
* `refactor`
* `docs`
* `test`
* `perf`
* `chore`

---

##  Pull Request Process

1. Fork the repository
2. Create a dedicated branch:

   ```bash
   git checkout -b feature/my-improvement
   ```
3. Make focused changes

Ensure that:

* The code builds without warnings
* No regression in examples
* Comments are written in English

### Pull Request Must Include

* Clear description of the change
* Motivation and rationale
* Potential trade-offs
* Updated documentation if applicable

### Pull Request Checklist

* [ ] Aligns with Fluxion philosophy
* [ ] No unnecessary complexity
* [ ] No breaking changes (unless previously discussed)
* [ ] Documentation updated
* [ ] Example provided when relevant

---

## 🐞 Reporting Bugs

Open a GitHub issue including:

* Fluxion version
* Platform (OS, compiler)
* Minimal reproducible example
* Expected vs actual behavior

> Bug reports without reproducible code may be ignored.

---

##  Feature Requests

Feature proposals must include:

* A clear use case
* Alignment with Fluxion philosophy
* Justification for core inclusion (vs external tooling)

Significant features **must be discussed before implementation**.

---

##  Testing & Validation

Before submitting a PR, run:

```bash
gcc -std=c99 -Wall -Wextra -Iinclude \
    src/fluxion_node.c src/fluxion_runtime.c src/fluxion_tools.c \
    examples/basic_pipeline.c -o fluxion_app
```

Verify:

* No crashes
* Deterministic output
* Correct node states
* No memory leaks (Valgrind recommended)

---

##  License

By contributing to Fluxion, you agree that your contributions are licensed under the **MIT License**, the same license as the project.

---

## Final Note

Fluxion is a **serious low-level execution engine**.

Quality, rigor, and clarity matter more than feature count.

When in doubt, **open an issue before coding**.

Thank you for helping improve Fluxion.
