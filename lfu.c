#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MR_SIZE 128     // Tamanho da memória real
#define MV_SIZE 1024    // Tamanho da memória virtual
#define MS_SIZE (MV_SIZE - MR_SIZE)  // Tamanho da memória em disco (swap)

// Estrutura para representar uma página
typedef struct {
    int pageNumber;
    int frequency;
} Page;

// Função para inicializar uma página
void initializePage(Page* page, int pageNumber) {
    page->pageNumber = pageNumber;
    page->frequency = 0;
}

// Função para encontrar a página menos frequentemente usada na memória real
int findLFUPage(Page* mr) {
    int lfuIndex = 0;
    int minFrequency = mr[0].frequency;

    for (int i = 1; i < MR_SIZE; i++) {
        if (mr[i].frequency < minFrequency) {
            minFrequency = mr[i].frequency;
            lfuIndex = i;
        }
    }

    return lfuIndex;
}

// Função para substituir uma página na memória real usando o algoritmo LFU
void replacePageLFU(Page* mr, Page* mv, Page* ms, int pageIndex, int* pageFaults) {
    int lfuIndex = findLFUPage(mr);
    int replacedPage = mr[lfuIndex].pageNumber;

    mr[lfuIndex] = mv[pageIndex];

    bool isPageInSwap = false;
    for (int i = 0; i < MS_SIZE; i++) {
        if (ms[i].pageNumber == replacedPage) {
            isPageInSwap = true;
            break;
        }
    }

    if (isPageInSwap) {
        (*pageFaults)++;
    }

    mv[pageIndex].pageNumber = -1;
}

int main() {
    srand(time(NULL));  // Inicializar o gerador de números aleatórios

    Page mr[MR_SIZE];   // Memória real
    Page mv[MV_SIZE];   // Memória virtual
    Page ms[MS_SIZE];   // Memória em disco (swap)

    // Inicializar as páginas da memória real e virtual
    for (int i = 0; i < MR_SIZE; i++) {
        initializePage(&mr[i], -1);
    }

    for (int i = 0; i < MV_SIZE; i++) {
        initializePage(&mv[i], i + 1);
    }

    // Inicializar as páginas da memória em disco (swap)
    for (int i = 0; i < MS_SIZE; i++) {
        initializePage(&ms[i], -1);
    }

    int pageReferences = 1000;    // Número de referências de página

    // Run LFU algorithm
    int pageFaultsLFU = 0;      // Contador de faltas de página usando LFU

    for (int i = 0; i < pageReferences; i++) {
        // Gerar uma referência de página aleatória
        int pageIndex = rand() % MV_SIZE;

        // Verificar se a página já está na memória real
        bool pageHit = false;
        for (int j = 0; j < MR_SIZE; j++) {
            if (mr[j].pageNumber == mv[pageIndex].pageNumber) {
                pageHit = true;
                mr[j].frequency++;
                break;
            }
        }

        // Se a página não está na memória real, ocorre uma falta de página
        if (!pageHit) {
            pageFaultsLFU++;
            replacePageLFU(mr, mv, ms, pageIndex, &pageFaultsLFU);
        }
    }

    printf("Page Faults (LFU): %d\n", pageFaultsLFU);

    return 0;
}