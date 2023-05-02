/* https://en.wikipedia.org/wiki/Doubly_linked_list */

#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int value;
    struct node* next;
    struct node* prev;
} node;

node* create_node(int value, node* next) {
    node* n = malloc(sizeof(node));
    if (n == NULL) {
        perror("malloc");
        return NULL;
    }
    n->value = value;
    n->next = next;
    n->prev = NULL;
    /* Jeśli podano `next` to trzeba też ustawić w nim `prev` */
    if (next) next->prev = n;
    return n;
}

void clear_linked_list(node* head) {
    node* n = head;
    while (n != NULL) {
        node* next = n->next;
        free(n);
        n = next;
    }
}

void print_linked_list(node* head) {
    while (head != NULL) {
        if (head->prev == NULL) {
            printf("[X] <-> ");
        }
        printf("[%d]", head->value);
        if (head->next) {
            printf(" <-> ");
        } else {
            printf(" <-> [X]");
        }
        head = head->next;
    }
    printf("\n");
}

int main(int argc, char const* argv[]) {
    int i;
    node* head = NULL;

    if (argc < 2) {
        printf("Usage: %s <n> ...\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        int value = atoi(argv[i]);
        head = create_node(value, head);
    }

    print_linked_list(head);
    clear_linked_list(head);

    return 0;
}
