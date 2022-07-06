
# Sequencer

TO DO list:
- podpiąć Step limiter
- zrobić note
- usunac zegar
- ustawić do porządku rotary encoder

### DISPLAY MONITOR

Run display monitor: VSC -> Miscellaneous -> PlatformIO Core CLI -> paste below line

```bash
pio device monitor --baud 115200 --port /dev/cu.<dev name>
```

### Instaling external libraries

```bash
pio lib -g install  <path>
```
