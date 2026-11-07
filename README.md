# cse1106-final
Final project for Introduction to Computer Engineering.

## Raspberry Pi Health Monitor (Simplified)

`health_monitor.c` now focuses on a smaller implementation using `stdio`, `unistd`, and `pigpio`.

It reports:
- Memory usage percentage
- Storage usage percentage
- System status (`HEALTHY`, `WARNING`, `CRITICAL`)
- LED status with explicit `ON/OFF` for Green/Yellow/Red

### Build

If `pigpio` is installed:

```bash
gcc -Wall -Wextra -std=c11 health_monitor.c -o health_monitor -lpigpio -lrt -lpthread
```

If `pigpio` is not installed, it still compiles and prints metrics, but skips LED output:

```bash
gcc -Wall -Wextra -std=c11 health_monitor.c -o health_monitor
```

### Run

```bash
./health_monitor
```

Optional arguments:
- `./health_monitor <threshold_percent>`
- `./health_monitor <threshold_percent> <memory_percent> <storage_percent>`

Example:

```bash
./health_monitor 75 82 61
```
