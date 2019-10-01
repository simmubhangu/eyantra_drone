[![Build Status](https://travis-ci.org/simmubhangu/eyantra_drone.svg?branch=master)](https://travis-ci.org/simmubhangu/eyantra_drone)
# eyantra_drone
Metapackage to control the eyantra_drone via service and topics 
<http://wiki.ros.org/eyantra_drone>

# HacktoberFest e-Yantra

---
![e-Yantra](logo.png "e-Yantra")
---

This repository is for controlling the Pluto Drone. You will need the Pluto Drone in order to work with this repository.

## How to contribute to this project
Fork this repository and get started. Currently, documentation and comments are missing within the code. You can create a pull request by adding documentation

## Rules
1. Use only the Pluto Drone for this project.
2. You must add comments in your code. File level, function level and line level comments are expected wherever necessary

## Winner
First PR that follows the given rules and completes the given task, is declared the winner at e-Yantra's discretion


## Getting Started 
Use following instructions on how to use this package:

###### Prerequisites
```
# libavcodec and libsdl2-dev for camera
sudo apt install libavcodec-dev libsdl2-dev 

```

###### Installation

```
# Navigate to catkin workspace
cd <WORKSPACE_NAME>

# clone repository
git clone https://github.com/simmubhangu/eyantra_drone.git

# Build the Package
catkin_make 

```

###### Run Package

```
# launch file to communication with  edrone

roslaunch edrone_server drone_comb.launch

# Follow the instruction on terminal to control the drone
```
## Using ROSTopic:
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1000, rcAUX1: 0, rcAUX2: 0, rcAUX3: 0, rcAUX4: 1500}" // example of arming drone 

```
## ROSTopic To Control edrone

###### Arm
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1000, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Disarm 
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1000}"
```
###### Increase Roll value to move forward with respect to x-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1600, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Decrease Roll value to move backward with respect to x-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1400, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Increase Pitch value to move forward/left with respect to y-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1600, rcYaw: 1500, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Decrease Pitch value to move backward/right with respect to y-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1400, rcYaw: 1500, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Increase Throttle value to move up with respect to z-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1800, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Decrease Throttle value to move down with respect to z-axis
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1500, rcThrottle: 1200, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Increase Yaw value to rotate in clockwise direction
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1800, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
###### Decrease Yaw value to rotate in anti-clockwise direction
```
rostopic pub /drone_command edrone_client/edrone_msgs "{rcRoll: 1500, rcPitch: 1500, rcYaw: 1200, rcThrottle: 1500, rcAUX1: 1500, rcAUX2: 1500, rcAUX3: 1500, rcAUX4: 1500}"
```
**Note: RC values ranges from 1000 to 2000**

## Multiple Drones

Following is the procedure to control multiple drones within the same network:

###### Setting the drone in client mode: Connect to drone wifi and use following command to open telnet connection: 
```
telnet 192.168.4.1 // drone wifi ip
```
###### Set the drone in both Station(STA) and Access Point Mode(AP) : Use following command:
```
+++AT MODE 3
```
###### Set the ssid and password: Use following command:
```
+++AT STA ssid password
```
###### Add IPs: Start your hotspot and your drone should connect to the hotspot. Note the IP address assigned to it. Edit following lines in [DroneSwarm.cpp](/edrone_client/src/DroneSwarm.cpp). Repeat this for all new drones which are added to the network. 
```
all_ips.push_back(&quot;192.168.43.151&quot;);
all_ips.push_back(&quot;&quot;);
```
###### Send data: Follow procedure in ROSTopic Header to give commands to fly the drones. Add droneIndex in edrone_msgs for every topic. This index is the same as the index of the IP within 'all_ips' when you add it. 

###### TODO - Get drone data from multiple drones
Cómo contribuir a este proyecto

Bifurca este repositorio y comienza. Actualmente, faltan documentación y comentarios dentro del código. Puede crear una solicitud de extracción agregando documentación
Reglas

    Use solo el Plutón Drone para este proyecto.
    Debe agregar comentarios en su código. Se esperan comentarios de nivel de archivo, nivel de función y nivel de línea siempre que sea necesario

Ganador

El primer RP que sigue las reglas dadas y completa la tarea dada, se declara ganador a discreción de e-Yantra
Empezando

Utilice las siguientes instrucciones sobre cómo usar este paquete:
Prerrequisitos

# libavcodec y libsdl2-dev para cámara
sudo apt install libavcodec-dev libsdl2-dev

Instalación

# Navegar al espacio de trabajo de catkin
cd <WORKSPACE_NAME>

# repositorio de clones
git clone https://github.com/simmubhangu/eyantra_drone.git

# Construye el paquete
catkin_make




