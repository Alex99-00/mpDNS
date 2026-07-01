
#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct _Node Node;
struct _Node {
        char               * buf;
        size_t               bufLen;
        struct sockaddr_in * addr;
        Node               * next;
};

typedef struct {
        Node * first;
        Node * last;
}Queue;

#define NODE_EMPTY ((NULL))
#define NODE_IS_EMPTY(node) ((node == NODE_EMPTY))



void queueInit( Queue * queue );
void queueFree( Queue * queue );
void queueAppend( Queue * queue, Node * node );
Node * queueRemoveFirstElement( Queue * queue );

unsigned int queueCountElements( Queue * queue );

void nodeInit( Node * node );
Node * nodeNew();
void nodeFree( Node * node );


#endif

/*EOF*/
