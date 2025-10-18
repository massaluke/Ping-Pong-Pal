# Ping-Pong-Pal
Remote Controlled Table Tennis Server  
[Demo Video](https://youtu.be/qHtv6UpS5UU)

<img width="961" height="681" alt="PingPongPal Main Image" src="https://github.com/user-attachments/assets/6d703083-7241-45d0-9dc1-b690c4cd1827" />
<img width="939" height="501" alt="PingPongPal Setup" src="https://github.com/user-attachments/assets/3320ff17-0fef-4ad8-bc14-450b07357b0b" />

---

## Main Hub
The **Main Hub** consists of several subsystems:  

- **Power Subsystem:** The battery connects to a power unit which supplies all power-consuming components.  
- **User Interface Subsystem:** Includes a power switch and an RGB LED strip that receives colour data from the microcontroller.  
- **Launch Subsystem:** Four launching motors and motor drivers receive control input from the microcontroller and power from the power unit.  
- **Position Subsystem:** Receives setting data from the microcontroller to a motor driver, powered by the main unit.  
- **Wireless Communication Module:** Enables communication between the Main Hub microcontroller and peripheral devices.

<img width="906" height="684" alt="Main Hub Diagram" src="https://github.com/user-attachments/assets/db713cff-b79a-4f11-bd9c-f6fc5eba9d8c" />

---

## Target
The **Target** includes:  

- **Detection Subsystem:** Provides analogue input to the target microcontroller via a signal conditioning circuit.  
- **Wireless Communication:** Sends hit data to the Main Hub microcontroller.  
- **LED Indicator:** Green LED controlled by the microcontroller to indicate activity.  
- **Power Switch:** Controls the on/off state of the target.

<img width="945" height="491" alt="Target Diagram" src="https://github.com/user-attachments/assets/edb08c6e-a123-4e2f-b6d3-be2b19887d79" />

---

## Remote
The **Remote** subsystem includes:  

- **Microcontroller & LCD Display:** Receives power from the remote power unit. LCD takes user input and sends it to the microcontroller.  
- **Power Switch:** Controls the on/off state.  
- **Wireless Communication:** Microcontroller communicates with the Main Hub via wireless protocols.

<img width="925" height="499" alt="Remote Diagram" src="https://github.com/user-attachments/assets/1f7f3030-ba64-4c36-9f97-1c4cad721f7b" />

---

## User Application
The **User Application** runs on the mobile device:  

- Powered by the phone battery.  
- Sends and receives data to/from the Main Hub microcontroller using Bluetooth.  
