#ifndef PIN_ASSIGNMENTS_H
#define PIN_ASSIGNMENTS_H

/********************************************************************/
// Data wire for temperature probes is plugged into pin 2 on the Arduino 
// digital pin 0 and 1 are for RX0 and TX0

#define DIGPIN_TEMP_PROBES_ONE_WIRE_BUS 2
#define DIGPIN_PUMP 3
#define DIGPIN_CHIPSELECT_SDCARD 4
#define DIGPIN_SURGE_TANK_LEVEL 5 
#define DIGPIN_CHIPSELECT_ETHERNET 10
#define DIGPIN_STATUS_LED LED_BUILTIN   // expect 13

#define ANLGPIN_SOLAR_INTENSITY_PIN A4

/*
 Arduino communicates with both the W5100 and SD card using the SPI bus (through the ICSP header). 
 This is on digital pins 10, 11, 12, and 13 on the Uno and pins 50, 51, and 52 on the Mega. 
 On both boards, pin 10 is used to select the W5100 and pin 4 for the SD card. 
 These pins cannot be used for general I/O. 
 On the Mega, the hardware SS pin, 53, is not used to select either the W5100 or the SD card, but it must be kept as an output or the SPI interface won't work.
  Note that because the W5100 and SD card share the SPI bus, only one can be active at a time. 
  If you are using both peripherals in your program, this should be taken care of by the corresponding libraries. 
  If you're not using one of the peripherals in your program, however, you'll need to explicitly deselect it.
  To do this with the SD card, set pin 4 as an output and write a high to it. For the W5100, set digital pin 10 as a high output.
*/

// Real Time Clock
/*
 Connect the SCL pin to the I2C clock SCL pin on your Arduino. On an UNO & '328 based Arduino, this is also known as A5, on a Mega it is also known as digital 21 and on a Leonardo/Micro, digital 3
  Connect the SDA pin to the I2C data SDA pin on your Arduino. On an UNO & '328 based Arduino, this is also known as A4, on a Mega it is also known as digital 20 and on a Leonardo/Micro, digital 2
 */


#endif
