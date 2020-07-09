# CAN_WIFI_MODULE
CAN and Wi-Fi Interface Utilizing Custom Drivers Written for the STM32F031K6 and ESP8266 MCU and Custom Server Application

## Project Rational
In order to gain some experience with wireless networks utilizing Wi-Fi, I decided to
expand upon the previous work I had done with the CAN interface and create a server to MCU
interface. 

Previous CAN project: [MCP2515_CAN_DRIVER](https://github.com/matt001k/MCP2515_CAN_DRIVER)

In this project, messages are sent over the CAN bus from the STM32 MCU to the
ESP8266 MCU, which then sends this data to a server and displays the messages onto the server.
This will be useful for the final project as it will allow data to be exchanged to and from a server
on a single board computer, to the quadra-pod robot, which will be the final goal/project that this is attributing to(called HXM8).

## Hardware Required
The devices used are the two MCUs mentioned earlier, along with two MCP2515 CAN
Controllers, two MCP2562 CAN Transceivers along with a Raspberry Pi (SBC) for receiving
and transferring of data. Both MCUs communicate to the MCP2515s utilizing Serial Peripheral
Interface (SPI). The circuit is shown in the figure below utilizing the CAN Bus interface and two MCUs.

<p align="center">
  <img width="" height="" src="https://github.com/matt001k/CAN_WIFI_MODULE/blob/master/photos/Circuit.jpg">
</p>

## Message Structure
A certain structure for the logging and data exchange was created in order to provide for
better troubleshooting of the system. The structure is as follows:

Message Type | Time(only on server) | Device No. | Message
------------ | ------------- | ------------ | -------------
I = INFORMATION, W = WARNING, E = ERROR, CE = CRITICAL ERROR| Current Date and Time | Device that the Message is Coming From | LATIN-1 Encoded Message

The Message Type and Device No. are determined by the standard identifier(11 bit) of the CAN
message in the following structure:
<p align="center">
  <img width="" height="" src="https://github.com/matt001k/CAN_WIFI_MODULE/blob/master/photos/IDENTIFIER%20LAYOUT.PNG"
</p>

The time format is as follows: * (YEARMONTHDAY | HOURMINUTE:SECOND)*

Time format is only used in the server, whereas all other fields are used in exchanges from
controller to controller. This message structure allows for up to 512 devices to be used, but the
CAN Bus network is limited to 110 nodes, so there is a lot of room for more devices on the
network incase other modules will be added

## System Operation
Both MCUs are using **FreeRTOS** in order to run a scheduler of tasks. The STM32 will have two tasks when completed, it will run the quadra-pods movement as well at handle communication through the CAN Bus between the ESP8266. The ESP8266 runs three tasks, one task to handle received messages from the server, one task to send messages to the server and the last task will communicate over the CAN Bus. The ESP8266 client speaks to the server through a TCP socket. In order to run some test code, every ten seconds a message is encoded in ISO/IEC 8859-1 encoding (LATIN-1) on the STM32 MCU and is then sent over the CAN Bus to the ESP8266. The message being sent for Device Number Message Type testing purposes is “SYSSTART”. Once received by the ESP8266, this fires an interrupt on the RX0_INT line on the ESP8266 to let the MCU know there is a message in the buffer of the MCP2515. This message is then read by the ESP8266 through the SPI interface to the controller and is then sent to the server over the opened TCP socket.

In order to create the server, simple Python code was used to create a command line
interface. The program displays the messages from the ESP8266 client on the terminal and then
logs the data into a text file. Currently, the Python program is just used to receive messages over
the TCP connection, but the ESP8266 microcontroller can receive and send messages. This
makes expanding the system easier in the future for when an interface is needed for the server to
send commands to the quadra-pod. 

## Testing
The following figures display  the results on the SBC’s console output, results
from the written log file (in .txt form) of the system and the ESP8266’s debugging console. The
data is sent every ten seconds and there is minimal delay through the CAN Bus and through WiFi. The ESP8266’s output messages show the data received from the STM32 MCU along with
messages received from the server, in this case the message sent from the server is just
RECEIVED. The data from the CAN message is sent over 32 bit increments and therefor two
RECEIVED messages are sent very quickly by the server.

<p align="center">
  <img width="" height="" src="https://github.com/matt001k/CAN_WIFI_MODULE/blob/master/photos/Server%20Terminal%20Display..PNG"
</p>
  <p align="center">
  <img width="" height="" src="https://github.com/matt001k/CAN_WIFI_MODULE/blob/master/photos/txt%20Display.PNG"
</p>
    <p align="center">
  <img width="" height="" src="https://github.com/matt001k/CAN_WIFI_MODULE/blob/master/photos/ESP8266%20Debug%20Terminal.PNG"
</p>
