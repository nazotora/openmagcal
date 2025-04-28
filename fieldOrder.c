#include <stdlib.h>

#include "fieldOrder.h"

fieldOrderQueue_t* fieldOrderQueue_init() {
    fieldOrderQueue_t* queue = malloc(sizeof(fieldOrderQueue_t));
    return queue;
}

fieldOrderNode_t* fieldOrderNode_create(float x, float y, float z, int t) {
    fieldOrderNode_t* order = malloc(sizeof(fieldOrderNode_t));
    order->x = x;
    order->y = y;
    order->z = z;
    order->t = t;
    return order;
}

void fieldOrderQueue_enqueue(fieldOrderQueue_t* queue, fieldOrderNode_t* node) {
    if (!queue->head) {
        // head is null, initialize it
        queue->head = node;
    }
    fieldOrderNode_t* current_tail = queue->tail;
    queue->tail = node;
    if (current_tail != NULL) {
        current_tail->next = node;
    }
}

/** WARNING: free this on your own dime, its removed from the data structure here */
fieldOrderNode_t* fieldOrderQueue_dequeue(fieldOrderQueue_t* queue) {
    fieldOrderNode_t* current_head = queue->head;
    if (current_head) {
        fieldOrderNode_t* next = current_head->next;
        if (!next) {
            queue->tail = NULL;
        }
        queue->head = next;
    }
    return current_head;
}

int fieldOrderQueue_isEmpty(fieldOrderQueue_t* queue) {
    if (queue->head || queue->tail) {
        return 0;
    }
    return 1;
}

// this will free from the beginning.
void fieldOrderQueue_free(fieldOrderQueue_t* queue) {
    fieldOrderNode_t* next = queue->head;
    while (next) {
        fieldOrderNode_t* this = next;
        next = this->next;
        free(this);
    }
    free(queue);
    return;
}
