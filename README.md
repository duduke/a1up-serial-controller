## Arcade1Up 2-Player Contoller Serial Interface

Arcade1Up controllers use a serial interface to communicate with the Arcade1Up mainboard.
User [QuasariNova](https://github.com/QuasariNova) at [this repository](https://github.com/QuasariNova/A1UP-SFTable-Arduino) has been able to decode the traffic between the serial controller and the Arcade1Up mainboard.

The goal in this project is to directly connect the Arcade1Up controller to a serial interface on a PC (via a PL2303 TTL to USB device) and translate buttons and movement from the Arcade1Up controller to linux uinput events.

### IDC 12 Pinout
| Pin | Function |
|--|--|
| 1 | Speaker(UNUSED) |
| 2 | Speaker(UNUSED) |
| 3 | GND |
| 4 | GND |
| 5 | UART(3.3V) 1 TX |
| 6 | UART(3.3V) 1 RX |
| 7 | 5V |
| 8 | GND |
| 9 | UART(3.3V) 2 RX |
| 10 | UART(3.3V) 2 TX |
| 11 | Power Switch |
| 12 |Gnd |

### UART encoding
I used a logic analyzer to decode what was going on via the serial communication. Looking at the rate of pulses I found out it was 115200 speed. Counting the pulses based on that speed I found out it was 8n1.

The game board polls the controller board every 20 ms by sending 3 bytes:

    A6 01 00
The board then responds by sending something like this:

    A7 10 0V 01 SJ XY 00 02 SJ XY 00 00 00 00 00 00 00 00

 - A710 is just the Header.
 - 01 is the header for player one
 - V(4 bits) is the volume switch placement. 0 is towards the left, 1 is centered, and 2 is to the right
 - S(4 bits) is the start button. 0010 is pressed, 0 is not
 - J(4 bits) is the joystick. 0001 is Down, 0010 is Left, 0100 is Up, and 1000 is Right. You can OR them for combinations.
 - X(4 bits) is for two buttons. 0001 is Jab and 0010 is Roundhouse. You can OR them for both being pressed
 - Y(4 bits) is for the four other buttons. 0001 is Forward, 0010 is Strong, 0100 is Fierce, and 1000 is Short.
 - 02 is the header for player 2. The rest are the same for as player 1.

On the four player cabinets, the second UART is the same format and still uses 01 and 02 to signify players 3 and 4, while Volume is always set to 2 on the second UART. It's not used for this project, but is pretty cool none the less.

### Putting it together
Knowing how the UART works and what pins go where, I prototyped my device. Due to the Leonardo using 5V logic and the controller board using 3.3V I had to reduce the voltage of my output to it, incase 5V might damage it. The Leonardo should read 3.3V just fine as it's within it's threshold. I first tried a simple voltage divider circuit, but it did not work. I suspect it pulled the power down too much and the controller board failed to read it. 

The solution, integrate a level shifter. Thankfully there are already boards that do that which are faily cheap, so I ordered some sparkfun ones, et voila, it worked! 

![View of Circuit Board Design](https://github.com/QuasariNova/A1UP-SFTable-Arduino/blob/main/IMAGES/SFTableArduinoShield.png?raw=true)

Since I got the circuit working and programmed a solution, all I needed to do was fabricate a board and solder it. One last thing I needed to do was write a shader to split the screen, so that both sides of the table can see the same view. I hadn't done shader coding before, but it was a quick enough learn to do. I decided to use reShade to inject the shader as it becomes automatic after setup.

You'll find the fabrication files and the source code to this in this repository.


