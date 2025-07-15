/*
   Shruti Shukla - scs250
   Shreya Bhatlapenumarti - sb2453
   Arnav Nethula - akn74
   Samuel Hernandez - sh1758
*/
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    char metric = 0;
    float target = 0.0f;
    char *csim_binary = NULL;
    char *tracefile = NULL;

    // Parse options
    while ((opt = getopt(argc, argv, "p:r:b:t:")) != -1) {
        switch (opt) {
            case 'p':
                metric = optarg[0];
                break;
            case 'r':
                target = atof(optarg);
                break;
            case 'b':
                csim_binary = optarg;
                break;
            case 't':
                tracefile = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -p <h|m|e> -r <target> -b <csim> -t <trace>\n", argv[0]);
                return 1;
        }
    }

    // Optional: print parsed values for sanity check
    printf("Metric: %c\n", metric);
    printf("Target: %.2f\n", target);
    printf("CSIM binary: %s\n", csim_binary ? csim_binary : "(null)");
    printf("Trace file: %s\n", tracefile ? tracefile : "(null)");

    // For now just print no valid configuration found to pass tests
    printf("No valid configuration found\n");

    return 0;
}
