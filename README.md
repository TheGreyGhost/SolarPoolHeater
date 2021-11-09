# SolarPoolHeater
Arduino controller for solar pool heating

An arduino project used to control a solar heating system for a swimming pool.

Uses DS18B20 one-wire temperature sensors, an ethernet shield based on WizNet W5100 (including SD card), and 
  a D51307 real time clock module

Three github projects are used for these:
• SolarPoolHeater (Arduino) - contains code and circuit diagrams for the Arduino which controls the heating components.
• AutomationWebServerCode (Raspberry Pi) - contains the python code used to retrieve data from the Arduino and store it in the database on the RPi
• SwimmingPoolWebServer (Raspberry Pi) - contains the web server code (php) used to display the monitoring data stored in the database



