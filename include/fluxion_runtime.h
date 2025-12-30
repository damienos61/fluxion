#ifndef FLUXION_RUNTIME_H
#define FLUXION_RUNTIME_H

#include <stdint.h>
#include <stddef.h>

#include "fluxion_node.h"

/* ============================================================================
 * FLUXION — RUNTIME CORE
 *
 * The runtime enforces the Fluxion paradigm:
 * - A data triggers a pulse
 * - A node executes only once per pulse
 * - The graph is stable and deterministic
 * ============================================================================
 */

/* --- ERROR CODES --- */

typedef enum {
    FLUXION_OK = 0,
    FLUXION_ERR_NULL_CONTEXT,
    FLUXION_ERR_INVALID_NODE,
    FLUXION_ERR_CYCLE_DETECTED,
    FLUXION_ERR_TYPE_MISMATCH
} FluxionError;

/* --- EXECUTION POLICY --- */

typedef enum {
    FLUXION_EXEC_IMMEDIATE = 0,  // Execute immediately upon emission
    FLUXION_EXEC_DEFERRED        // Execute during fluxion_pulse()
} FluxionExecPolicy;

/* --- RUNTIME CONTEXT --- */

/**
 * @brief Global Fluxion context
 *
 * A single context can manage multiple graphs,
 * but a pulse is always atomic.
 */
typedef struct {
    uint64_t current_pulse;       // Global pulse identifier
    uint64_t executed_nodes;      // Execution statistics
    FluxionError last_error;      // Last encountered error
    FluxionExecPolicy policy;     // Execution policy
} FluxionContext;

/* ============================================================================
 * RUNTIME API
 * ============================================================================
 */

/**
 * @brief Initializes a Fluxion context
 */
FluxionContext fluxion_init(void);

/**
 * @brief Sets the execution policy
 */
void fluxion_set_policy(FluxionContext* ctx, FluxionExecPolicy policy);

/**
 * @brief Links two nodes in the graph
 *
 * May reject the connection if:
 * - Types are incompatible
 * - An immediate cycle is detected
 */
FluxionError fluxion_link(Node* src, Node* dst);

/**
 * @brief Injects data into the graph
 *
 * Automatically increments the pulse_id.
 * Protects against multiple execution of the same node.
 */
FluxionError fluxion_emit(FluxionContext* ctx, Node* target, void* data);

/**
 * @brief Executes all nodes ready for the current pulse
 */
void fluxion_pulse(FluxionContext* ctx, Node* graph[], size_t count);

/**
 * @brief Resets the runtime state (without destroying the graph)
 */
void fluxion_reset(FluxionContext* ctx);

/* ============================================================================
 * DSL MACROS (ERGONOMICS)
 * ============================================================================
 */

/**
 * Fluent connection
 */
#define CONNECT(src, dst) fluxion_link(&(src), &(dst))

/**
 * Fluent emission (the context manages the pulse)
 */
#define EMIT(ctx, target, value) \
    fluxion_emit((ctx), &(target), (void*)(value))

#endif /* FLUXION_RUNTIME_H */
