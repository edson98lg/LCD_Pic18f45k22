# LCD_Pic18f45k22
Program focused on the bit configuration of the Pic18f45k22, this program measures temperature and humidity, and according to the threshold ranges will turn on different visual alarms and can turn on a 24v motor.


![circuit diagram](https://raw.githubusercontent.com/edson98lg/LCD_Pic18f45k22/master/assets/Schematic.JPG)

## Control
The control system has 4 buttons, three buttons are for moving and selecting within the interface, and the fourth button is the one at the top, and is used to enter the programmer mode.

![Front view](https://raw.githubusercontent.com/edson98lg/LCD_Pic18f45k22/master/assets/front%20view.jpeg)

### Programmer mode
In this mode, you can change the minimum and maximum ranges, and the time the heater is desired to be on.

## Greenhouse
This is the mini greenhouse, it has two sensors, humidity and temperature, besides it has a 12 volts motor which is activated when the temperature exceeds 30 °C, besides that, it has a leds system which simulates a heater, and the heater is activated when the humidity or temperature drops below the minimum range.

![mini greenhouse view](https://raw.githubusercontent.com/edson98lg/LCD_Pic18f45k22/master/assets/Mini%20greenhouse%20view.jpeg)


## What have I used?
* Pic 18f45k22
   * Timer
   * Pwm
   * I/O digital sink
   * I analog
* Lcd 4x20
* 1 RGB led
* 1 blue led
* 4 red leds (to simulate the heater)
* 3 push buttons
* 1 Latching switch
* 2 2N2222 Transistor
* 1 optocoupler
  
  
## Expressions of Gratitude 😊
* Publicly thanks 😊
* Donate with SOLANA to this address: `BGADeDwPhghunkZevrT1rxBfRTYFDQwhRABUWgX8EWGD`
* Donate with ETH to this address: `0x879e34c7Ae54A6A3936d7eD9A1c3E81Ae52C4`
