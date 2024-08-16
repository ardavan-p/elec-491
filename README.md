# ECE Capstone Project TL81 - Wirelessly Powered Tire Pressure Monitoring System

## Introduction
The following repository contains all design components for UBC ECE capstone project TL81 - Wirelessly Powered Tire Pressure Monitoring System

## Design Components
The following directories contain primary design components. 

### Graphical User Interface for TPMS
The source code for the GUI which was designed for demonstration purposes is located in the `gui/` folder. It is design in the Python.

### ECU Source Code
The firmware for the master ECU used in the demonstration is located in the `ecu/` folder. It is all written in C, for the STM32F103RCT6 microcontroller.

### Power Transmitter Node
All design files for the power transmitter node is located in the `ptn/` and `enclosures/ptn/` directories. <br>

The PCB design files for the power transmitter node are located in `ptn/pcb/`. The PCB is designed using the KiCad PCB design software. <br>

The firmware source code for the power transmitter node is located in `ptn/fw/`. It is all written in C, for the STM32F103C8T6 microcontroller. <br>

The physical enclosure for the power transmitter node is located in `enclosures/ptn/`. The PTN enclosure was designed using SolidWorks.

### Sensor Node
All design files for the sensor node is located in the `sn/` directory. <br>

The PCB design files for the sensor node are located in `sn/rev-2/pcb/sensor-node-v2/`. The PCB is designed using the KiCad PCB design software. <br>

The firmware source code for the final revision of the sensor node is located in `sn/rev-2/fw/sensor-node-v2/`. It is all written in C, for the STM32U031K8U6 microcontroller. <br>

## Capstone Video
All assets for the video created for this project is located in the `video/` folder.
