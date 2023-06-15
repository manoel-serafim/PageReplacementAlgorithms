#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define MAX_REP 100
#define NUM_ALG 3
#define NUM_EXP 5

// TODO - mudar o tamanho do vetor de acordo com o tamanho da memória virtual | 1gb em bytes= 1.073.741.824
#define MV_SIZE 100
#define MR_SIZE 50
#define MAX_SIZE 100

typedef struct {
	unsigned short present:1;
	unsigned short r:1;
	unsigned short m:1;
	unsigned short real_page_frame:12;
	unsigned short counter:8;
    int frequency;
    int next;
} TPageType;

TPageType virtualMem[MV_SIZE];
int head;
int tail;
int clock_hand;

void insert(TPageType page) {
    int index = 0;
    if (head == -1) {
        index = 0;
        head = index;
        tail = index;
    } else {
        index = (tail + 1) % MAX_SIZE;
        virtualMem[tail].next = index;
        tail = index;
    }
    virtualMem[index] = page;
    virtualMem[index].next = head;
}

void remove_node(int index) {
    if (head == tail) {
        head = -1;
        tail = -1;
    } else {
        int prev_index = head;
        while (virtualMem[prev_index].next != index) {
            prev_index = virtualMem[prev_index].next;
        }
        virtualMem[prev_index].next = virtualMem[index].next;
        if (index == head) {
           head = virtualMem[index].next;
        }
        else if (index == tail) {
            tail = prev_index; 
        }
    }
    virtualMem[index].next = -1;
}

void print_list() {
    if (head == -1) {
        printf("List is empty\n");
        return;
    }
    int current_index = head;
    do {
        printf("%d ", virtualMem[current_index].real_page_frame);
        current_index = virtualMem[current_index].next;
    } while (current_index != head);
    
    printf("\n");
}

void init_clock() {
    clock_hand = head;
}

TPageType* get_page_to_replace(int page_index) {
    while (virtualMem[clock_hand].r) {
        virtualMem[clock_hand].r = 0;
        clock_hand = virtualMem[clock_hand].next;
    }
    TPageType *page = &(virtualMem[clock_hand]); 
    
    if (virtualMem[clock_hand].present == 1) {
        virtualMem[clock_hand].present = 0;
        virtualMem[clock_hand].r = 0;
    }
    virtualMem[clock_hand].real_page_frame = virtualMem[page_index].real_page_frame;
    virtualMem[clock_hand].present = 1;
    virtualMem[clock_hand].r = 1;
    virtualMem[clock_hand].m = virtualMem[page_index].m;

    
    clock_hand = virtualMem[clock_hand].next;
    return page;
}


int randon_gen(int lower, int upper){
	return(((rand() % (upper - lower + 1)) + lower));
}

void initialize_world(void) {
    srand(time(NULL));
    int virtualPage, realPage = 0;
    while (realPage < MR_SIZE) {
        virtualPage = randon_gen(0, MV_SIZE - 1);
        while (virtualMem[virtualPage].present) {
            virtualPage = randon_gen(0, MV_SIZE - 1);
        }
        virtualMem[virtualPage].present = 1;
        virtualMem[virtualPage].r = randon_gen(0, 1);
        virtualMem[virtualPage].m = randon_gen(0, 1);
        virtualMem[virtualPage].real_page_frame = realPage;
        virtualMem[virtualPage].frequency = 0;
        virtualMem[virtualPage].next = -1;
        realPage++;
    }

    for (int i = 0; i < MV_SIZE; i++) {
        if (virtualMem[i].present != 1) {
            virtualMem[i].frequency = 0;
            virtualMem[i].counter = 0;
            virtualMem[i].present = 0;
            virtualMem[i].real_page_frame = -1;
            virtualMem[i].m = 0;
            virtualMem[i].r = 0;
            virtualMem[virtualPage].next = -1;
        }
    }

    head = -1;
    tail = -1;
    clock_hand = 0;
}

int main() {
    initialize_world();

    int pageReferences = 10000;    // Número de referências de página

    // Run LFU algorithm
    int pageFaultsLFU = 0;      // Contador de faltas de página usando LFU

    for (int i = 0; i < pageReferences; i++) {
        // Gerar uma referência de página aleatória
        int pageIndex = randon_gen(0, 100);

        // Verificar se a página já está na memória real
        bool pageHit = false;

        if (virtualMem[pageIndex].present == 1) {
            pageHit = true;
            virtualMem[pageIndex].frequency++;
        }

        // Se a página não está na memória real, ocorre uma falta de página
        if (!pageHit) {
            pageFaultsLFU++;
            insert(*(get_page_to_replace(pageIndex)));
        }
    }

    printf("Page Faults (LFU): %d\n", pageFaultsLFU);

    return 0;
}

// int main(int argc, char *argv[]) {

//     int virtual_page, lower, upper;
//     int page_miss[NUM_ALG][NUM_EXP] = {0};

//     list l;
//     init_list(&l);

//     // Encher lista de paginas de 0 a 49
//     fill_page_list(&l);

//     TPageType* real_page;

//     for (int alg_replace = 0; alg_replace < NUM_ALG; alg_replace++) {

//         for (int experiment = 1; experiment <= NUM_EXP; experiment++) {

//             lower = (VM_size/2) - experiment * 10;
//             upper = ((VM_size/2) + experiment * 10) - 1;

//             for (int repeat_count = 0; repeat_count < MAX_REP; repeat_count++)  {
//                 virtual_page = randon_gen(repeat_count, lower, upper);
//                 real_page = search_page(&l, virtual_page);

//                 if (real_page == NULL) {
//                     real_page = trap(alg_replace, virtual_page);
//                     page_miss[alg_replace][experiment-1] = page_miss[alg_replace][experiment-1] + 1;
//                 }

//                 access(virtual_page, real_page);
//             }
//         }
//     }

//     return 0;
// }
