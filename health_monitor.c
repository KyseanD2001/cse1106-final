#include <stdio.h>
#include <unistd.h>

#if __has_include(<pigpio.h>)
#include <pigpio.h>
#define HAS_PIGPIO 1
#else
#define HAS_PIGPIO 0
#endif

extern FILE *popen(const char *command, const char *type);
extern int pclose(FILE *stream);

#define GREEN_LED_PIN 17
#define YELLOW_LED_PIN 27
#define RED_LED_PIN 22

typedef enum {
    HEALTHY,
    WARNING,
    CRITICAL
} SystemStatus;

typedef struct {
    float threshold;
    float hysteresis;
} MonitorConfig;

static int read_memory_percent(float *memory_percent) {
    FILE *fp = fopen("/proc/meminfo", "r");
    char line[128];
    float total_kb = 0.0f;
    float available_kb = 0.0f;

    if (!fp) {
        return -1;
    }

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "MemTotal: %f kB", &total_kb);
        sscanf(line, "MemAvailable: %f kB", &available_kb);
        if (total_kb > 0.0f && available_kb > 0.0f) {
            break;
        }
    }

    fclose(fp);

    if (total_kb <= 0.0f || available_kb < 0.0f || available_kb > total_kb) {
        return -1;
    }

    *memory_percent = ((total_kb - available_kb) / total_kb) * 100.0f;
    return 0;
}

static int read_storage_percent(float *storage_percent) {
    FILE *fp = popen("df -P / | tail -1", "r");
    float blocks = 0.0f;
    float used = 0.0f;

    if (!fp) {
        return -1;
    }

    if (fscanf(fp, "%*s %f %f %*f %*s %*s", &blocks, &used) != 2) {
        pclose(fp);
        return -1;
    }

    pclose(fp);

    if (blocks <= 0.0f || used < 0.0f) {
        return -1;
    }

    *storage_percent = (used / blocks) * 100.0f;
    return 0;
}

static SystemStatus evaluate_status(float memory_percent, float storage_percent, const MonitorConfig *cfg) {
    float max_percent = (memory_percent > storage_percent) ? memory_percent : storage_percent;

    if (max_percent >= 95.0f) {
        return CRITICAL;
    }
    if (max_percent >= cfg->threshold) {
        return WARNING;
    }
    if (max_percent <= (cfg->threshold - cfg->hysteresis)) {
        return HEALTHY;
    }

    return WARNING;
}

static const char *status_text(SystemStatus status) {
    if (status == HEALTHY) return "HEALTHY";
    if (status == WARNING) return "WARNING";
    return "CRITICAL";
}

static void set_leds(SystemStatus status, int *green_on, int *yellow_on, int *red_on) {
    *green_on = (status == HEALTHY);
    *yellow_on = (status == WARNING);
    *red_on = (status == CRITICAL);
}

static void apply_led_output(int green_on, int yellow_on, int red_on) {
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

int main(int argc, char **argv) {
    MonitorConfig cfg = {80.0f, 5.0f};
    float memory_percent = 0.0f;
    float storage_percent = 0.0f;
    int green_on = 0;
    int yellow_on = 0;
    int red_on = 0;

    if (argc > 1) {
        if (sscanf(argv[1], "%f", &cfg.threshold) != 1 || cfg.threshold <= 0.0f || cfg.threshold > 100.0f) {
            fprintf(stderr, "Usage: %s [threshold] [memory_percent storage_percent]\n", argv[0]);
            return 1;
        }
    }

    if (argc >= 4) {
        if (sscanf(argv[2], "%f", &memory_percent) != 1 || sscanf(argv[3], "%f", &storage_percent) != 1) {
            fprintf(stderr, "Invalid override percentages.\n");
            return 1;
        }
    } else {
        if (read_memory_percent(&memory_percent) != 0 || read_storage_percent(&storage_percent) != 0) {
            fprintf(stderr, "Failed to read system usage data.\n");
            return 1;
        }
    }

    SystemStatus status = evaluate_status(memory_percent, storage_percent, &cfg);
    set_leds(status, &green_on, &yellow_on, &red_on);

    printf("Raspberry Pi Health Monitor\n");
    printf("Memory Usage Percentage : %.2f%%\n", memory_percent);
    printf("Storage Usage Percentage: %.2f%%\n", storage_percent);
    printf("System Status: %s\n", status_text(status));
    printf("LED GREEN : %s\n", green_on ? "ON" : "OFF");
    printf("LED YELLOW: %s\n", yellow_on ? "ON" : "OFF");
    printf("LED RED   : %s\n", red_on ? "ON" : "OFF");

    apply_led_output(green_on, yellow_on, red_on);
    return 0;
}
