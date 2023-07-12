# TerminalFlir.Mlx90640

## What?

This tool allows to create a low-res flir camera using just Raspberry Pi
and MLX90640 sensor.
(other single board combuters (SBC) must be easy to adapt too, but I have only tested it on RPi3.

Here is the wiring diagram. Basically MLX90640 needs to be attached to 
SBC using I2C bus pins, 3.3V pin and ground.

![wiring diagram](https://raw.githubusercontent.com/AlexanderSavochkin/TerminalFlir.Mlx90640/master/doc/connection-911.png) 

The output image is printed into terminal, so the RPi + Mlx90640 setup
can be used in both desktop (with keyboard and monitor attached to SBC) and headles mode (with ssh connection over network).

Mlx90640 sensor must be connected to I2C pins of SBC.

## How to run

Example:

The program requires permissions to access `/dev/i2c-1`. So you need either elevated permission
```
sudo ./mlxflir  --palette whot --low-temperature 20 --high-temperature 38
```
or (preferred) to add the user to `i2c` group. Default value for `--low-temperature` is 19.0, `--high-temperature` is 42.0 and 
`--palette` is "iron" (colored),
so
```
./mlxflir
```
is equivalent
```
./mlxflir --palette iron --low-temperature 19 --high-temperature 42
```

## Examples

The screenshots below were taken on a "headless" setup: Windows machine with ssh connection to Raspberry Pi 3 
connected to Mlx90640 sensors.

A man with a hot beverage mug: "iron" palette.
![man with mug, iron](https://raw.githubusercontent.com/AlexanderSavochkin/TerminalFlir.Mlx90640/master/doc/man-with-hot-beverage-mug-iron.png)

A man with a hot beverage mug: "white-hot" palette.

```
./mlxflir --palette whot 
```
![man with mug, whitehot](https://raw.githubusercontent.com/AlexanderSavochkin/TerminalFlir.Mlx90640/master/doc/man-with-hot-beverage-mug-whitehot.png)


