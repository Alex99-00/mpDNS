/*

  Programm/Lib : mpDNSlib
  Datei        : queue.c
  Version      : v0.5r
  
  Beschreibung:
  Queue Funktionalität.
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <alloc.h>
#include <queue.h>

/*
  Queue Struktur initialisieren.
  queue  = Zu init. Queue Struktur.
  return = void.
*/
void queueInit( Queue * queue ) {
    assert( queue != NULL );
    
    queue->first = NODE_EMPTY;
    queue->last  = NODE_EMPTY;
}

/*
  Queue Struktur freigeben.
  queue  = Freizugebende Queue Struktur.
  return = void.
*/
void queueFree( Queue * queue ) {
    assert( queue != NULL );
    
    {    
     Node *tempNode = queue->first;
     while ( ! NODE_IS_EMPTY(tempNode) ) {     
             queue->first = tempNode->next;
             nodeFree( tempNode );
             tempNode = queue->first;             
     }     
    }
    
    queueInit( queue );
}

/*
  Anhängen einer Node an die Queue.
  queue  = Queue Struktur an die die Node angeh. wird.
  node   = Anzuhängende Node.
  return = void.
*/
void queueAppend( Queue * queue, Node * node ) {
    assert( (queue != NULL) && (node != NULL) );
    
    node->next = NODE_EMPTY;
    
    if ( NODE_IS_EMPTY(queue->first) ) {
         queue->first = node;
         queue->last  = node;
         return;
    }
    
    queue->last->next = node;
    queue->last       = node;
}

/*
  Trägt erstes Element aus Queue aus und gibt es zurück.
  queue  = Queue Struktur aus der das erste Element ausgetragen werden soll.
  return = Node.
*/
Node * queueRemoveFirstElement( Queue * queue ) {
    Node * tempNode;
    assert( queue != NULL );
    
    tempNode = queue->first;
    if ( ! NODE_IS_EMPTY(tempNode) ) {
         queue->first = tempNode->next;
    } else {
         queueInit( queue );
    }
    
    return tempNode;
}

/*
  Zählen der in der Queue enthaltenen Nodes.
  queue  = Queue Struktur die durchgezählt werden soll.
  return = Anzahl Elemente in Queue.
*/
unsigned int queueCountElements( Queue * queue ) {
    unsigned int c=0;
    Node * node;
    
    assert( queue != NULL );
    
    node = queue->first;
    while ( ! NODE_IS_EMPTY(node) ) {
            c++;
            node = node->next;
    }
    
    return c;
}

/*
  Eine Node Initialisieren.
  node   = Zu initialis. Node.
  return = void.
*/
void nodeInit( Node * node ) {
    assert( node != NULL );
    node->next = NODE_EMPTY;
}

/*
  Gibt eine neue und initialis. Node zurück.
  return = Neue Node.
*/
Node * nodeNew() {
    Node *tempNode = MALLOC( sizeof(Node) );
    if ( tempNode == NULL )
         return NULL;
    
    nodeInit( tempNode );
    
    return tempNode;
}

/*
  Freigeben der Node daten und der Node selber.
  node   = Freizugebende Node.
  return = void.
*/
void nodeFree( Node * node ) {
    assert( node != NULL );
    FREE( node->buf );
    FREE( node->addr );
    FREE( node );
}

/*EOF*/
