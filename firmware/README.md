# Build system
Uses CMake, Ninja and VSCode to build STM32CubeMX projects.

## Installation 
Setup build system for STM32CubeMX projects with CMake and VSCode
https://github.com/MaJerle/stm32-cube-cmake-vscode?tab=readme-ov-file

## Programming
There are two main ways to program the STM32.
1. Using a ST-Link programmer such as the ST-Link V2 and the VS code task Flash.
2. Using the STM32's built in bootloader and the VS code task Flash Bootloader.

Note: When using the built in bootloader the BOOT0 line has to be pulled high. This can be done by connecting the BOOT0 pin to the 3.3V pin via a jumper wire.

## Debugging
Debugging is done using the ST-Link V2 programmer and the vs code debug.
