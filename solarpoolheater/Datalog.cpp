#include "Datalog.h"
#include <SPI.h>
#include <SD.h>
#include <Ethernet.h>
#include <Wire.h>

#include "TemperatureProbes.h"

const int SD_CHIPSELECT = 4;
File datalogfile;
const int DATALOG_BYTES_PER_SAMPLE = NUMBER_OF_PROBES * 3 * sizeof(float); // each sample has min, avg, max for each probe
const char DATALOG_FILENAME[] = "datalog.txt";

volatile LogfileStatus logfileStatus;

const int LOG_PERIOD_SAMPLES = 60;

void setupEthernet();

void setupDatalog()
{
  Serial.print("Initializing SD card..."); // todo comment out

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIPSELECT)) {
    logfileStatus = LFS_CARD_NOT_PRESENT;
    return;
  }
  logfileStatus = LFS_OK;
  Serial.println("card initialized.");  // todo comment out
  setupEthernet();

  datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
  if (!datalogfile) {
    logfileStatus = LFS_FAILED_TO_OPEN;
  }
}

void tickDatalog()
{
  if (temperatureDataStats[0].getCount() >= LOG_PERIOD_SAMPLES) {

    // if the file is available, write to it:
    if (datalogfile) {  // don't forget to update DATALOG_BYTES_PER_SAMPLE
//      Serial.print("start datafile write:"); Serial.println(millis());
      datalogfile.seek(datalogfile.size());

      for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
        float temp[3];
        if (probeStatuses[i] == PS_OK) {
          temp[0] = temperatureDataStats[i].getMin();
          temp[1] = temperatureDataStats[i].getAverage();
          temp[2] = temperatureDataStats[i].getMax();
        } else {
          temp[0] = 0.0; temp[1] = 0.0; temp[2] = 0.0;  
        }
        size_t bytesWritten = datalogfile.write((byte *)temp, sizeof temp);
        if (bytesWritten != sizeof temp) {
          logfileStatus = LFS_WRITE_FAILED;
        }
        temperatureDataStats[i].clear();
      }
      datalogfile.flush();
    }
  }
}

bool dataLogErase() 
{
  bool success;
  datalogfile.close();
  success = SD.remove(DATALOG_FILENAME);
  if (success) {
    datalogfile = SD.open(DATALOG_FILENAME, FILE_WRITE);
    if (!datalogfile) {
      success = false;
    }
  }
  return success;
}

unsigned long dataLogNumberOfSamples() 
{
  return datalogfile.size() / DATALOG_BYTES_PER_SAMPLE;
}

void dataLogExtractEntries(Print &dest, long startidx, long numberOfEntries)
{
  unsigned long filesize = datalogfile.size();
  unsigned long samplesInFile = filesize / DATALOG_BYTES_PER_SAMPLE;
  if (startidx >= samplesInFile) {
     dest.print("arg1 exceeds file size:");
     dest.println(samplesInFile);
  } else {  
    datalogfile.seek(startidx * DATALOG_BYTES_PER_SAMPLE);
    long samplesToRead = min(numberOfEntries, min(24*60, samplesInFile - startidx));  // something wrong here?  asked for 2530 but only got 1420?
    for (int i = 0; i < NUMBER_OF_PROBES; ++i) {
      dest.print(probeNames[i]); 
      dest.print(" min avg max ");
    }
    dest.println();
    for (long i = 0; i < samplesToRead; ++i) {
      for (int j = 0; j < NUMBER_OF_PROBES; ++j) {
        float temp[3];
        for (int k = 0; k < sizeof(temp); ++k) {
          ((byte *)temp)[k] = datalogfile.read(); 
        }
        for (int k = 0; k < 3; ++k) {
          dest.print(temp[k], 1); 
          dest.print(" ");
        }  
      }
      dest.println();
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

