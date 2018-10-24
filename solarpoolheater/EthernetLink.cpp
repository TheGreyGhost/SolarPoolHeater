#include "EthernetLink.h"
#include <Arduino.h>
#include <Ethernet.h>

void tickEthernet()
{
//  unsigned long secondsSinceLastLog = (millis() - lastLogTime) / 1000;
//  if (secondsSinceLastLog >= (isSystemIdle() ?  getSetting(SET_logIntervalIdleSeconds) :  getSetting(SET_logIntervalSeconds))) {
//    lastLogTime = millis();    
//    // if the file is available, write to it:
//    if (datalogfile) {  // don't forget to update DATALOG_BYTES_PER_SAMPLE
//      datalogfile.seek(datalogfile.size());
//
//      size_t totalBytesWritten = 0;
//      long timestamp = currentTime.secondstime();
//      size_t bytesWritten = datalogfile.write((byte *)&timestamp, sizeof timestamp);
//      if (bytesWritten != sizeof timestamp) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      totalBytesWritten += bytesWritten;
//      
//      for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
//        float temp[3];
//        if (probeStatuses[i] == PS_OK) {
//          temp[0] = temperatureDataStats[i].getMin();
//          temp[1] = temperatureDataStats[i].getAverage();
//          temp[2] = temperatureDataStats[i].getMax();
//        } else {
//          temp[0] = 0.0; temp[1] = 0.0; temp[2] = 0.0;  
//        }
//        bytesWritten = datalogfile.write((byte *)temp, sizeof temp);
//        if (bytesWritten != sizeof temp) {
//          logfileStatus = LFS_WRITE_FAILED;
//        }
//        totalBytesWritten += bytesWritten;
//        temperatureDataStats[i].clear();
//      }
//
//      bytesWritten = datalogfile.write((byte *)&cumulativeInsolation, sizeof cumulativeInsolation);
//      if (bytesWritten != sizeof cumulativeInsolation) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      totalBytesWritten += bytesWritten;
//
//    // todo remove
//      float tempLevel = surgeTankLevelStats.getAverage();
//      surgeTankLevelStats.clear();
//      bytesWritten = datalogfile.write((byte *)&tempLevel, sizeof tempLevel);
//      if (bytesWritten != sizeof tempLevel) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      totalBytesWritten += bytesWritten;
//
//      bytesWritten = datalogfile.write((byte *)&pumpRuntimeSeconds, sizeof pumpRuntimeSeconds);
//      if (bytesWritten != sizeof pumpRuntimeSeconds) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      totalBytesWritten += bytesWritten;
//
//      PumpState pumpState = getPumpState();
//      bytesWritten = datalogfile.write((byte *)&pumpState, sizeof pumpState);
//      if (bytesWritten != sizeof pumpState) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      totalBytesWritten += bytesWritten;
//      
//      if (totalBytesWritten != DATALOG_BYTES_PER_SAMPLE) {
//        logfileStatus = LFS_WRITE_FAILED;
//      }
//      datalogfile.flush();
//    }
//  }
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
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED //todo update
};
IPAddress ip(192, 168, 2, 35);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setupEthernet() {
  // Open serial communications and wait for port to open:

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
