#include "../include/fluxion_node.h"
#include <string.h>
#include <stdio.h>

/* ============================================================================
 * FLUXION — NODE IMPLEMENTATION
 * ============================================================================ */

/**
 * @brief Initialize or replace the internal state of a node
 */
void fluxion_node_set_state(Node* n, const void* state_data, size_t size) {
    if (!n || !state_data || size == 0) return;

    /* Proper cleanup before replacement */
    if (n->state) {
        free(n->state);
    }

    n->state = malloc(size);
    if (!n->state) {
        fprintf(stderr, "[Fluxion] Error: Could not allocate state for node '%s'\n", n->name);
        return;
    }

    memcpy(n->state, state_data, size);
    n->state_size = size;
}



/**
 * @brief Establishes a directed link from src to dst
 * Uses a growth strategy to minimize reallocations.
 */
void fluxion_node_link(Node* src, Node* dst) {
    if (!src || !dst) return;

    /* Check if we need to expand capacity */
    if (src->subscriber_count >= src->subscriber_capacity) {
        size_t new_cap = (src->subscriber_capacity == 0) ? 2 : src->subscriber_capacity * 2;
        Node** new_list = realloc(src->subscribers, sizeof(Node*) * new_cap);
        
        if (!new_list) {
            fprintf(stderr, "[Fluxion] Error: Failed to expand subscribers for '%s'\n", src->name);
            return;
        }

        src->subscribers = new_list;
        src->subscriber_capacity = new_cap;
    }

    /* Add the subscriber */
    src->subscribers[src->subscriber_count++] = dst;
}

/**
 * @brief Remove a link between two nodes
 */
void fluxion_node_unlink(Node* src, Node* dst) {
    if (!src || !dst || src->subscriber_count == 0) return;

    for (size_t i = 0; i < src->subscriber_count; i++) {
        if (src->subscribers[i] == dst) {
            /* Shift remaining subscribers to fill the gap */
            for (size_t j = i; j < src->subscriber_count - 1; j++) {
                src->subscribers[j] = src->subscribers[j + 1];
            }
            src->subscriber_count--;
            return;
        }
    }
}

/**
 * @brief Deep cleanup of a node
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
    }

    n->state_size = 0;
    n->subscriber_count = 0;
    n->subscriber_capacity = 0;
    n->input_buffer = NULL;
    n->state_flag = FLUXION_NODE_SLEEPING;
    n->last_pulse_id = 0;
}

/**
 * @brief Print detailed information about a Fluxion node
 */
void fluxion_node_debug(const Node* n) {
    if (!n) return;

    const char* exec_state = "UNKNOWN";
    switch (n->state_flag) {
        case FLUXION_NODE_SLEEPING: exec_state = "SLEEPING"; break;
        case FLUXION_NODE_READY:    exec_state = "READY";    break;
        case FLUXION_NODE_RUNNING:  exec_state = "RUNNING";  break;
        case FLUXION_NODE_ERROR:    exec_state = "ERROR!!";  break;
    }

    printf(
        "--- [Node: %s] ---\n"
        "  UID        : %u\n"
        "  Type       : %s\n"
        "  State      : %s\n"
        "  Links      : %zu / %zu (Count/Cap)\n"
        "  Memory     : %s (%zu bytes)\n\n",
        n->name ? n->name : "unnamed",
        n->uid,
        n->data_type ? n->data_type : "generic",
        exec_state,
        src->subscriber_count,
        src->subscriber_capacity,
        n->state ? "Allocated" : "None",
        n->state_size
    );
}
