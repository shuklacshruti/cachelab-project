/*
 * Shruti Shukla - scs250
 * Shreya Bhatlapenumarti - sb2453
 * Arnav Nethula - akn74
 * Samuel Hernandez - sh1758
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  

#define MAX_CMD_LEN 256

typedef struct {
    int s;
    int E;
    int b;
} cache_config_t;

typedef struct {
    int hits;
    int misses;
    int evictions;
} cache_stats_t;

char perf_metric = 0; 
float target_rate = 0.0;
char *csim_binary = NULL;
char *trace_file = NULL;

void print_usage(char *progname) {
    printf("Usage: %s -p <h|m|e> -r <rate> -b <csim binary> -t <tracefile>\n", progname);
    printf("  -p <h|m|e>    Performance metric: hit(h), miss(m), eviction(e)\n");
    printf("  -r <rate>     Target rate (float 0.00-100.00)\n");
    printf("  -b <binary>   Path to cache simulator binary\n");
    printf("  -t <file>     Trace file\n");
}

void parse_args(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:r:b:t:")) != -1) {
        switch (opt) {
            case 'p':
                if (strlen(optarg) != 1 || (optarg[0] != 'h' && optarg[0] != 'm' && optarg[0] != 'e')) {
                    fprintf(stderr, "Invalid performance metric: %s\n", optarg);
                    print_usage(argv[0]);
                    exit(1);
                }
                perf_metric = optarg[0];
                break;
            case 'r':
                target_rate = atof(optarg);
                if (target_rate < 0.0 || target_rate > 100.0) {
                    fprintf(stderr, "Target rate must be between 0.00 and 100.00\n");
                    print_usage(argv[0]);
                    exit(1);
                }
                break;
            case 'b':
                csim_binary = optarg;
                break;
            case 't':
                trace_file = optarg;
                break;
            default:
                print_usage(argv[0]);
                exit(1);
        }
    }

    if (!perf_metric || !csim_binary || !trace_file) {
        fprintf(stderr, "Missing required arguments\n");
        print_usage(argv[0]);
        exit(1);
    }
}

int run_csim_and_get_stats(cache_config_t config, cache_stats_t *stats) {
    char cmd[MAX_CMD_LEN];
    char line[256];
    FILE *fp;

    snprintf(cmd, MAX_CMD_LEN, "%s -s %d -E %d -b %d -t %s", csim_binary, config.s, config.E, config.b, trace_file);

    fp = popen(cmd, "r");
    if (!fp) {
        fprintf(stderr, "Failed to run command: %s\n", cmd);
        return -1;
    }

    stats->hits = 0;
    stats->misses = 0;
    stats->evictions = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "hits:%d misses:%d evictions:%d", &stats->hits, &stats->misses, &stats->evictions) == 3) {
            pclose(fp);
            return 0;
        }
    }

    pclose(fp);
    return -1;
}

float compute_metric(cache_stats_t *stats, char metric) {
    int total = stats->hits + stats->misses;
    if (total == 0) return 0.0;

    switch(metric) {
        case 'h': return (float)stats->hits / total * 100.0f;
        case 'm': return (float)stats->misses / total * 100.0f;
        case 'e': return (float)stats->evictions / total * 100.0f;
        default: return 0.0;
    }
}

int is_smaller_config(cache_config_t a, cache_config_t b) {
    if (a.s < b.s) return 1;
    if (a.s > b.s) return 0;
    if (a.E < b.E) return 1;
    if (a.E > b.E) return 0;
    return a.b < b.b;
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    cache_config_t best_config = {0,0,0};
    cache_stats_t best_stats = {0,0,0};
    int found_valid = 0;
    float best_metric = 0.0;

    (void)best_stats;  // suppress unused warning for now

    // use search space from project directions: s=1-5, E=1-4, b=1-5
    for (int s = 1; s <= 5; s++) {
        for (int E = 1; E <= 4; E++) {
            for (int b = 1; b <= 5; b++) {
                cache_config_t cfg = {s, E, b};
                cache_stats_t stats;
                if (run_csim_and_get_stats(cfg, &stats) != 0) {
                    continue;
                }

                float metric_value = compute_metric(&stats, perf_metric);

                int metric_ok = 0;
                if (perf_metric == 'h') {
                    metric_ok = (metric_value >= target_rate);
                } else {
                    metric_ok = (metric_value <= target_rate);
                }

                if (metric_ok) {
                    if (!found_valid || metric_value > best_metric || 
                        (metric_value == best_metric && is_smaller_config(cfg, best_config))) {
                        best_config = cfg;
                        best_stats = stats;
                        best_metric = metric_value;
                        found_valid = 1;
                    }
                }
            }
        }
    }

    if (found_valid) {
        printf("%s -s %d -E %d -b %d -t %s\n", csim_binary, best_config.s, best_config.E, best_config.b, trace_file);
    } else {
        printf("No valid configuration found\n");
    }

    return 0;
}
