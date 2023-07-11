# TerminalFlir.Mlx90640

## What?

This tool allows to create a low-res flir camera using just Raspberry Pi 
(other SBC must be easy to adapt too, but I have only tested it on RPi3.

The output image is printed into terminal, so the RPi + Mlx90640 setup
can be used in headles mode via ssh connection.

Mlx90640 sensor must be connected to I2C pins of SBC.

## Examples

The screenshots below were taken on a "headless" setup: Windows machine with ssh connection to Raspberry Pi 3 
connected to Mlx90640 sensors.

A man with a hot beverage mug: "iron" palette.
![screenshot](https://raw.githubusercontent.com/AlexanderSavochkin/TerminalFlir.Mlx90640/master/doc/man-with-hot-beverage-mug-iron.png)

A man with a hot beverage mug: "white-hot" palette.
![screenshot](https://raw.githubusercontent.com/AlexanderSavochkin/TerminalFlir.Mlx90640/master/doc/man-with-hot-beverage-mug-whitehot.png)


