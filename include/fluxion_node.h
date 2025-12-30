#ifndef FLUXION_NODE_H
#define FLUXION_NODE_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 * FLUXION — NODE CORE
 * A node is a dormant reactive entity.
 * It only executes when data is present.
 * ============================================================================
 */

/* --- BASIC TYPES --- */

typedef struct Node Node;

/**
 * @brief Signature of a Fluxion node logic
 * @param self The node itself (access to state and metadata)
 * @param data The data received via the flow
 */
typedef void (*NodeAction)(Node* self, void* data);

/**
 * @brief Possible node states (for debugging & tools)
 */
typedef enum {
    FLUXION_NODE_SLEEPING = 0,
    FLUXION_NODE_READY    = 1,
    FLUXION_NODE_RUNNING  = 2
} FluxionNodeState;

/**
 * @brief Core structure of a Fluxion node
 */
struct Node {
    /* --- Identity --- */
    uint32_t uid;              // Stable unique identifier
    const char* name;          // Symbolic name
    const char* data_type;     // Expected data type (debug)

    /* --- Behavior --- */
    NodeAction action;         // Business logic
    void* state;               // Persistent node memory
    size_t state_size;         // Size of the state (optional)

    /* --- Data --- */
    void* input_buffer;        // Current received data

    /* --- Execution --- */
    FluxionNodeState state_flag; // Current node state
    uint64_t last_pulse_id;      // Reentrancy protection

    /* --- Graph --- */
    struct Node** subscribers; // Dependent nodes
    size_t subscriber_count;
};

/* ============================================================================
 * NODE MANAGEMENT API
 * ============================================================================
 */

/**
 * @brief Initializes the internal memory state of the node
 */
void fluxion_node_set_state(Node* n, void* state_data, size_t size);

/**
 * @brief Removes a link between two nodes
 */
void fluxion_node_unlink(Node* src, Node* dst);

/**
 * @brief Frees all resources of the node
 */
void fluxion_node_cleanup(Node* n);

/**
 * @brief Prints node debug information
 */
void fluxion_node_debug(const Node* n);

/**
 * @brief Checks if the node is ready to execute
 */
static inline int fluxion_node_is_ready(const Node* n) {
    return n && n->state_flag == FLUXION_NODE_READY;
}

/* ============================================================================
 * DSL MACROS (FLUXION LANGUAGE)
 * ============================================================================
 */

/**
 * @brief Defines a Fluxion logic
 * Usage:
 *   FLUX_NODE(my_logic) {
 *       // code
 *   }
 */
#define FLUX_NODE(name) void name##_logic(Node* self, void* data)

/**
 * @brief Generates a pseudo-unique UID at compile time
 */
#define FLUXION_UID(var) ((uint32_t)((uintptr_t)&(var) ^ __LINE__))

/**
 * @brief Initializes a Fluxion node
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
        .subscriber_count = 0 \
    }

#endif /* FLUXION_NODE_H */
