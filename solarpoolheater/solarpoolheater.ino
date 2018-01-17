/********************************************************************/
#include <RTClib.h>

#include "DataStats.h"
#include "MovingAverage.h"
#include "TemperatureProbes.h"

/********************************************************************/

const char SPH_VERSION[] = "0.2";

void setup(void) 
{ 
 // start serial port 
 Serial.begin(9600);
 Serial.print("Version:");
 Serial.println(SPH_VERSION); 
 Serial.println("Setting up"); 

 
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIPSELECT)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  setupEthernet();

  datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);

  if (!datalogfile) {
    Serial.println("failed to open datafile for writing logged data");
  }

//  setupRTC();

} 

void loop(void) 
{ 
  tickTemperatureProbes();
  processIncomingSerial();
  /********************************************************************/

  if (temperatureDataStats[0].getCount() >= 60) {
//    loopEthernet();
//    loopRTC();

    // if the file is available, write to it:
    if (datalogfile) {  // don't forget to update DATALOG_BYTES_PER_SAMPLE
//      Serial.print("start datafile write:"); Serial.println(millis());
      for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
        float temp[3];
        if (probeStatuses[i] == OK) {
          temp[0] = temperatureDataStats[i].getMin();
          temp[1] = temperatureDataStats[i].getAverage();
          temp[2] = temperatureDataStats[i].getMax();
        } else {
          temp[0] = 0.0; temp[1] = 0.0; temp[2] = 0.0;  
        }
        datalogfile.write((byte *)temp, sizeof temp);
        temperatureDataStats[i].clear();
      }
//      Serial.print("start datafile flush:"); Serial.println(millis());
      datalogfile.flush();
//      Serial.print("end datafile flush:"); Serial.println(millis());
//      Serial.print("file size:");
//      Serial.println(datalogfile.size());
    }
  }
}

/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi

 */

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setupEthernet() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loopEthernet() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printDateTime(DateTime dateTime);

RTC_DS1307 RTC;

void setupRTC(void){
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
}

void loopRTC() {
  if (Serial.available() > 0) {
  int instruct = Serial.read();
  switch (instruct) {
    case 'D': {
      DateTime now = RTC.now();
      printDateTime(now);
      break;
    } case 'S':
      RTC.set(RTC_MONTH, 6);
      RTC.set(RTC_HOUR, 16);
      break;
    }
  }
}

void printDateTime(DateTime dateTime) {
  Serial.print(dateTime.year(), DEC);
  Serial.print('/');
  Serial.print(dateTime.month(), DEC);
  Serial.print('/');
  Serial.print(dateTime.day(), DEC);
  Serial.print(' ');
  Serial.print(dateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(dateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(dateTime.second(), DEC);
  Serial.println();
}
