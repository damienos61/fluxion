#include "../include/fluxion_runtime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ============================================================================
 * RUNTIME INITIALIZATION
 * ============================================================================
 */

FluxionContext fluxion_init(void) {
    FluxionContext ctx;
    ctx.current_pulse   = 1;
    ctx.executed_nodes  = 0;
    ctx.last_error      = FLUXION_OK;
    ctx.policy          = FLUXION_EXEC_DEFERRED;
    return ctx;
}

void fluxion_set_policy(FluxionContext* ctx, FluxionExecPolicy policy) {
    if (!ctx) return;
    ctx->policy = policy;
}

/* ============================================================================
 * GRAPH CONSTRUCTION
 * ============================================================================
 */

FluxionError fluxion_link(Node* src, Node* dst) {
    if (!src || !dst) return FLUXION_ERR_INVALID_NODE;

    /* Strict type validation */
    if (src->data_type && dst->data_type &&
        strcmp(src->data_type, dst->data_type) != 0) {

        fprintf(stderr,
            "[Fluxion] Warning: type mismatch %s(%s) -> %s(%s)\n",
            src->name, src->data_type,
            dst->name, dst->data_type
        );
    }

    /* Incremental allocation */
    Node** tmp = realloc(src->subscribers, sizeof(Node*) * (src->subscriber_count + 1));
    if (!tmp) return FLUXION_ERR_INVALID_NODE;

    src->subscribers = tmp;
    src->subscribers[src->subscriber_count++] = dst;

    return FLUXION_OK;
}

/* ============================================================================
 * INTERNAL PROPAGATION (RECURSIVE & SAFE)
 * ============================================================================
 */

static void fluxion_propagate(
    FluxionContext* ctx,
    Node* n,
    void* data,
    int depth
) {
    if (!ctx || !n) return;

    /* Loop protection and excessive depth check */
    if (depth > 1024) {
        ctx->last_error = FLUXION_ERR_CYCLE_DETECTED;
        return;
    }

    /* Already processed this pulse? */
    if (n->last_pulse_id == ctx->current_pulse) return;

    /* Update data */
    n->input_buffer  = data;
    n->state_flag    = FLUXION_NODE_READY;
    n->last_pulse_id = ctx->current_pulse;

    /* Downstream propagation to subscribers */
    for (size_t i = 0; i < n->subscriber_count; i++) {
        fluxion_propagate(ctx, n->subscribers[i], data, depth + 1);
    }
}

/* ============================================================================
 * PUBLIC API — EMIT
 * ============================================================================
 */

FluxionError fluxion_emit(FluxionContext* ctx, Node* target, void* data) {
    if (!ctx) return FLUXION_ERR_NULL_CONTEXT;
    if (!target) return FLUXION_ERR_INVALID_NODE;

    ctx->last_error = FLUXION_OK;

    fluxion_propagate(ctx, target, data, 0);

    /* Immediate policy: execute on emit */
    if (ctx->policy == FLUXION_EXEC_IMMEDIATE) {
        Node* graph[] = { target };
        fluxion_pulse(ctx, graph, 1);
    }

    return ctx->last_error;
}

/* ============================================================================
 * PULSE EXECUTION
 * ============================================================================
 */

void fluxion_pulse(FluxionContext* ctx, Node* graph[], size_t count) {
    if (!ctx || !graph) return;

    size_t executed = 0;

    for (size_t i = 0; i < count; i++) {
        Node* n = graph[i];
        if (!n || n->state_flag != FLUXION_NODE_READY) continue;

        /* Safe execution */
        n->state_flag = FLUXION_NODE_RUNNING;

        if (n->action) {
            n->action(n, n->input_buffer);
            executed++;
        }

        n->state_flag = FLUXION_NODE_SLEEPING;
    }

    ctx->executed_nodes += executed;

    if (executed == 0) {
        ctx->last_error = FLUXION_ERR_CYCLE_DETECTED;
    }

    ctx->current_pulse++;
}

/* ============================================================================
 * RUNTIME RESET
 * ============================================================================
 */

void fluxion_reset(FluxionContext* ctx) {
    if (!ctx) return;

    ctx->current_pulse  = 1;
    ctx->executed_nodes = 0;
    ctx->last_error     = FLUXION_OK;
}

/* ============================================================================
 * DEBUG & INSPECTION
 * ============================================================================
 */

void fluxion_runtime_debug(const FluxionContext* ctx) {
    if (!ctx) return;

    const char* policy = (ctx->policy == FLUXION_EXEC_IMMEDIATE) ? "IMMEDIATE" : "DEFERRED";

    printf("[Fluxion::Runtime]\n"
           "  Current Pulse  : %llu\n"
           "  Executed Nodes : %llu\n"
           "  Last Error     : %d\n"
           "  Policy         : %s\n\n",
           ctx->current_pulse,
           ctx->executed_nodes,
           ctx->last_error,
           policy
    );
}
