/*
 * TODO: add names and netids here
 */
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HIT 0
#define MISS 1
#define EVICTION 2

int hits = 0, misses = 0, evictions = 0;

typedef struct {
    int valid; 
    unsigned long tag; 
    int lru_counter; 
} cache_line_t; 

typedef struct {
    cache_line_t *lines;
} cache_set_t; 

typedef struct {
    cache_set_t *sets; 
    int s;
    int E;
    int b;
    int S;
} cache_t; 

cache_t cache;

// COMPLETED TODO: implement cache_access function that returns HIT, MISS, or EVICTION and increments hits, misses, evictions variables
int cache_access(unsigned long addr) {
    unsigned long setIndex = (addr >> cache.b) & ((1 << cache.s) - 1); 
    unsigned long tag = addr >> (cache.s + cache.b); 

    cache_set_t *set = &cache.sets[setIndex];
    
    for(int i = 0; i < cache.E; i++)
    {
        if(set->lines[i].valid && set->lines[i].tag == tag)
        {
            set->lines[i].lru_counter = 0; 
            for(int j = 0; j < cache.E; j++)
            {
                if(j != i && set->lines[j].valid)
                {
                    set->lines[j].lru_counter  = set->lines[j].lru_counter + 1; 
                }
            }
            hits = hits + 1; 
            return HIT; 
        }
    }

    int lruLine = 0; 
    int emptyLine = -1;
    int maxLru = set->lines[0].lru_counter;  
    for(int i = 0; i < cache.E; i++)
    {
        if(!set->lines[i].valid)
        {
            emptyLine = i;
            break; 
        }
        if(set->lines[i].lru_counter > maxLru)
        {
            maxLru = set->lines[i].lru_counter; 
            lruLine = i; 
        }
    }
    int r; 
    if(emptyLine == -1)
    {
        set->lines[lruLine].tag = tag; 
        set->lines[lruLine].lru_counter = 0; 
        r = EVICTION; 
        misses = misses + 1; 
        evictions = evictions + 1; 
    }
    else
    {
        set->lines[emptyLine].valid = 1;
        set->lines[emptyLine].tag = tag; 
        set->lines[emptyLine].lru_counter = 0;
        r = MISS; 
        misses = misses + 1; 
    }

    for(int i = 0; i < cache.E; i++)
    {
        if(set->lines[i].valid)
        {
            if(i != emptyLine && emptyLine != -1)
            {
                set->lines[i].lru_counter = set->lines[i].lru_counter + 1;
            }
            else if(i != lruLine && emptyLine == -1)
            {
                set->lines[i].lru_counter = set->lines[i].lru_counter + 1; 
            }
        }
    }
    return r;
}

void print_usage(char *argv0) {
    printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", argv0);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <s>     Number of set index bits.\n");
    printf("  -E <E>     Number of lines per set (associativity).\n");
    printf("  -b <b>     Number of block bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExample: %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv0);
}

int main(int argc, char **argv) {
    int s = -1, E = -1, b = -1;
    char *tracefile = NULL;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }

    // Check for required arguments
    if (s == -1 || E == -1 || b == -1 || tracefile == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv[0]);
        exit(1);
    }

    // At this point, you have your parameters in s, E, b, tracefile, and verbose

    cache.s = s;
    cache.E = E;
    cache.b = b;
    cache.S = 1 << s; // we have 2^s sets
    
    cache.sets = (cache_set_t*)malloc(cache.S * sizeof(cache_set_t));
    for (int i = 0; i < cache.S; i++) {
        cache.sets[i].lines = (cache_line_t*)malloc(cache.E * sizeof(cache_line_t));
        for (int j = 0; j < cache.E; j++) {
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru_counter = 0;
        }
    }

    FILE *fp = fopen(tracefile, "r");
    if (fp == NULL) {
        fprintf(stderr, "cannot open trace file %s: %m\n", tracefile);
        exit(1);
    }

    char op;
    unsigned long addr;
    int size;
    
    const char* result_str[] = {"hit", "miss", "miss eviction"};
    
    while (fscanf(fp, " %c %lx,%d", &op, &addr, &size) == 3) {
        if (op == 'I') continue;
        if (verbose) {
            printf("%c %lx,%d ", op, addr, size);
        }
        switch (op) {
            case 'L': // Load
            case 'S': // Store
                if (verbose) printf("%s\n", result_str[cache_access(addr)]);
                else cache_access(addr);
                break;
            case 'M': // Modify (load + store)
                if (verbose) printf("%s hit\n", result_str[cache_access(addr)]);
                else cache_access(addr);
                assert(cache_access(addr) == HIT);
                break;
            default: // Unknown
                break;
        }
    }
    fclose(fp);
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);

    for (int i = 0; i < cache.S; i++) {
        free(cache.sets[i].lines);
    }
    free(cache.sets);

    return 0;
}