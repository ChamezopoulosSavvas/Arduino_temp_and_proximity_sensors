# Arduino_temp_and_proximity_sensors

This is a small project developed for the ECE-Auth class Microprocessors and Peripherals.

It is a simple program built for the arduino platform that measures the temperature in a room.
When a user comes close to the module, the last measured temperature along with the mean of the 24 last temperatures
measured are displayed on the LCD screen. An ultrasonic sensor is used to measure the distance of the user from the module.

It also supports led warning lights that notify the user in case of a low (-er than what we defined) temp (green), a high temp (red)
and a warning led (yellow)

The modules used:

* Arduino Rev3
* DS18B20 Temperature Sensor Module for Arduino
* HC-SR04 Ultrasonic Module Distance for Arduino
* LCD Display 16x2 Module HD44780
