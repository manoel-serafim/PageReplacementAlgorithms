#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

///////////////////////// GENERIC /////////////////////////
#define MAX_REP 100
#define NUM_ALG 3
#define NUM_EXP 5

// TODO - mudar o tamanho do vetor de acordo com o tamanho da memória virtual | 1gb em bytes= 1.073.741.824
#define MV_SIZE 100
#define MR_SIZE 50

#define LFU 0
#define CLOCK 1
#define AGING 2

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
TPageType bkp_virtual_mem[MV_SIZE];


int randon_gen(int lower, int upper){
	return(((rand() % (upper - lower + 1)) + lower));
}

void initialize_world(void) {
    srand(1);
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

    memcpy(bkp_virtual_mem, virtualMem, sizeof(virtualMem));
}

//////////////////////////////////////////////////////////

///////////////////////// CLOCK /////////////////////////
void insert(TPageType page) {
    int index = 0;
    if (head == -1) {
        index = 0;
        head = index;
        tail = index;
    } else {
        index = (tail + 1) % MV_SIZE;
        virtualMem[tail].next = index;
        tail = index;
    }
    virtualMem[index] = page;
    virtualMem[index].next = head;
}

void clockReplace(int page_index) {
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

    insert(*page);
}

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// LFU //////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////
////////////////////////////////// AGING //////////////////////////////////

short get_real_page(unsigned short virtual_page)
{
	if (virtualMem[virtual_page].present)
	{
		return virtualMem[virtual_page].real_page_frame;
	}
	else
	{
		return -1;
	}
}

void aging(void){
	int i = 0;
	while(i < MV_SIZE){
		virtualMem[i].counter = virtualMem[i].counter >> 1;
		if((virtualMem[i].r) && (virtualMem[i].present)){
			virtualMem[i].counter = virtualMem[i].counter / 2;
			virtualMem[i].r = 0;
		}
		i++;
	}
}

unsigned short getAgingPage2Sub(void)
{
	int i = 0;
	int smaller = -1;
	unsigned short index_smaller;
	while (i < MV_SIZE)
	{
		if ((virtualMem[i].present) && (smaller == -1))
		{
			smaller = virtualMem[i].counter;
			index_smaller = i;
		}
		else if ((virtualMem[i].present) && (smaller < virtualMem[i].counter))
		{
			smaller = virtualMem[i].counter;
			index_smaller = i;
		}
		i++;
	}
	return (index_smaller);
}

unsigned short agingSub(unsigned short v_page)
{
	unsigned short i = getAgingPage2Sub();
	unsigned short new_page = virtualMem[i].real_page_frame;
	virtualMem[i].present = 0;
	virtualMem[i].r = 0;
	virtualMem[i].m = 0;
	virtualMem[i].real_page_frame = 0;
	virtualMem[i].counter = 0;

	virtualMem[v_page].present = 1;
	virtualMem[v_page].r = 0;
	virtualMem[v_page].m = 0;
	virtualMem[v_page].counter = 0;
	virtualMem[v_page].real_page_frame = new_page;
	return new_page;
}

void access_page(unsigned short real_page_frame, unsigned short virtual_page_frame, int access_type)
{
	if (virtualMem[virtual_page_frame].real_page_frame == real_page_frame)
	{
		virtualMem[virtual_page_frame].r = 1;
		if (access_type)
		{
			virtualMem[virtual_page_frame].m = 1;
		}
	}
	else
	{
		printf("deu ruim\n");
	}
}

////////////////////////////////////////////////////////////////////////////

void printResult(int page_miss[NUM_ALG][NUM_EXP]) {
    for (int i = 0; i < NUM_ALG; i++) {
        for (int j = 0; j < NUM_EXP; j++) {
            printf("%d ", page_miss[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {

    int new_page, lower, upper, real_page;
    int page_miss[NUM_ALG][NUM_EXP] = {0};

    initialize_world();


    for (int alg_replace = 0; alg_replace < NUM_ALG; alg_replace++) {

        for (int experiment = 1; experiment <= NUM_EXP; experiment++) {
            
            memcpy(virtualMem, bkp_virtual_mem, sizeof(bkp_virtual_mem));

            lower = (MV_SIZE/2) - experiment * 10;
            upper = ((MV_SIZE/2) + experiment * 10) - 1;
            srand(experiment);

            for (int repeat_count = 0; repeat_count < MAX_REP; repeat_count++)  {

                new_page = randon_gen(lower, upper);

                if (alg_replace == AGING)
                    real_page = get_real_page(new_page);

                if (virtualMem[new_page].present == 0) {
                    
                    if (alg_replace == LFU) {
                        replacePageLFU(new_page);
                    }
                    else if (alg_replace == CLOCK) {
                        clockReplace(new_page);
                    }
                    else {
                        real_page = agingSub(new_page);
                    }
                    
                    page_miss[alg_replace][experiment-1] = page_miss[alg_replace][experiment-1] + 1;
                }
                else {
                    if (alg_replace == LFU) {
                        virtualMem[new_page].frequency++;
                    }
                    else if (alg_replace == AGING) {
                        access_page(real_page, new_page, randon_gen(0, 1));
                    }
                }

            }
        }
    }

    printResult(page_miss);

    return 0;
}
