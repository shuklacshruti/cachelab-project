#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    // TODO: Your simulation code goes here

    return 0;
}
