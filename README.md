# 🧲 Fluxion Framework

![Version](https://img.shields.io/badge/version-2.3.1-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Language](https://img.shields.io/badge/language-C%2FC++-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20Mac-blue)

---

## 📌 Description

Fluxion is a lightweight **dataflow programming** and **reactive pipeline** framework in C, designed to orchestrate processing nodes, monitor their real-time state, log data flows, and export graphs for visualization. It is ideal for simulation, multi-branch pipeline orchestration, and advanced flow monitoring.

---

## 🧩 Key Features

### 1. Node Management

* **Create nodes** with `NODE_INIT(node, LogicFunc, "type")`
* **Link nodes** via `CONNECT(node_from, node_to)`
* **Configurable nodes** with internal state (`state`) and custom business logic
* **Multi-branch support** and complex topologies

### 2. Node States

* `SLEEPING` : inactive node
* `READY` : node ready to execute
* `RUNNING` : node currently executing
* Real-time monitoring via `fluxion_trace_nodes()`
* ANSI colors for state visualization

### 3. Emitting and executing pulses

* `fluxion_emit(&ctx, &node, &data)` : injects data into the pipeline
* `fluxion_pulse(&ctx, graph, count)` : executes a pulse on all connected nodes

### 4. Logging and Observability

* `fluxion_enable_logging(ctx, "file.csv")` : CSV log of pulses and execution policy
* `FluxionMetrics fluxion_inspect(ctx, graph, count)` : inspects the graph state and calculates:

  * Total Nodes
  * Ready Nodes
  * Running Nodes
  * Sleeping Nodes
  * Circular Blockages
  * Total Transfers
  * Pulse Efficiency
* `fluxion_print_summary(&metrics)` : prints a clear console summary

### 5. Export & Visualization

* `fluxion_export_dot(graph, count, "filename.dot")` : exports the graph in **DOT** format for Graphviz
* Colors and labels indicate node states

### 6. Terminal Support

* UTF-8 and ANSI color support (Windows/Linux)
* Automatic clear screen for live inspector

### 7. Cleanup and Memory Management

* `fluxion_node_cleanup(&node)` frees node memory and internal state
* Automatic closing of log files

---

## 🔧 Example Usage

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
    size_t node_count = sizeof(graph)/sizeof(graph[0]);

    int val = 42;
    fluxion_emit(&ctx, &gen, &val);
    fluxion_pulse(&ctx, graph, node_count);
    fluxion_trace_nodes(graph, node_count);

    FluxionMetrics m = fluxion_inspect(&ctx, graph, node_count);
    fluxion_print_summary(&m);

    fluxion_export_dot(graph, node_count, "pipeline.dot");

    fluxion_node_cleanup(&gen);
    fluxion_node_cleanup(&mul);
    fluxion_node_cleanup(&agg);
}
```

---

## 📂 Project Structure

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

## ⚙️ Compilation

```bash
gcc -std=c99 -Wall -Wextra -Iinclude \
    src/fluxion_node.c src/fluxion_runtime.c src/fluxion_tools.c \
    examples/basic_pipeline.c -o fluxion_app.exe
```

Then execute:

```bash
./fluxion_app.exe
```

---

## 📝 License

MIT License
