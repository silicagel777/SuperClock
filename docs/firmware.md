# Software notes

The firmware is written in C++. It can be compiled by [avr-gcc](avr-gcc) and flashed by [avrdude](https://github.com/avrdudes/avrdude) through [USBasp](https://www.fischl.de/usbasp/). You can find latest firmware builds in "Releases" section.

## Flashing firmware

ATmega32 doesn't come with a USB bootloader (and there are no free I/O pins for USB connection anyway), so you are going to need an external programmer tool.

There are lots of ATmega programmers around, and if you are into DIY electronics, chances are high that you already have one! I've used the [USBasp](https://www.fischl.de/usbasp/) programmer from AliExpress. If you have an Arduino board, you can try [Arduino ISP](https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/) sketch
instead.

To flash firmware, do the following:

- Connect your programmer to your PC and your clock. Most USBasp clones come with a 2x5 ribbon of standard [AVR ISP pinout](https://en.m.wikipedia.org/wiki/File:Isp_headers.svg): if that's the case, just plug the cable into the IDC socket. USB power is not required, as the clock will receive power from the programmer.
- Install drivers for your programmer. For USBasp, you can consult [the official page](https://www.fischl.de/usbasp/).
- Download the programming software. My favorite is [SinaProg](https://cxem.net/software/download/SinaProg2.1.1.rar), but sadly it's very dated, and the official website is long gone. Another good (and maintained) option is [AVRDUDESS](https://github.com/ZakKemble/AVRDUDESS). Both tools are Windows-only and are GUI wrappers for the [avrdude](https://github.com/avrdudes/avrdude) tool. If you run Linux, you can use avrdude directly instead and flash firmware by running a Makefile (see below).
- The pristine ATmega32 is configured to run at 1MHz. Before flashing the firmware, we need to tell it to run from the external crystal at 16Mhz and configure a few other settings. This is done by setting fuse bits. You need to set **low byte** to `0xBF` and **high byte** to `0xD1`. Be careful, setting wrong fuse values can brick your chip!
    - For fuses, you need to set a low programming speed in your software, as the default 1MHz clock is not enough for ATmega32 to react on fastest speeds.
    - Some USBasp clones from AliExpress run a very old firmware version that doesn't allow configuring programming speed in software: they always use the fastest one instead (1.5MHz). If this happens to you, short the "JP3" jumper on the programmer, it will reduce programming speed.
    - Once the fuses are flashed, you can return to full programming speed.
    - If the chip fails to detect after fuses are flashed, carefully check the soldering around crystal: ATmega32 won't run if it can't make it oscillate.
- Download the latest `*.hex` files from the "Releases" section and load it into the programming software. Flash it, and you should see your clock come alive!

Here is a picture with AVRDUDESS buttons highlighted:

![AVRDUDESS UI](img/avdudess.png)

## Developing firmware

### Installing development tools on Windows

- Grab an `avr-gcc` build from [WinAVR](https://winavr.sourceforge.net/)
- Download `make` from [xPack](https://github.com/xpack-dev-tools/windows-build-tools-xpack)
- Download `avrdude` [binary](https://github.com/avrdudes/avrdude)
- Add all the tools to your PATH environment variable (Control Panel -> System -> Advanced System Settings -> Environment Variables). For `avr-gcc` and `make` add `bin` directories, and for `avrdude` add the directory with `avrdude.exe`.

### Installing development tools on Ubuntu or Debian

Run as root: `apt update && apt install gcc-avr binutils-avr avr-libc make avrdude`

### Using `make`

The project comes with a `Makefile` that defines a few commands. Switch to project directory and try the following:

- `make` to build a hex file (you can append `-j<NUMBER>` for parallel builds, for example `make -j16` for a 8-core 16-thread CPU)
- `make font` to update font file
- `make fuse` write fuse bits through USBasp (do it once before flashing)
- `make flash` flash firmware through USBasp
- `make clean` to remove build files

### Using Visual Studio Code to develop

With all the build tools in place, setting up the [code editor](https://code.visualstudio.com/) is fairly straightforward!

Firstly, install the [C/C++ plugin](https://code.visualstudio.com/docs/languages/cpp). You'll probably see an annoying pop-up once you open a `*.cpp` file, so this requirement is hard to miss anyway.

Once you are done, create `.vscode/c_cpp_properties.json` in project root to configure the C/C++ plugin. Here is mine:

```json
{
    "configurations": [
        {
            "name": "AVR",
            "includePath": [
                "${workspaceFolder}/src/"
            ],
            "windowsSdkVersion": "10.0.19041.0",
            // adjust your compiler path accordingly
            "compilerPath": "C:/MCU/gcc-avr/bin/avr-g++",
            "compilerArgs": [
                "-std=c++17",
                "-O2",
                "-DF_CPU=16000000UL",
                "-mmcu=atmega32"
            ],
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-arm"
        }
    ],
    "version": 4
}
```

Create `.vscode/tasks.json` in project root to set up build tasks. You can also install the [quick-run-panel](https://marketplace.visualstudio.com/items/?itemName=davehart.quick-run-panel) plugin to have all the build tasks in your arm's reach. This is optional, as you can just run `make` commands from your command line. Here is mine:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            // change the following to "make -j $(getconf _NPROCESSORS_ONLN)" on Linux
            "command": "make -j $ENV:NUMBER_OF_PROCESSORS",
            "group": "build",
            "problemMatcher": [],
            "presentation": {
                "focus": true
            }
        },
        {
            "label": "run",
            "type": "shell",
            "command": "make flash",
            "problemMatcher": [],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "focus": true
            }
        },
        {
            "label": "clean",
            "type": "shell",
            "command": "make clean",
            "group": "build",
            "problemMatcher": [],
            "presentation": {
                "focus": true
            }
        },
        {
            "label": "font",
            "type": "shell",
            "command": "make font",
            "group": "build",
            "problemMatcher": [],
            "presentation": {
                "focus": true
            }
        }
    ]
}
```