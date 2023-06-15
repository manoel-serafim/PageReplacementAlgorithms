#include <stdio.h>
#include <stdlib.h>

#define MAX_REP 100
#define NUM_ALG 3
#define NUM_EXP 5

// TODO - mudar o tamanho do vetor de acordo com o tamanho da memória virtual | 1gb em bytes= 1.073.741.824
#define VM_size 100
#define RM_size 50

typedef struct {
	unsigned short present;
	unsigned short r;
	unsigned short m;
	unsigned short real_page_frame;
} pageType;

#define MAX_SIZE 100

typedef struct {
    pageType page;
    int next;
} node;

typedef struct {
    node nodes[MAX_SIZE];
    int head;
    int tail;
} list;

void init_list(list *l) {
    l->head = -1;
    l->tail = -1;
    for (int i = 0; i < MAX_SIZE; i++) {
        l->nodes[i].next = -1;
    }
}

void insert(list *l, pageType page) {
    int index = 0;
    if (l->head == -1) {
        index = 0;
        l->head = index;
        l->tail = index;
    } else {
        index = (l->tail + 1) % MAX_SIZE;
        l->nodes[l->tail].next = index;
        l->tail = index;
    }
    l->nodes[index].page = page;
    l->nodes[index].next = l->head;
}

void remove_node(list *l, int index) {
    if (l->head == l->tail) {
        l->head = -1;
        l->tail = -1;
    } else {
        int prev_index = l->head;
        while (l->nodes[prev_index].next != index) {
            prev_index = l->nodes[prev_index].next;
        }
        l->nodes[prev_index].next = l->nodes[index].next;
        if (index == l->head) {
            l->head = l->nodes[index].next;
        } else if (index == l->tail) {
            l->tail = prev_index;
        }
    }
    l->nodes[index].next = -1;
}

void print_list(list *l) {
    if (l->head == -1) {
        printf("List is empty\n");
        return;
    }
    int current_index = l->head;
    do {
        printf("%d ", l->nodes[current_index].page.real_page_frame);
        current_index = l->nodes[current_index].next;
    } while (current_index != l->head);
    printf("\n");
}

// end | circular linked list

int clock_hand;

void init_clock(list *l) {
    clock_hand = l->head;
}

pageType* get_page_to_replace(list *l) {
    while (l->nodes[clock_hand].page.r) {
        l->nodes[clock_hand].page.r = 0;
        clock_hand = l->nodes[clock_hand].next;
    }
    pageType *page = &(l->nodes[clock_hand].page); 
    clock_hand = l->nodes[clock_hand].next;
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
    int current_index = l->head;
    do {
        if (l->nodes[current_index].page.real_page_frame == pag_virtual) {
            return &(l->nodes[current_index].page);
        }
        current_index = l->nodes[current_index].next;
    } while (current_index != l->head);
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
