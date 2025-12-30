#include "../include/fluxion_runtime.h"
#include "../include/fluxion_node.h"
#include "../include/fluxion_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ============================================================================
 * 1. COMPLEX STATES
 * ============================================================================
 */
typedef struct {
    int factor;
    int activations;
} MultiplierConfig;

typedef struct {
    int sum;
    int count;
} StatsState;

typedef struct {
    FILE* file;
} LoggerState;

/* ============================================================================
 * 2. BUSINESS LOGIC (NODES)
 * ============================================================================
 */

// Generator
FLUX_NODE(Generator) {
    (void)self; // prevent unused parameter warning
    int val = *(int*)data;
    printf("\033[1;34m[GEN]\033[0m Injected signal: %d\n", val);
}

// Configurable multiplier
FLUX_NODE(ConfigurableMul) {
    if (!self->state) {
        MultiplierConfig conf = { .factor = 3, .activations = 0 };
        fluxion_node_set_state(self, &conf, sizeof(MultiplierConfig));
    }
    MultiplierConfig* c = (MultiplierConfig*)self->state;
    c->activations++;

    int incoming = *(int*)data;
    *(int*)data = incoming * c->factor;
    printf("\033[1;33m[MUL]\033[0m x%d (Activations: %d) -> %d\n",
           c->factor, c->activations, *(int*)data);
}

// Sliding average aggregator
FLUX_NODE(Aggregator) {
    if (!self->state) {
        StatsState st = {0,0};
        fluxion_node_set_state(self, &st, sizeof(StatsState));
    }
    StatsState* s = (StatsState*)self->state;
    s->sum += *(int*)data;
    s->count++;
    printf("\033[1;35m[AGG]\033[0m Current average: %.2f\n",
           (double)s->sum / s->count);
}

// File logger
FLUX_NODE(FileLogger) {
    if (!self->state) {
        LoggerState ls = { NULL };
        ls.file = fopen("fluxion_audit.log", "w");
        if(ls.file) fprintf(ls.file, "--- LOG SESSION START ---\n");
        fluxion_node_set_state(self, &ls, sizeof(LoggerState));
    }
    LoggerState* ls = (LoggerState*)self->state;
    if (ls && ls->file) {
        fprintf(ls->file, "[PULSE] Data: %d\n", *(int*)data);
        fflush(ls->file);
        printf("\033[1;36m[FILE]\033[0m Disk write OK\n");
    }
}

// Critical threshold monitor
FLUX_NODE(SafetyThreshold) {
    (void)self; // prevent unused parameter warning
    int val = *(int*)data;
    if (val > 100) {
        printf("\033[1;31m[CRIT]\033[0m Critical threshold exceeded: %d\n", val);
    }
}

/* ============================================================================
 * 3. ORCHESTRATION
 * ============================================================================
 */
int main(void) {
    fluxion_setup_terminal();
    FluxionContext ctx = fluxion_init();

    // Node creation
    Node gen, mul, agg, log, alert;
    NODE_INIT(gen, Generator, "int");
    NODE_INIT(mul, ConfigurableMul, "int");
    NODE_INIT(agg, Aggregator, "int");
    NODE_INIT(log, FileLogger, "int");
    NODE_INIT(alert, SafetyThreshold, "int");

    // Multi-branch topology
    CONNECT(gen, mul);
    CONNECT(mul, agg);
    CONNECT(mul, log);
    CONNECT(mul, alert);

    Node* app_graph[] = { &gen, &mul, &agg, &log, &alert };
    size_t node_count = sizeof(app_graph)/sizeof(app_graph[0]);

    printf("╔══════════════════════════════════════════════╗\n");
    printf("║   FLUXION SYSTEM : MULTI-BRANCH PIPELINE    ║\n");
    printf("╚══════════════════════════════════════════════╝\n");

    for (int i = 1; i <= 5; i++) {
        int val = i * 12;
        printf("\n--- Pulse %llu (Input: %d) ---\n",
               (unsigned long long)ctx.current_pulse, val);

        // Inject data and execute
        fluxion_emit(&ctx, &gen, &val);
        fluxion_pulse(&ctx, app_graph, node_count);

        // Retrieve metrics and display
        FluxionMetrics m = fluxion_inspect(&ctx, app_graph, node_count);
        fluxion_trace_nodes(app_graph, node_count, &m);

        // Display quick summary
        fluxion_print_summary(&m);
    }

    // Export DOT
    fluxion_export_dot(app_graph, node_count, "complex_pipeline.dot");

    // Cleanup log file
    if (log.state) {
        LoggerState* ls = (LoggerState*)log.state;
        if (ls && ls->file) {
            fprintf(ls->file, "--- LOG SESSION END ---\n");
            fclose(ls->file);
        }
    }

    // Cleanup nodes
    fluxion_node_cleanup(&gen);
    fluxion_node_cleanup(&mul);
    fluxion_node_cleanup(&agg);
    fluxion_node_cleanup(&log);
    fluxion_node_cleanup(&alert);

    printf("\n=== FLUXION CLEAN EXIT ===\n");
    printf("Press Enter to exit...");
    getchar();

    return 0;
}
