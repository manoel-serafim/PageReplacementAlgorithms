#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MR_SIZE 50     // Tamanho da memória real
#define MV_SIZE 100    // Tamanho da memória virtual

typedef struct {
    unsigned short present:1;
    unsigned short r:1;
    unsigned short m:1;
    unsigned short real_page_frame:12;
    unsigned short counter:8;
    unsigned short garbage:8;
} TPageType;

TPageType virtualMem[MV_SIZE];

int randon_gen(int lower, int upper) {
    return (((rand() % (upper - lower + 1)) + lower));
}

int findClockPage(int startIndex) {
    int index = startIndex;

    while (true) {
        if (virtualMem[index].r == 0) {
            return index;
        }

        virtualMem[index].r = 0;
        index = (index + 1) % MV_SIZE;
    }
}

void replacePageClock(int pageIndex, int* pageFaults) {
    int clockIndex = findClockPage(pageIndex);

    if (virtualMem[clockIndex].present == 1) {
        virtualMem[clockIndex].present = 0;
        virtualMem[clockIndex].r = 0;
    }

    virtualMem[clockIndex].real_page_frame = virtualMem[pageIndex].real_page_frame;
    virtualMem[clockIndex].present = 1;
    virtualMem[clockIndex].r = 1;
    virtualMem[clockIndex].m = virtualMem[pageIndex].m;
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
        realPage++;
    }

    for (int i = 0; i < MV_SIZE; i++) {
        if (virtualMem[i].present != 1) {
            virtualMem[i].present = 0;
            virtualMem[i].r = 0;
            virtualMem[i].m = 0;
            virtualMem[i].real_page_frame = -1;
            virtualMem[i].counter = 0;
            virtualMem[i].garbage = 0;
        }
    }
}

int main() {
    srand(time(NULL));  // Inicializar o gerador de números aleatórios

    initialize_world();

    int pageReferences = 10000;    // Número de referências de página

    // Run Clock algorithm
    int pageFaultsClock = 0;      // Contador de faltas de página usando Clock

    int clockHand = 0;   // Índice para a mão do relógio

    for (int i = 0; i < pageReferences; i++) {
        // Gerar uma referência de página aleatória
        int pageIndex = randon_gen(0, MV_SIZE - 1);

        // Verificar se a página já está na memória real
        bool pageHit = false;

        if (virtualMem[pageIndex].present == 1) {
            pageHit = true;
            virtualMem[pageIndex].r = 1;
        }

        // Se a página não está na memória real, ocorre uma falta de página
        if (!pageHit) {
            pageFaultsClock++;
            replacePageClock(pageIndex, &pageFaultsClock);
        }
    }

    printf("Page Faults (Clock): %d\n", pageFaultsClock);

    return 0;
}
