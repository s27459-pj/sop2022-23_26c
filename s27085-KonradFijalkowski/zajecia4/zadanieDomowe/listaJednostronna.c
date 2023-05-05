#include <stdio.h>
#include <malloc.h>

typedef struct Node{
    int data;
    struct Node* next;
} Node;

//funkcje ze strony:
//https://www.geeksforgeeks.org/insertion-in-linked-list/
void printList(Node* currentNode){
    printf("List: ");
    while(currentNode != NULL){
        printf("[%d] -> ", currentNode->data);
        currentNode = currentNode->next;
    }
    printf("NULL");
}

void appendNode(Node ** head, int newData){
    Node* newNode = malloc(sizeof(Node));
    Node* lastNode = *head;

    newNode->data=newData;
    newNode->next=NULL;

    if(*head == NULL){
        *head = newNode;
        return;
    }
    while(lastNode->next !=NULL){
        lastNode = lastNode->next;
    }
    lastNode->next = newNode;
    return;
}

void freeList(Node* node) {
    Node* nextNode = node;
    while (node != NULL) {
        nextNode = node->next;
        free(node);
        node = nextNode;
    }
}

int main(){
    struct Node* head = NULL;
    appendNode(&head, 6);
    printList(head);
    freeList(head);
    return 0;
}
