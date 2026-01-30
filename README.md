#  Fluxion

**Deterministic Reactive Dataflow Engine (C)**

---
![Language](https://img.shields.io/badge/language-C-blue.svg)
![C Standard](https://img.shields.io/badge/C%20Standard-C99-orange.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)
![Deterministic](https://img.shields.io/badge/execution-Deterministic-critical.svg)
![Observability](https://img.shields.io/badge/observability-Built--in-informational.svg)
![Status](https://img.shields.io/badge/status-Active%20Development-success.svg)

##  Overview

**Fluxion** is a lightweight, deterministic **reactive dataflow execution engine** written in **C**, designed to orchestrate processing nodes within directed graphs, execute them in a controlled pulse-based manner, observe their runtime behavior in real time, and export full graph state for visualization and analysis.

Fluxion is built for developers who need **predictability, observability, and low-level control** — without sacrificing clarity or extensibility.

Typical use cases include:

* Deterministic simulations
* Modular AI / cognitive systems
* Embedded and IoT pipelines
* Multi-branch data processing graphs
* Runtime graph inspection and debugging

---

##  Core Philosophy

Fluxion is **not** an asynchronous black box.

It is built around three fundamental principles:

1. **Determinism first** – every execution step is explicit and reproducible
2. **Observability by design** – the system exposes its internal state at all times
3. **Graph-based execution** – logic is expressed as connected processing nodes

Execution progresses through **global pulses**, making system behavior transparent, debuggable, and replayable.

---

##  Key Features

###  Node Management

* Create processing nodes using a clean macro-based API:

  ```c
  NODE_INIT(node, LogicFunction, "type");
  ```
* Connect nodes using directed edges:

  ```c
  CONNECT(node_from, node_to);
  ```
* Nodes support:

  * Custom internal state
  * User-defined logic functions
  * Multiple inputs and outputs
* Arbitrary graph topologies:

  * Linear pipelines
  * Multi-branch graphs
  * Fan-in / fan-out configurations

---

###  Node Lifecycle & States

Each node transitions through explicit runtime states:

| State      | Description                        |
| ---------- | ---------------------------------- |
| `SLEEPING` | Node is inactive or awaiting input |
| `READY`    | Node is ready to execute           |
| `RUNNING`  | Node is currently executing        |

Live inspection is available via:

```c
fluxion_trace_nodes(graph, count);
```

* ANSI / UTF-8 color-coded output
* Cross-platform terminal support (Windows / Linux)

---

###  Pulse-Based Execution Model

Fluxion executes graphs using a **global pulse mechanism**:

* Data is injected using:

  ```c
  fluxion_emit(&ctx, &node, &data);
  ```
* Execution is triggered explicitly:

  ```c
  fluxion_pulse(&ctx, graph, node_count);
  ```

Each pulse:

* Evaluates node readiness
* Executes eligible nodes exactly once
* Propagates data deterministically

This model enables:

* Precise control over execution order
* Deterministic simulation steps
* Debug-friendly behavior

---

###  Logging & Observability

Fluxion includes **built-in observability tools**:

#### CSV Logging

```c
fluxion_enable_logging(&ctx, "execution_log.csv");
```

Logs include:

* Pulse number
* Node transitions
* Execution decisions
* Data transfers

#### Runtime Inspection

```c
FluxionMetrics metrics = fluxion_inspect(&ctx, graph, node_count);
fluxion_print_summary(&metrics);
```

Collected metrics:

* Total node count
* Ready / running / sleeping nodes
* Circular dependency detection
* Total data transfers
* Pulse efficiency

---

###  Graph Export & Visualization

Fluxion can export the full runtime graph:

```c
fluxion_export_dot(graph, node_count, "pipeline.dot");
```

* DOT format compatible with **Graphviz**
* Node colors reflect runtime state
* Edge labels show data flow direction

This makes Fluxion ideal for:

* Debugging complex graphs
* Teaching dataflow concepts
* Building graphical tooling on top

---

### Terminal Support

* ANSI and UTF-8 color output
* Automatic screen refresh for live inspectors
* Platform compatibility: Windows & Linux

---

###  Memory Management

Fluxion follows explicit and safe memory rules:

```c
fluxion_node_cleanup(&node);
```

* Releases node resources and internal state
* Automatically closes log files
* No hidden allocations

---

##  Minimal Example

```c
#include "../include/fluxion_runtime.h"
#include "../include/fluxion_node.h"
#include "../include/fluxion_tools.h"
#include <stdio.h>

int main(void) {
    fluxion_setup_terminal();
    FluxionContext ctx = fluxion_init();

    Node gen, mul, agg;
    NODE_INIT(gen, Generator, "int");
    NODE_INIT(mul, ConfigurableMul, "int");
    NODE_INIT(agg, Aggregator, "int");

    CONNECT(gen, mul);
    CONNECT(mul, agg);

    Node* graph[] = { &gen, &mul, &agg };
    size_t node_count = sizeof(graph) / sizeof(graph[0]);

    int value = 42;
    fluxion_emit(&ctx, &gen, &value);
    fluxion_pulse(&ctx, graph, node_count);

    fluxion_trace_nodes(graph, node_count);

    FluxionMetrics metrics = fluxion_inspect(&ctx, graph, node_count);
    fluxion_print_summary(&metrics);

    fluxion_export_dot(graph, node_count, "pipeline.dot");

    fluxion_node_cleanup(&gen);
    fluxion_node_cleanup(&mul);
    fluxion_node_cleanup(&agg);
}
```

---

##  Project Structure

```
fluxion/
├─ include/
│  ├─ fluxion_node.h
│  ├─ fluxion_runtime.h
│  └─ fluxion_tools.h
├─ src/
│  ├─ fluxion_node.c
│  ├─ fluxion_runtime.c
│  └─ fluxion_tools.c
├─ examples/
│  └─ basic_pipeline.c
└─ README.md
```

---

##  Compilation

```bash
gcc -std=c99 -Wall -Wextra -Iinclude \
    src/fluxion_node.c src/fluxion_runtime.c src/fluxion_tools.c \
    examples/basic_pipeline.c -o fluxion_app
```

Run:

```bash
./fluxion_app
```

---

##  Roadmap (Planned)

* Typed connections and validation
* Subgraphs and hierarchical execution
* Error and blocked node states
* Deterministic replay and snapshotting
* Language bindings (Python)
* Real-time visual inspector

---

##  License

MIT License

---

##  Final Note

Fluxion is designed as a **foundational execution engine**, not a toy abstraction.

If you need transparency, determinism, and full control over your dataflow logic — **Fluxion is your core**.
