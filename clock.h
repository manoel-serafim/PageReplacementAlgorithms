#include <stdio.h>
#include <stdlib.h>

#define VM_size 100
#define RM_size 50

typedef struct {
	unsigned short present;
	unsigned short r;
	unsigned short m;
	unsigned short real_page_frame;
} pageType;

// circular linked list

typedef struct node {
    pageType page;
    struct node *next;
} node;

typedef struct {
    node *head;
    node *tail;
} list;

void init_list(list *l) {
    l->head = NULL;
    l->tail = NULL;
}

void insert(list *l, pageType page) {
    node *new_node = (node*) malloc(sizeof(node));
    new_node->page = page;
    if (l->head == NULL) {
        l->head = new_node;
        l->tail = new_node;
        new_node->next = new_node;
    } else {
        new_node->next = l->head;
        l->tail->next = new_node;
        l->tail = new_node;
    }
}

void remove_node(list *l, node *n) {
    if (l->head == l->tail) {
        l->head = NULL;
        l->tail = NULL;
    } else {
        node *aux = l->head;
        while (aux->next != n) {
            aux = aux->next;
        }
        aux->next = n->next;
        if (n == l->head) {
            l->head = n->next;
        } else if (n == l->tail) {
            l->tail = aux;
        }
    }
    free(n);
}

void print_list(list *l) {
    node *aux = l->head;
    if (aux != NULL) {
        do {
            printf("%d ", aux->page.real_page_frame);
            aux = aux->next;
        } while (aux != l->head);
    }
    printf("\n");
}

// end | circular linked list



node* clock_hand;

void init_clock(list *l) {
    clock_hand = l->head;
}

pageType* get_page_to_replace() {
    while (clock_hand->page.r) {
        clock_hand->page.r = 0;
        clock_hand = clock_hand->next;
    }
    pageType *page = &(clock_hand->page); 
    clock_hand = clock_hand->next;
    return page;
}