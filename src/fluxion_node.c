#include "../include/fluxion_node.h"

#include <string.h>
#include <stdio.h>

/* ============================================================================
 * FLUXION — NODE IMPLEMENTATION
 * ============================================================================
 */

/**
 * @brief Initialize or replace the internal state of a node
 *
 * The state is persistent memory unique to the node.
 * It allows the node to have "memory" across pulses.
 */
void fluxion_node_set_state(Node* n, void* state_data, size_t size) {
    if (!n || !state_data || size == 0) return;

    /* Clean replacement */
    if (n->state) {
        free(n->state);
        n->state = NULL;
        n->state_size = 0;
    }

    n->state = malloc(size);
    if (!n->state) {
        fprintf(stderr, "[Fluxion] Failed to allocate state for node '%s'\n", n->name);
        return;
    }

    memcpy(n->state, state_data, size);
    n->state_size = size;
}

/**
 * @brief Remove a link between two nodes
 *
 * Enables dynamic graph reconfiguration
 * without breaking memory integrity.
 */
void fluxion_node_unlink(Node* src, Node* dst) {
    if (!src || !dst || src->subscriber_count == 0) return;

    for (size_t i = 0; i < src->subscriber_count; i++) {
        if (src->subscribers[i] == dst) {

            /* Shift remaining subscribers */
            for (size_t j = i; j < src->subscriber_count - 1; j++) {
                src->subscribers[j] = src->subscribers[j + 1];
            }

            src->subscriber_count--;

            if (src->subscriber_count == 0) {
                free(src->subscribers);
                src->subscribers = NULL;
            } else {
                Node** tmp = realloc(
                    src->subscribers,
                    sizeof(Node*) * src->subscriber_count
                );
                if (tmp) src->subscribers = tmp;
            }

            return;
        }
    }
}

/**
 * @brief Deep cleanup of a node
 *
 * Does NOT destroy the node itself,
 * but resets its internal state.
 */
void fluxion_node_cleanup(Node* n) {
    if (!n) return;

    if (n->subscribers) {
        free(n->subscribers);
        n->subscribers = NULL;
    }

    if (n->state) {
        free(n->state);
        n->state = NULL;
        n->state_size = 0;
    }

    n->input_buffer = NULL;
    n->subscriber_count = 0;
    n->state_flag = FLUXION_NODE_SLEEPING;
    n->last_pulse_id = 0;
}

/**
 * @brief Print detailed information about a Fluxion node
 *
 * Key tool for debugging, tracing, and visualization.
 */
void fluxion_node_debug(const Node* n) {
    if (!n) return;

    const char* exec_state = "UNKNOWN";
    switch (n->state_flag) {
        case FLUXION_NODE_SLEEPING: exec_state = "SLEEPING"; break;
        case FLUXION_NODE_READY:    exec_state = "READY";    break;
        case FLUXION_NODE_RUNNING:  exec_state = "RUNNING";  break;
    }

    printf(
        "[Fluxion::Node]\n"
        "  Name        : %s\n"
        "  UID         : %u\n"
        "  Data Type   : %s\n"
        "  Exec State  : %s\n"
        "  Subscribers : %zu\n"
        "  Has State   : %s (%zu bytes)\n\n",
        n->name ? n->name : "<unnamed>",
        n->uid,
        n->data_type ? n->data_type : "generic",
        exec_state,
        n->subscriber_count,
        n->state ? "yes" : "no",
        n->state_size
    );
}
