#include <stdio.h>
#include <stdlib.h>

#define MAX_REP 100
#define NUM_ALG 3
#define NUM_EXP 5

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



int randon_gen(int seed, int lower, int upper){
    srand(seed);
	return(((rand() % (upper - lower + 1)) + lower));
}

void fill_page_list(list *l) {
    for (int i = 0; i < VM_size; i++) {
        pageType page;
        page.present = 1;
        page.r = 1; // Definindo r como 1 para todas as páginas
        page.m = (i % 6 == 0) ? 1 : 0; // Definindo m como 1 a cada 6 paginas
        page.real_page_frame = i;
        insert(l, page);
    }
}

pageType* search_page(list *l, int pag_virtual) {
    node *current = l->head;
    do {
        if (current->page.real_page_frame == pag_virtual) {
            return &(current->page);
        }
        current = current->next;
    } while (current != l->head);
    return NULL;
}

pageType* trap(int alg_replace, int virtual_page) {
    // CHAMAR O ALGORITMO DE CADA UM PARA FAZER REPLACE

    return NULL;
}

void access(int virtual_page, pageType* real_page) {
    // CRIAR ACESSO MODIFICADO
    real_page->present = 1;
    real_page->r = 1;
    real_page->real_page_frame = virtual_page;

}

int main(int argc, char *argv[]) {

    int virtual_page, lower, upper;
    int page_miss[NUM_ALG][NUM_EXP] = {0};

    list l;
    init_list(&l);

    // Encher lista de paginas de 0 a 49
    fill_page_list(&l);

    pageType* real_page;

    for (int alg_replace = 0; alg_replace < NUM_ALG; alg_replace++) {

        for (int experiment = 1; experiment <= NUM_EXP; experiment++) {

            lower = (VM_size/2) - experiment * 10;
            upper = ((VM_size/2) + experiment * 10) - 1;

            for (int repeat_count = 0; repeat_count < MAX_REP; repeat_count++)  {
                virtual_page = randon_gen(repeat_count, lower, upper);
                real_page = search_page(&l, virtual_page);

                if (real_page == NULL) {
                    real_page = trap(alg_replace, virtual_page);
                    page_miss[alg_replace][experiment-1] = page_miss[alg_replace][experiment-1] + 1;
                }

                access(virtual_page, real_page);
            }
        }
    }

    return 0;
}
