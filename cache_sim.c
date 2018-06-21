#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

enum cache_block_state {
	INVALID,
	VALID
};

typedef unsigned uint;

typedef struct cache_block{
	enum cache_block_state status;
	uint tag;
	uint time;
}cache_block;

uint sets;
uint assoc;
uint block;
uint block_log2 = 0;
uint total_cache_accesses = 0;
uint total_cache_misses = 0;
uint sim_time = 0;
struct cache_block *cache;

void cache_access(char inst_type, uint addr);

void cache_access(char inst_type, uint addr){
    //TODO: fill you code here
    uint span_of_index = log2(sets);
    uint index = 0;

    uint sum_of_two = (span_of_index+block_log2);
    uint mask = pow (2, sum_of_two) - 1 ;
    index = addr & mask;
    index = index / pow(2, block_log2);

    uint tag;
    tag = (addr / pow(2, sum_of_two));


    uint off = assoc * index;
    cache_block* block;
    int i;
    bool found = false;
    for (i = 0; i < assoc; i++) {
        cache_block* block_from_cache = &cache[off + i];
        if (block_from_cache->tag == tag){
            block = block_from_cache;
            found = true;
            break;
        }
    }



    if(!found){
        block = NULL;
    }

    bool has_place = false;
    bool found_next_empty_block = false;

    if(block != NULL){
        block->time = 0;

    } else {
        total_cache_misses++;
        cache_block* block = NULL;
        // Check if set full or not

        for (i = 0; i < assoc; i++) {
            cache_block* block_from_set = &cache[off + i];
            if (block_from_set->status == INVALID){
                has_place = true;
                break;
            }
        }
        if(has_place){
            for (i = 0; i < assoc; i++) {
                cache_block* empty_block = &cache[off + i];
                if (empty_block->status == INVALID){
                    block = empty_block;
                    found_next_empty_block = true;
                    break;
                }
            }



            if(!found_next_empty_block){
                block = NULL;
            }
        } else {

            int LRU_index = 0;
            uint MAX = 0;
            i = 0;
            while(i < assoc){
                cache_block* LRU_block = &cache[off + i];
                if (LRU_block->time > MAX) {
                MAX = LRU_block->time;
                LRU_index = i;
              }
              ++i;
            }

          block = &cache[off + LRU_index];
        }

        block->time = 0;
        block->status = VALID;
        block->tag = tag;

    }

        for (i = 0; i < assoc; i++) {
            cache_block* blockS = &cache[off + i];
            if (blockS->tag != tag) {
              (blockS->time) ++;
            }
        }

   // printf("%x\n", tag);
}

int main(int argc, char** argv)
{
    if (argc != 5) {
        printf("Usage: %s <number_of_sets> <associativity> <cache_block_size> <trace_file>\n", argv[0]);
        return -1;
    }
    /*
       input parameters
       argv[1] = number of sets
       argv[2] = associativity
       argv[3] = cache block size
       argv[4] = trace file name
    */
    char inst_type = 0;
    uint addr = 0;
    sets = atoi(argv[1]);
    assoc = atoi(argv[2]);
    block = atoi(argv[3]);
    while (block >>= 1) ++block_log2;

    FILE *fp = fopen(argv[4], "r");
    if (fp == NULL){
        printf("trace file does not exist\n");
        return -1;
    }

    cache=malloc(sizeof(struct cache_block)*sets*assoc);
    int i=0;
    for(i=0;i<sets*assoc;i++){
        cache[i].time=0;
        cache[i].tag=0;
        cache[i].status=INVALID;
    }

    while (fscanf(fp, "%c 0x%x\n", &inst_type, &addr) != EOF) {
        cache_access(inst_type, addr);
        total_cache_accesses++;
	    sim_time++;
    }

    printf("Cache accesses = %u\n", total_cache_accesses);
    printf("Cache misses = %u\n", total_cache_misses);


    free(cache);
    fclose(fp);

    return 0;
}

//void cache_access(char inst_type, uint addr)
//{
//    printf("type = %c, addr = 0x%08x\n", inst_type, addr);
//}

