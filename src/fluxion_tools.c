#include "../include/fluxion_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* --- PORTABLE TERMINAL --- */
#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

/* ============================================================================
 * TERMINAL CONFIGURATION
 * ============================================================================
 */
void fluxion_setup_terminal(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
    SetConsoleOutputCP(65001); // UTF-8
#endif
}

/* ============================================================================
 * DOT EXPORT
 * ============================================================================
 */
void fluxion_export_dot(Node* graph[], size_t count, const char* filename) {
    if (!graph || !filename) return;
    FILE* f = fopen(filename, "w");
    if (!f) return;

    fprintf(f,
        "digraph Fluxion {\n"
        "  rankdir=LR;\n"
        "  node [shape=record, style=filled, fontname=\"Verdana\"];\n"
    );

    for (size_t i = 0; i < count; i++) {
        Node* n = graph[i];
        if (!n) continue;

        const char* fill = "#ecf0f1"; // default: SLEEPING
        switch (n->state_flag) {
            case FLUXION_NODE_READY:   fill = "#2ecc71"; break;
            case FLUXION_NODE_RUNNING: fill = "#f1c40f"; break;
            case FLUXION_NODE_SLEEPING: fill = "#bdc3c7"; break;
        }

        fprintf(f, "  n%u [label=\"{%s|%s}\", fillcolor=\"%s\"];\n",
                n->uid, n->name,
                n->data_type ? n->data_type : "any", fill);

        for (size_t j = 0; j < n->subscriber_count; j++) {
            fprintf(f, "  n%u -> n%u;\n", n->uid, n->subscribers[j]->uid);
        }
    }

    fprintf(f, "}\n");
    fclose(f);
}

/* ============================================================================
 * TRACE NODES
 * ============================================================================
 */
void fluxion_trace_nodes(Node* graph[], size_t count, FluxionMetrics* metrics) {
    if (!graph) return;
    system(CLEAR_SCREEN);

#ifdef _WIN32
    const char *top = "+----------------------------------------------------+";
    const char *mid = "|               FLUXION LIVE INSPECTOR              |";
    const char *bot = "+----------------------------------------------------+";
#else
    const char *top = "┌────────────────────────────────────────────────────┐";
    const char *mid = "│               FLUXION LIVE INSPECTOR              │";
    const char *bot = "└────────────────────────────────────────────────────┘";
#endif

    printf("%s\n%s\n%s\n", top, mid, bot);

    size_t ready = 0, running = 0, sleeping = 0;

    for (size_t i = 0; i < count; i++) {
        Node* n = graph[i];
        if (!n) continue;

        const char* state_str = "UNKNOWN";
        const char* color = "\033[0m";

        switch (n->state_flag) {
            case FLUXION_NODE_SLEEPING: state_str = "SLEEPING"; color = "\033[1;30m"; sleeping++; break;
            case FLUXION_NODE_READY:    state_str = "READY";    color = "\033[1;32m"; ready++; break;
            case FLUXION_NODE_RUNNING:  state_str = "RUNNING";  color = "\033[1;33m"; running++; break;
        }

        printf(" %-15s [%s%s\033[0m] -> Type: %s\n",
               n->name, color, state_str,
               n->data_type ? n->data_type : "any");
    }

    if (metrics) {
        printf("\nTotal: %zu | Ready: %zu | Running: %zu | Sleeping: %zu | Efficiency: %.2f%%\n",
               metrics->total_nodes, ready, running, sleeping, metrics->pulse_efficiency);
    } else {
        double efficiency = (count > 0) ? ((double)running / count) * 100.0 : 0.0;
        printf("\nTotal: %zu | Ready: %zu | Running: %zu | Sleeping: %zu | Efficiency: %.2f%%\n",
               count, ready, running, sleeping, efficiency);
    }
}

/* ============================================================================
 * CSV LOGGING
 * ============================================================================
 */
void fluxion_enable_logging(FluxionContext* ctx, const char* csv_path) {
    if (!ctx || !csv_path) return;

    static int header_written = 0;
    FILE* f = fopen(csv_path, header_written ? "a" : "w");
    if (!f) return;

    if (!header_written) {
        fprintf(f, "pulse_id,timestamp,executed_nodes,policy\n");
        header_written = 1;
    }

    fprintf(f, "%llu,%lu,%llu,%s\n",
            (unsigned long long)ctx->current_pulse,
            (unsigned long)time(NULL),
            (unsigned long long)ctx->executed_nodes,
            (ctx->policy == FLUXION_EXEC_IMMEDIATE) ? "IMMEDIATE" : "DEFERRED");

    fclose(f);
}

/* ============================================================================
 * METRICS INSPECTION
 * ============================================================================
 */
FluxionMetrics fluxion_inspect(FluxionContext* ctx, Node* graph[], size_t count) {
    FluxionMetrics m = {0};
    if (!ctx || !graph) return m;

    m.total_nodes = count;
    for (size_t i = 0; i < count; i++) {
        Node* n = graph[i];
        if (!n) continue;
        switch (n->state_flag) {
            case FLUXION_NODE_READY:   m.ready_nodes++; break;
            case FLUXION_NODE_RUNNING: m.running_nodes++; break;
            case FLUXION_NODE_SLEEPING: m.sleeping_nodes++; break;
        }
    }

    m.total_transfers = ctx->executed_nodes;
    m.pulse_efficiency = (count > 0) ? ((double)m.running_nodes / count) * 100.0 : 0.0;

    return m;
}

/* ============================================================================
 * COLOR UTILITY
 * ============================================================================
 */
const char* fluxion_node_state_color(const Node* n) {
    if (!n) return "\033[0m";
    switch (n->state_flag) {
        case FLUXION_NODE_SLEEPING: return "\033[1;30m";
        case FLUXION_NODE_READY:    return "\033[1;32m";
        case FLUXION_NODE_RUNNING:  return "\033[1;33m";
        default: return "\033[0m";
    }
}

/* ============================================================================
 * SUMMARY DISPLAY
 * ============================================================================
 */
void fluxion_print_summary(const FluxionMetrics* metrics) {
    if (!metrics) return;
    printf("=== FLUXION SUMMARY ===\n");
    printf("Total Nodes       : %zu\n", metrics->total_nodes);
    printf("Sleeping Nodes    : %zu\n", metrics->sleeping_nodes);
    printf("Ready Nodes       : %zu\n", metrics->ready_nodes);
    printf("Running Nodes     : %zu\n", metrics->running_nodes);
    printf("Circular Blockages: %zu\n", metrics->circular_blockages);
    printf("Total Transfers   : %llu\n", (unsigned long long)metrics->total_transfers);
    printf("Pulse Efficiency  : %.2f%%\n", metrics->pulse_efficiency);
}
