#include<stdio.h>
#include<stdlib.h>
#include<time.h>

// 100 000 000 acessos a memoria 100 000 um tick de relogio

#define MAX_ACCESS 100
#define VM_size 100
#define RM_size 50


typedef struct {
	unsigned short present:1;
	unsigned short r:1;
	unsigned short m:1;
	unsigned short real_page_frame:12;
	unsigned short counter:8;
	unsigned short garbage:8;
} TPageType;

int randon_gen(int lower, int upper);
void initialize_world(void);
void print_vm(void);
short get_real_page(unsigned short virtual_page);
void aging(void);
unsigned short get_page_to_sub(void);
unsigned short page_sub(unsigned short v_page);
void access_page(unsigned short real_page_frame, unsigned short virtual_page_frame, int access_type);

TPageType virtualMem[VM_size];

int main(void){
	initialize_world();
	int i = 0;
	int page_miss = 0;
	short v_page, r_page;
	srand(time(NULL));
	while(i < 100){
		if((i%10) == 0){
			aging();
		}
		v_page = randon_gen(0, 100);
		r_page = get_real_page(v_page);
		if(r_page == -1){
			page_miss++;
			//r_page = page_sub(v_page);
		}
		else
		{
			access_page(r_page, v_page, randon_gen(0, 1));
		}
		i++;
	}
	//print_vm();
	printf("page_miss = %d\n", page_miss);
	return 0;
}

int randon_gen(int lower, int upper){
	return(((rand() % (upper - lower + 1)) + lower));
}

void initialize_world(void){
	srand(3);
	int virtualPage, realPage = 0;
	while(realPage < RM_size){
		virtualPage = randon_gen(0, 99);
		while(virtualMem[virtualPage].present){
			virtualPage = randon_gen(0, 99);
		}
		virtualMem[virtualPage].present = 1;
		virtualMem[virtualPage].r = randon_gen(0, 1);
		virtualMem[virtualPage].m = randon_gen(0, 1);
		virtualMem[virtualPage].real_page_frame = realPage;
		realPage++;
	}
}

void print_vm(void){
	int i = 0;
	int cont = 0;
	while(i < VM_size){
		if(virtualMem[i].present){
			printf("virtual address: %d, r bit: %d, m bit: %d, real page frame: %d, counter: %d\n", i, virtualMem[i].r, virtualMem[i].m, virtualMem[i].real_page_frame, virtualMem[i].counter);
			cont++;
		}
		else{
			printf("virtual address: %d, not mapped\n", i);
		}
		i++;
	}
	printf("%d virtual addresses mapped in RM\n", cont);
}

void access_page(unsigned short real_page_frame, unsigned short virtual_page_frame, int access_type){
	if(virtualMem[virtual_page_frame].real_page_frame == real_page_frame){
		virtualMem[virtual_page_frame].r = 1;
		if(access_type){
			virtualMem[virtual_page_frame].m = 1;
		}
	}
	else{
		printf("deu ruim\n");
	}
}

short get_real_page(unsigned short virtual_page){
	if(virtualMem[virtual_page].present){
		return virtualMem[virtual_page].real_page_frame;
	}
	else{
		return -1;
	}
}

void aging(void){
	int i = 0;
	while(i < VM_size){
		virtualMem[i].counter = virtualMem[i].counter >> 1;
		if((virtualMem[i].r) && (virtualMem[i].present)){
			virtualMem[i].counter = virtualMem[i].counter | 128;
			virtualMem[i].r = 0;
		}
		i++;
	}
}

unsigned short get_page_to_sub(void){
	int i = 0;
	int smaller = -1;
	unsigned short index_smaller;
	while(i < VM_size){
		if ((virtualMem[i].present) && (smaller == -1)){
			smaller = virtualMem[i].counter;
			index_smaller = i;
		}
		else if((virtualMem[i].present) && (smaller < virtualMem[i].counter)){
			smaller = virtualMem[i].counter;
			index_smaller = i;
		}
		i++;
	}
	return(index_smaller);
}

unsigned short page_sub(unsigned short v_page){
	unsigned short i = get_page_to_sub();
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