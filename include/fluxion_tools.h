#ifndef FLUXION_TOOLS_H
#define FLUXION_TOOLS_H

#include "fluxion_node.h"
#include "fluxion_runtime.h"
#include <stddef.h>
#include <stdint.h>

/* ============================================================================
 * FLUXION — TOOLS & VISUALIZATION
 * ============================================================================
 */

/**
 * @brief Health statistics of the Fluxion graph.
 * Provides a global view of the state and efficiency of the pipeline.
 */
typedef struct {
    size_t total_nodes;        // Total number of nodes in the graph
    size_t sleeping_nodes;     // Nodes dormant after a pulse
    size_t ready_nodes;        // Nodes ready to execute
    size_t running_nodes;      // Nodes currently executing
    size_t circular_blockages; // Number of detected cycles
    uint64_t total_transfers;  // Total number of data transfers
    double pulse_efficiency;   // Ratio of executed nodes / ready nodes (%)
} FluxionMetrics;

/* ============================================================================
 * TERMINAL CONFIGURATION
 * ============================================================================
 */

/**
 * @brief Configures the terminal to support ANSI colors and UTF-8
 * Required for Windows (10/11) to display borders and colors correctly.
 */
void fluxion_setup_terminal(void);

/* ============================================================================
 * EXPORT & VISUALIZATION
 * ============================================================================
 */

/**
 * @brief Generates a DOT file for Graphviz from the Fluxion graph
 * @param graph Array of pointers to nodes
 * @param count Total number of nodes
 * @param filename Name of the DOT file to generate
 * @note The DOT includes colors and labels to indicate node states
 */
void fluxion_export_dot(Node* graph[], size_t count, const char* filename);

/**
 * @brief Displays the current state of nodes in the console
 * Using ANSI colors:
 *  - Green  : READY
 *  - Yellow : RUNNING
 *  - Gray   : SLEEPING
 * @param graph Array of nodes
 * @param count Number of nodes
 * @param metrics Optional: metrics to display under the graph (NULL if unused)
 */
void fluxion_trace_nodes(Node* graph[], size_t count, FluxionMetrics* metrics);

/* ============================================================================
 * LOGGING & OBSERVABILITY
 * ============================================================================
 */

/**
 * @brief Enables runtime data logging to a CSV file
 * @param ctx Fluxion context
 * @param csv_path Path to the CSV file
 * @note Columns include pulse_id, timestamp, node_count, etc.
 */
void fluxion_enable_logging(FluxionContext* ctx, const char* csv_path);

/**
 * @brief Analyzes the graph and returns advanced metrics
 * @param ctx Fluxion context
 * @param graph Array of nodes
 * @param count Number of nodes
 * @return Complete FluxionMetrics structure
 */
FluxionMetrics fluxion_inspect(FluxionContext* ctx, Node* graph[], size_t count);

/* ============================================================================
 * UTILITIES
 * ============================================================================
 */

/**
 * @brief Returns an ANSI string representing the current state of a node
 * @param n Pointer to the node
 * @return ANSI color string (e.g., "\033[1;32m")
 */
const char* fluxion_node_state_color(const Node* n);

/**
 * @brief Displays a global summary of the Fluxion graph in the console
 * @param metrics Metrics structure calculated with fluxion_inspect
 */
void fluxion_print_summary(const FluxionMetrics* metrics);

#endif /* FLUXION_TOOLS_H */
