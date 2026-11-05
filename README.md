# cse1106-final
Final project for Introduction to Computer Engineering.

## Raspberry Pi Equipment Health Monitor (C)

This repository now includes `health_monitor.c`, a C program for Raspberry Pi 4B that:
- obtains memory usage from `/proc/meminfo` and storage usage from `/`
- can accept manual usage values through CLI arguments
- calculates utilization percentages for memory and storage
- compares utilization against a configurable warning threshold
- prints `HEALTHY` or `WARNING` status
- saves diagnostics to a file
- controls 3 GPIO LEDs for status indication (green/yellow/red)

### Build

```bash
gcc -Wall -Wextra -std=c11 health_monitor.c -o health_monitor
```

### Run

```bash
./health_monitor
```

Useful options:
- `--threshold <percent>` (default: `80`)
- `--diag-file <path>` (default: `diagnostic_results.txt`)
- `--mem-used-mb <n> --mem-total-mb <n>`
- `--storage-used-mb <n> --storage-total-mb <n>`
- `--no-led` (skip GPIO LED writes)

Example with custom threshold and manual values:

```bash
./health_monitor --threshold 70 --mem-used-mb 3000 --mem-total-mb 4000 --storage-used-mb 20000 --storage-total-mb 30000 --no-led
```

GPIO pins used:
- Green: BCM 17
- Yellow: BCM 27
- Red: BCM 22
