# Radar Visualizer

Pygame visualizer for the ultrasonic radar serial stream.

## Install

```sh
python -m pip install -r tools/requirements.txt
```

## Run

```sh
python tools/radar_visualizer.py --list-ports
python tools/radar_visualizer.py /dev/ttyACM0
```

The default baud rate is `57600`, matching the firmware.

When the visualizer opens the serial port, it waits briefly for the MCU to
finish any serial-reset boot sequence, then sends `y` to enable USART rendering.

Accepted calibration lines:

```text
$mind:20
$maxd:200
$mina:0
$maxa:180
```

Receiving a new calibration/configuration block clears the current radar image
before applying the new limits.

Accepted scan sample lines:

```text
A90,D120,I24
$A90,D120,I24
```
