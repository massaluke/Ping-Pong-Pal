# Ping-Pong-Pal
Remote Controlled Table Tennis Server https://youtu.be/qHtv6UpS5UU

<img width="961" height="681" alt="image" src="https://github.com/user-attachments/assets/6d703083-7241-45d0-9dc1-b690c4cd1827" />
<img width="251" height="367" alt="image" src="https://github.com/user-attachments/assets/974670a7-6986-4548-b0aa-cd8cb5167c03" />
<img width="939" height="501" alt="image" src="https://github.com/user-attachments/assets/3320ff17-0fef-4ad8-bc14-450b07357b0b" />

Main Hub
The main hub power subsystem includes the battery which is connected to a power unit which then connects to all power
consuming components. The user interface subsystem includes a power switch and an RGB LED strip which receives
colour setting data from the microcontroller and power from the power unit. The launch subsystem includes the four
launching motors and motor drivers which receive data input from the microcontroller and power from the power unit.
The position subsystem takes setting data from the microcontroller to a motor driver, which receives power from the
power unit. The wireless communication module will enable the Main Hub microcontroller to communicate to
peripheral devices easily.
<img width="906" height="684" alt="image" src="https://github.com/user-attachments/assets/db713cff-b79a-4f11-bd9c-f6fc5eba9d8c" />

Target
The detection subsystem provides analogue input to the target microcontroller after being passed through a signal
conditioning circuit. The microcontroller data is transmitted through wireless communication to the main hub
microcontroller. The microcontroller receives power from the power unit located within the power supply. The green
LED located on the target takes on/off commands from the microcontroller pin voltage which is driven when set high.
The power switch inputs the circuit the on/off state of operation.
<img width="945" height="491" alt="image" src="https://github.com/user-attachments/assets/edb08c6e-a123-4e2f-b6d3-be2b19887d79" />

Remote
The Remote microcontroller and LCD display receive power input from the power unit. The LCD display takes user
inputs and outputs data to the microcontroller on the Remote PCB. The power switch inputs the circuit the on/off state
of operation. The microcontroller on the Remote communicates to the Main Hub microcontroller through wireless
communication protocols.
<img width="925" height="499" alt="image" src="https://github.com/user-attachments/assets/1f7f3030-ba64-4c36-9f97-1c4cad721f7b" />

User Application
The user application is powered from the user’s mobile phone battery. The mobile application
receives and sends data to the Main Hub microcontroller through the Bluetooth capabilities of the mobile device.
