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
    int frequency;
} TPageType;

TPageType virtualMem[MV_SIZE];

int randon_gen(int lower, int upper) {
    return (((rand() % (upper - lower + 1)) + lower));
}

int findLFUPage() {
    int lfuIndex = 0;
    int minFrequency = virtualMem[0].frequency;

    for (int i = 1; i < MV_SIZE; i++) {
        if (virtualMem[i].frequency < minFrequency && virtualMem[i].present == 1) {
            minFrequency = virtualMem[i].frequency;
            lfuIndex = i;
        }
    }

    return lfuIndex;
}

void replacePageLFU(int pageIndex) {
    unsigned short  lfuIndex = findLFUPage();
    int replacedPage = virtualMem[lfuIndex].real_page_frame;

    int present = virtualMem[lfuIndex].real_page_frame;
    int real_page_frame = virtualMem[lfuIndex].frequency;
    int frequency = virtualMem[lfuIndex].present ;

    virtualMem[lfuIndex].real_page_frame = virtualMem[pageIndex].real_page_frame;
    virtualMem[lfuIndex].frequency = virtualMem[pageIndex].frequency;
    virtualMem[lfuIndex].present = virtualMem[pageIndex].present;

    virtualMem[pageIndex].real_page_frame = real_page_frame;
    virtualMem[pageIndex].frequency = frequency;
    virtualMem[pageIndex].present = present;
}

void initialize_world(void) {
    srand(3);
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
        }
    }
}

int main() {
    initialize_world();

    int pageReferences = 1000;    // Número de referências de página

    // Run LFU algorithm
    int pageFaultsLFU = 0;      // Contador de faltas de página usando LFU

    for (int i = 0; i < pageReferences; i++) {
        // Gerar uma referência de página aleatória
        int pageIndex = rand() % MV_SIZE;

        // Verificar se a página já está na memória real
        bool pageHit = false;

        if (virtualMem[pageIndex].present == 1) {
            pageHit = true;
            virtualMem[pageIndex].frequency++;
        }

        // Se a página não está na memória real, ocorre uma falta de página
        if (!pageHit) {
            pageFaultsLFU++;
            replacePageLFU(pageIndex);
        }
    }

    printf("Page Faults (LFU): %d\n", pageFaultsLFU);

    return 0;
}
