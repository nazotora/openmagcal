#include <stdio.h>
#include <stdlib.h>
#include "fieldOrder.h"

int main(int argc, char* argv) {
    fieldOrderQueue_t* q = fieldOrderQueue_init();
    printf("adding some fieldorders\n");

    fieldOrderQueue_enqueue(q, fieldOrderNode_create(1, 2, 3, 4));
    fieldOrderQueue_enqueue(q, fieldOrderNode_create(5, 6, 7, 8));
    fieldOrderQueue_enqueue(q, fieldOrderNode_create(9, 10, 11, 12));

    printf("dequeing one\n");
    
    for(int i = 0; i < 10; i++) {
        fieldOrderNode_t* p = fieldOrderQueue_dequeue(q);
        if (p) {
            printf("%f %f %f %d\n", p->x, p->y, p->z, p->t);
            free(p);
        } else {
            printf("NULL\n");
        }
    }

    printf("freeing the q\n");
    fieldOrderQueue_free(q);

    printf(":D\n");
}
