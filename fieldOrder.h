// The fieldOrder struct contains ints for x, y, z in nanoTeslas,
// as well as the int t in seconds. 
typedef struct fieldOrderNode {
    int x;
    int y;
    int z;
    int t;
    struct fieldOrderNode* next;
} fieldOrderNode_t;

typedef struct fieldOrderQueue {
    struct fieldOrderNode* head;
    struct fieldOrderNode* tail;
} fieldOrderQueue_t;

fieldOrderQueue_t* fieldOrderQueue_init();

fieldOrderNode_t* fieldOrderNode_create(int x, int y, int z, int t);
void fieldOrderQueue_enqueue(fieldOrderQueue_t* queue, fieldOrderNode_t* node);
fieldOrderNode_t* fieldOrderQueue_dequeue(fieldOrderQueue_t* queue);

void fieldOrderQueue_free(fieldOrderQueue_t* queue);
