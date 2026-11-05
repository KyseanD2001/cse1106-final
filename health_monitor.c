#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define DEFAULT_THRESHOLD 80.0
#define DEFAULT_DIAG_FILE "diagnostic_results.txt"
#define STORAGE_PATH "/"

#define LED_GREEN_PIN 17
#define LED_YELLOW_PIN 27
#define LED_RED_PIN 22

typedef struct {
    double used;
    double total;
} ResourceUsage;

typedef struct {
    double threshold;
    char diag_file[PATH_MAX];
    int disable_led;
    int has_mem_override;
    int has_storage_override;
    ResourceUsage memory_override;
    ResourceUsage storage_override;
} Config;

static int parse_double_arg(const char *value, double *out) {
    char *end = NULL;
    errno = 0;
    double parsed = strtod(value, &end);
    if (errno != 0 || end == value || *end != '\0') {
        return -1;
    }
    *out = parsed;
    return 0;
}

static int parse_args(int argc, char **argv, Config *cfg) {
    cfg->threshold = DEFAULT_THRESHOLD;
    cfg->disable_led = 0;
    cfg->has_mem_override = 0;
    cfg->has_storage_override = 0;
    snprintf(cfg->diag_file, sizeof(cfg->diag_file), "%s", DEFAULT_DIAG_FILE);

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--threshold") == 0 && i + 1 < argc) {
            if (parse_double_arg(argv[++i], &cfg->threshold) != 0 || cfg->threshold <= 0.0 || cfg->threshold > 100.0) {
                fprintf(stderr, "Invalid threshold value. Use 0-100.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "--diag-file") == 0 && i + 1 < argc) {
            snprintf(cfg->diag_file, sizeof(cfg->diag_file), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--mem-used-mb") == 0 && i + 1 < argc) {
            if (parse_double_arg(argv[++i], &cfg->memory_override.used) != 0 || cfg->memory_override.used < 0.0) {
                fprintf(stderr, "Invalid --mem-used-mb value.\n");
                return -1;
            }
            cfg->has_mem_override = 1;
        } else if (strcmp(argv[i], "--mem-total-mb") == 0 && i + 1 < argc) {
            if (parse_double_arg(argv[++i], &cfg->memory_override.total) != 0 || cfg->memory_override.total <= 0.0) {
                fprintf(stderr, "Invalid --mem-total-mb value.\n");
                return -1;
            }
            cfg->has_mem_override = 1;
        } else if (strcmp(argv[i], "--storage-used-mb") == 0 && i + 1 < argc) {
            if (parse_double_arg(argv[++i], &cfg->storage_override.used) != 0 || cfg->storage_override.used < 0.0) {
                fprintf(stderr, "Invalid --storage-used-mb value.\n");
                return -1;
            }
            cfg->has_storage_override = 1;
        } else if (strcmp(argv[i], "--storage-total-mb") == 0 && i + 1 < argc) {
            if (parse_double_arg(argv[++i], &cfg->storage_override.total) != 0 || cfg->storage_override.total <= 0.0) {
                fprintf(stderr, "Invalid --storage-total-mb value.\n");
                return -1;
            }
            cfg->has_storage_override = 1;
        } else if (strcmp(argv[i], "--no-led") == 0) {
            cfg->disable_led = 1;
        } else {
            fprintf(stderr,
                    "Usage: %s [--threshold PERCENT] [--diag-file PATH] [--mem-used-mb N --mem-total-mb N] "
                    "[--storage-used-mb N --storage-total-mb N] [--no-led]\n",
                    argv[0]);
            return -1;
        }
    }

    if (cfg->has_mem_override) {
        if (cfg->memory_override.total <= 0.0 || cfg->memory_override.used < 0.0 ||
            cfg->memory_override.used > cfg->memory_override.total) {
            fprintf(stderr, "Both memory override values are required and used must be <= total.\n");
            return -1;
        }
    }

    if (cfg->has_storage_override) {
        if (cfg->storage_override.total <= 0.0 || cfg->storage_override.used < 0.0 ||
            cfg->storage_override.used > cfg->storage_override.total) {
            fprintf(stderr, "Both storage override values are required and used must be <= total.\n");
            return -1;
        }
    }

    return 0;
}

static int get_memory_usage(ResourceUsage *memory) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        perror("Failed to open /proc/meminfo");
        return -1;
    }

    unsigned long total_kb = 0;
    unsigned long avail_kb = 0;
    char key[64];
    unsigned long value = 0;
    char unit[16];

    while (fscanf(fp, "%63s %lu %15s", key, &value, unit) == 3) {
        if (strcmp(key, "MemTotal:") == 0) {
            total_kb = value;
        } else if (strcmp(key, "MemAvailable:") == 0) {
            avail_kb = value;
        }
        if (total_kb && avail_kb) {
            break;
        }
    }

    fclose(fp);

    if (total_kb == 0 || avail_kb > total_kb) {
        fprintf(stderr, "Unable to parse memory usage from /proc/meminfo\n");
        return -1;
    }

    memory->total = (double)total_kb / 1024.0;
    memory->used = (double)(total_kb - avail_kb) / 1024.0;
    return 0;
}

static int get_storage_usage(ResourceUsage *storage) {
    struct statvfs stat = {0};
    if (statvfs(STORAGE_PATH, &stat) != 0) {
        perror("Failed to read storage usage");
        return -1;
    }

    unsigned long long total_bytes = (unsigned long long)stat.f_blocks * (unsigned long long)stat.f_frsize;
    unsigned long long available_bytes = (unsigned long long)stat.f_bavail * (unsigned long long)stat.f_frsize;

    if (total_bytes == 0 || available_bytes > total_bytes) {
        fprintf(stderr, "Invalid storage values from filesystem stats\n");
        return -1;
    }

    storage->total = (double)total_bytes / (1024.0 * 1024.0);
    storage->used = (double)(total_bytes - available_bytes) / (1024.0 * 1024.0);
    return 0;
}

static double utilization_pct(const ResourceUsage *resource) {
    if (resource->total <= 0.0) {
        return 0.0;
    }
    return (resource->used / resource->total) * 100.0;
}

static int write_text_file(const char *path, const char *value) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return -1;
    }
    if (fputs(value, fp) == EOF) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

static int gpio_setup_pin(int pin) {
    char path[128];
    char pin_str[16];

    snprintf(pin_str, sizeof(pin_str), "%d", pin);
    if (write_text_file("/sys/class/gpio/export", pin_str) != 0 && errno != EBUSY) {
        return -1;
    }

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    if (write_text_file(path, "out") != 0) {
        return -1;
    }

    return 0;
}

static int gpio_set_pin(int pin, int on) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    return write_text_file(path, on ? "1" : "0");
}

static void set_led_status(const char *level, int disable_led) {
    if (disable_led) {
        return;
    }

    if (gpio_setup_pin(LED_GREEN_PIN) != 0 || gpio_setup_pin(LED_YELLOW_PIN) != 0 || gpio_setup_pin(LED_RED_PIN) != 0) {
        fprintf(stderr, "Warning: unable to initialize GPIO LEDs, continuing without LED output.\n");
        return;
    }

    int green = strcmp(level, "GREEN") == 0;
    int yellow = strcmp(level, "YELLOW") == 0;
    int red = strcmp(level, "RED") == 0;

    if (gpio_set_pin(LED_GREEN_PIN, green) != 0 || gpio_set_pin(LED_YELLOW_PIN, yellow) != 0 || gpio_set_pin(LED_RED_PIN, red) != 0) {
        fprintf(stderr, "Warning: unable to update GPIO LEDs.\n");
    }
}

static int save_diagnostics(const char *diag_path,
                            const ResourceUsage *memory,
                            const ResourceUsage *storage,
                            double mem_pct,
                            double storage_pct,
                            double threshold,
                            const char *status,
                            const char *led_state) {
    FILE *fp = fopen(diag_path, "a");
    if (!fp) {
        perror("Unable to open diagnostics file");
        return -1;
    }

    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    if (!tm_now) {
        fclose(fp);
        return -1;
    }

    char timestamp[64];
    if (strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_now) == 0) {
        fclose(fp);
        return -1;
    }

    fprintf(fp,
            "[%s] Memory: %.2f/%.2f MB (%.2f%%), Storage: %.2f/%.2f MB (%.2f%%), Threshold: %.2f%%, Status: %s, LED: %s\n",
            timestamp,
            memory->used,
            memory->total,
            mem_pct,
            storage->used,
            storage->total,
            storage_pct,
            threshold,
            status,
            led_state);

    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {
    Config cfg;
    if (parse_args(argc, argv, &cfg) != 0) {
        return 1;
    }

    ResourceUsage memory = {0};
    ResourceUsage storage = {0};

    if (cfg.has_mem_override) {
        memory = cfg.memory_override;
    } else if (get_memory_usage(&memory) != 0) {
        return 1;
    }

    if (cfg.has_storage_override) {
        storage = cfg.storage_override;
    } else if (get_storage_usage(&storage) != 0) {
        return 1;
    }

    double memory_pct = utilization_pct(&memory);
    double storage_pct = utilization_pct(&storage);
    double max_pct = memory_pct > storage_pct ? memory_pct : storage_pct;

    const char *status = (memory_pct >= cfg.threshold || storage_pct >= cfg.threshold) ? "WARNING" : "HEALTHY";
    const char *led_state = "GREEN";

    if (strcmp(status, "HEALTHY") == 0) {
        led_state = "GREEN";
    } else if (memory_pct >= cfg.threshold && storage_pct >= cfg.threshold) {
        led_state = "RED";
    } else if (max_pct >= cfg.threshold + 15.0 || max_pct >= 95.0) {
        led_state = "RED";
    } else {
        led_state = "YELLOW";
    }

    printf("Raspberry Pi Resource Health Monitor\n");
    printf("Memory Usage : %.2f / %.2f MB (%.2f%%)\n", memory.used, memory.total, memory_pct);
    printf("Storage Usage: %.2f / %.2f MB (%.2f%%)\n", storage.used, storage.total, storage_pct);
    printf("Warning Threshold: %.2f%%\n", cfg.threshold);
    printf("Status: %s\n", status);
    printf("LED State: %s\n", led_state);

    set_led_status(led_state, cfg.disable_led);

    if (save_diagnostics(cfg.diag_file, &memory, &storage, memory_pct, storage_pct, cfg.threshold, status, led_state) != 0) {
        return 1;
    }

    printf("Diagnostics saved to %s\n", cfg.diag_file);
    return 0;
}
