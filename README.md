
# Sequencer

TO DO list:
- podpiąć Step limiter
- zrobić note
- usunac zegar
- ustawić do porządku rotary encoder

### Board: DOIT ESP32 DEVKIT V1

![missing image](https://raw.githubusercontent.com/reverbrick/algo_seq/main/docs/espPinout.png)

### Used Pins

Display:
- SCL: 26
- SDA: 25

Rotary Encoder:
- CLK: 34
- DT: 35
- SW: 32

Buttons: (each for channel)
- BUTTON1: 15
- BUTTON2: 13
- BUTTON3: 4
- BUTTON4: 5

LEDS: (each for channel)
- LED1: 18
- LED2: 19
- LED3: 21
- LED4: 22

External Clock IN:
- CLOCK 23

### DISPLAY MONITOR

Run display monitor: VSC -> Miscellaneous -> PlatformIO Core CLI -> paste below line

```bash
pio device monitor --baud 115200 --port /dev/cu.<dev name>
```

### Instaling external libraries

```bash
pio lib -g install  <path>
```
