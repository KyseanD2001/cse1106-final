#include <stdio.h>
#include <unistd.h>

#if __has_include(<pigpio.h>)
#include <pigpio.h>
#define HAS_PIGPIO 1
#else
#define HAS_PIGPIO 0
#endif

#define GREEN_LED_PIN 17
#define YELLOW_LED_PIN 27
#define RED_LED_PIN 22

typedef struct {
    float total;
    float used;
    float percent;
} UsageInfo;

typedef enum {
    STATUS_HEALTHY,
    STATUS_WARNING,
    STATUS_CRITICAL
} SystemStatus;

typedef struct {
    float threshold;
    float hysteresis;
    int override_values;
    float memory_percent_override;
    float storage_percent_override;
} MonitorConfig;

static int read_memory_usage(UsageInfo *memory) {
    FILE *fp = fopen("/proc/meminfo", "r");
    char line[128];
    float total_kb = 0.0f;
    float available_kb = 0.0f;

    if (!fp) {
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %f kB", &total_kb) == 1) {
            continue;
        }
        if (sscanf(line, "MemAvailable: %f kB", &available_kb) == 1) {
            continue;
        }
        if (total_kb > 0.0f && available_kb > 0.0f) {
            break;
        }
    }

    fclose(fp);

    if (total_kb <= 0.0f || available_kb < 0.0f || available_kb > total_kb) {
        return -1;
    }

    memory->total = total_kb / 1024.0f;
    memory->used = (total_kb - available_kb) / 1024.0f;
    memory->percent = (memory->used / memory->total) * 100.0f;
    return 0;
}

static int read_storage_usage(UsageInfo *storage) {
    FILE *fp = popen("df -P / | tail -1", "r");
    float blocks = 0.0f;
    float used = 0.0f;
    float available = 0.0f;

    if (!fp) {
        return -1;
    }

    if (fscanf(fp, "%*s %f %f %f %*s %*s", &blocks, &used, &available) != 3) {
        pclose(fp);
        return -1;
    }

    pclose(fp);

    if (blocks <= 0.0f || used < 0.0f || available < 0.0f) {
        return -1;
    }

    storage->total = blocks / 1024.0f;
    storage->used = used / 1024.0f;
    storage->percent = (storage->used / storage->total) * 100.0f;
    return 0;
}

static SystemStatus evaluate_status(float memory_percent, float storage_percent, float threshold, float hysteresis) {
    float max_usage = (memory_percent > storage_percent) ? memory_percent : storage_percent;
    float warning_on = threshold;
    float warning_off = threshold - hysteresis;

    if (warning_off < 0.0f) {
        warning_off = 0.0f;
    }

    if (max_usage >= 95.0f) {
        return STATUS_CRITICAL;
    }
    if (max_usage >= warning_on) {
        return STATUS_WARNING;
    }
    if (max_usage <= warning_off) {
        return STATUS_HEALTHY;
    }

    return STATUS_WARNING;
}

static const char *status_name(SystemStatus status) {
    if (status == STATUS_HEALTHY) {
        return "HEALTHY";
    }
    if (status == STATUS_WARNING) {
        return "WARNING";
    }
    return "CRITICAL";
}

static void led_levels(SystemStatus status, int *green_on, int *yellow_on, int *red_on) {
    *green_on = (status == STATUS_HEALTHY);
    *yellow_on = (status == STATUS_WARNING);
    *red_on = (status == STATUS_CRITICAL);
}

static void apply_led_status(int green_on, int yellow_on, int red_on) {
#if HAS_PIGPIO
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio init failed; LED output skipped.\n");
        return;
    }

    gpioSetMode(GREEN_LED_PIN, PI_OUTPUT);
    gpioSetMode(YELLOW_LED_PIN, PI_OUTPUT);
    gpioSetMode(RED_LED_PIN, PI_OUTPUT);

    gpioWrite(GREEN_LED_PIN, green_on);
    gpioWrite(YELLOW_LED_PIN, yellow_on);
    gpioWrite(RED_LED_PIN, red_on);

    usleep(100000);
    gpioTerminate();
#else
    (void)green_on;
    (void)yellow_on;
    (void)red_on;
    fprintf(stderr, "pigpio header not found; LED output skipped.\n");
#endif
}

static void print_led_report(int green_on, int yellow_on, int red_on) {
    printf("LED GREEN : %s\n", green_on ? "ON" : "OFF");
    printf("LED YELLOW: %s\n", yellow_on ? "ON" : "OFF");
    printf("LED RED   : %s\n", red_on ? "ON" : "OFF");
}

static int parse_args(int argc, char **argv, MonitorConfig *config) {
    config->threshold = 80.0f;
    config->hysteresis = 5.0f;
    config->override_values = 0;
    config->memory_percent_override = 0.0f;
    config->storage_percent_override = 0.0f;

    if (argc > 1) {
        if (sscanf(argv[1], "%f", &config->threshold) != 1 || config->threshold <= 0.0f || config->threshold > 100.0f) {
            return -1;
        }
    }

    if (argc > 2) {
        if (argc < 4) {
            return -1;
        }
        if (sscanf(argv[2], "%f", &config->memory_percent_override) != 1) {
            return -1;
        }
        if (sscanf(argv[3], "%f", &config->storage_percent_override) != 1) {
            return -1;
        }
        config->override_values = 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    UsageInfo memory = {0};
    UsageInfo storage = {0};
    MonitorConfig config;
    SystemStatus status;
    int green_on = 0;
    int yellow_on = 0;
    int red_on = 0;

    if (parse_args(argc, argv, &config) != 0) {
        fprintf(stderr, "Usage: %s [threshold_percent] [memory_percent storage_percent]\n", argv[0]);
        return 1;
    }

    if (config.override_values) {
        memory.percent = config.memory_percent_override;
        storage.percent = config.storage_percent_override;
    } else {
        if (read_memory_usage(&memory) != 0) {
            fprintf(stderr, "Failed to read memory usage.\n");
            return 1;
        }
        if (read_storage_usage(&storage) != 0) {
            fprintf(stderr, "Failed to read storage usage.\n");
            return 1;
        }
    }

    status = evaluate_status(memory.percent, storage.percent, config.threshold, config.hysteresis);
    led_levels(status, &green_on, &yellow_on, &red_on);

    printf("Raspberry Pi Health Monitor\n");
    printf("Memory Usage Percentage : %.2f%%\n", memory.percent);
    printf("Storage Usage Percentage: %.2f%%\n", storage.percent);
    printf("Threshold: %.2f%% (hysteresis %.2f%%)\n", config.threshold, config.hysteresis);
    printf("System Status: %s\n", status_name(status));

    print_led_report(green_on, yellow_on, red_on);
    apply_led_status(green_on, yellow_on, red_on);

    return 0;
}
