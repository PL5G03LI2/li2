#include <stdlib.h>

typedef struct node
{
    void *data;
    struct node *next;
} Node;

Node *push(Node *previous, void *data)
{
    Node *new = (Node *)malloc(sizeof(Node));

    previous->next = new;

    new->data = data;
    new->next = NULL;

    return previous;
}