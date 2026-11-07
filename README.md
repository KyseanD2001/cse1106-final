# cse1106-final
Final project for Introduction to Computer Engineering.

## Raspberry Pi Health Monitor (Simplified)

`health_monitor.c` is intentionally simplified and uses only:
- `stdio.h`
- `unistd.h`
- `pigpio.h` (when available)

It reports:
- Memory usage percentage
- Storage usage percentage
- System status (`HEALTHY`, `WARNING`, `CRITICAL`)
- LED states (`ON/OFF`) for Green, Yellow, Red

### Build

With pigpio installed:

```bash
gcc -Wall -Wextra -std=c11 health_monitor.c -o health_monitor -lpigpio -lrt -lpthread
```

Without pigpio headers, it still compiles and prints status/percentages, but LED hardware output is skipped:

```bash
gcc -Wall -Wextra -std=c11 health_monitor.c -o health_monitor
```

### Run

```bash
./health_monitor
```

Optional:
- `./health_monitor <threshold_percent>`
- `./health_monitor <threshold_percent> <memory_percent> <storage_percent>`
