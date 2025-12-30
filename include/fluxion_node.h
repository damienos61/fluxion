#ifndef FLUXION_NODE_H
#define FLUXION_NODE_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declaration of Context to avoid circular dependency */
typedef struct FluxionContext FluxionContext;

/* ============================================================================
 * FLUXION — NODE CORE
 * ============================================================================ */

typedef struct Node Node;

/**
 * @brief Signature of a Fluxion node logic
 * Updated to include the Context for data propagation.
 */
typedef void (*NodeAction)(FluxionContext* ctx, Node* self, void* data);

typedef enum {
    FLUXION_NODE_SLEEPING = 0,
    FLUXION_NODE_READY    = 1,
    FLUXION_NODE_RUNNING  = 2,
    FLUXION_NODE_ERROR    = 3
} FluxionNodeState;



/**
 * @brief Core structure of a Fluxion node
 */
struct Node {
    /* --- Identity --- */
    uint32_t uid;
    const char* name;
    const char* data_type;

    /* --- Behavior --- */
    NodeAction action;         
    void* state;               /* Persistent internal memory */
    size_t state_size;

    /* --- Data --- */
    void* input_buffer;        

    /* --- Execution --- */
    FluxionNodeState state_flag; 
    uint64_t last_pulse_id;    /* Prevention of infinite loops/reentrancy */

    /* --- Graph (Topology) --- */
    struct Node** subscribers; 
    size_t subscriber_count;
    size_t subscriber_capacity; /* Optimization: avoids frequent reallocs */
};

/* ============================================================================
 * NODE MANAGEMENT API
 * ============================================================================ */

/**
 * @brief Allocates and copies internal state for the node.
 */
void fluxion_node_set_state(Node* n, const void* state_data, size_t size);

/**
 * @brief Establishes a directed link from src to dst.
 */
void fluxion_node_link(Node* src, Node* dst);

void fluxion_node_unlink(Node* src, Node* dst);

void fluxion_node_cleanup(Node* n);

/* --- Inline Helpers --- */

static inline bool fluxion_node_is_ready(const Node* n) {
    return n && n->state_flag == FLUXION_NODE_READY;
}

static inline bool fluxion_node_has_subscribers(const Node* n) {
    return n && n->subscriber_count > 0;
}

/* ============================================================================
 * DSL MACROS
 * ============================================================================ */

/**
 * @brief Defines node logic with the correct naming convention.
 */
#define FLUX_NODE(name) void name##_logic(FluxionContext* ctx, Node* self, void* data)

#define FLUXION_UID(var) ((uint32_t)((uintptr_t)&(var) ^ __LINE__))

/**
 * @brief Initializes a Node on the stack or heap.
 */
#define NODE_INIT(node_var, logic_func, type_str) \
    node_var = (Node){ \
        .uid = FLUXION_UID(node_var), \
        .name = #node_var, \
        .data_type = type_str, \
        .action = logic_func##_logic, \
        .state = NULL, \
        .state_size = 0, \
        .input_buffer = NULL, \
        .state_flag = FLUXION_NODE_SLEEPING, \
        .last_pulse_id = 0, \
        .subscribers = NULL, \
        .subscriber_count = 0, \
        .subscriber_capacity = 0 \
    }

/**
 * @brief Simple DSL for connecting nodes.
 */
#define CONNECT(src, dst) fluxion_node_link(&src, &dst)

#endif /* FLUXION_NODE_H */
