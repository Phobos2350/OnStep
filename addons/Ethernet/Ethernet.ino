/*
 * Title       OnStep Ethernet Server
 * by          Howard Dutton
 *
 * Copyright (C) 2016 to 2019 Howard Dutton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 *
 * Revision History, see GitHub
 *
 *
 * Author: Howard Dutton
 * http://www.stellarjourney.com
 * hjd1964@gmail.com
 *
 * Description
 *
 * W5100 and W5500 Ethernet for OnStep
 *
 */
 
#define OETHS
#define Product "OnEth"
#define FirmwareDate          __DATE__
#define FirmwareTime          __TIME__
#define FirmwareVersionMajor  "1"
#define FirmwareVersionMinor  "8"
#define FirmwareVersionPatch  "d"

#define Version FirmwareVersionMajor "." FirmwareVersionMinor FirmwareVersionPatch

// work around PROGMEM use on Teensy3.2: FPSTR() gets ignored
#define FPSTR

#include <limits.h>

// pretty sure this wasn't being compiled in even if W5500 was ON (ahead of #include Config.h) so commented out for now.
//#ifdef W5500_ON
//  #include <Ethernet3.h>  // https://github.com/PaulStoffregen/Ethernet
//#else
//  #include <Ethernet.h>
//#endif

#define DEBUG_OFF   // Turn _ON to allow Ethernet startup without OnStep (Serial port for debug at 9600 baud)

#include <Ethernet.h>
#include "CmdServer.h"

#include "Constants.h"
#include "Config.h"

#include "WebServer.h"

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#if SERIAL_BAUD<=28800
  #define TIMEOUT_WEB 60
  #define TIMEOUT_CMD 60
#else
  #define TIMEOUT_WEB 15
  #define TIMEOUT_CMD 30
#endif

#define ICACHE_RAM_ATTR
#define AXIS1_ENC_A_PIN 5  // pin# for Axis1 encoder, for A or CW
#define AXIS1_ENC_B_PIN 6  // pin# for Axis1 encoder, for B or CCW
#define AXIS2_ENC_A_PIN 7  // pin# for Axis2 encoder, for A or CW
#define AXIS2_ENC_B_PIN 8  // pin# for Axis2 encoder, for B or CCW
#include "Encoders.h"
#if ENCODERS == ON
Encoders encoders;
#endif

#include "MountStatus.h"

// macros to help with sending webpage data
#define sendHtmlStart()
#define sendHtml(x) client->print(x); x=""
#define sendHtmlDone(x) client->print(x); x=""

int WebTimeout=TIMEOUT_WEB;
int CmdTimeout=TIMEOUT_CMD;

WebServer server;
CmdServer cmdSvr;

#if defined(_mk20dx128_h_) || defined(__MK20DX128__) || defined(__MK20DX256__)
  #include <EEPROM.h>
#else
  #define EEPROM_DISABLED
#endif
#define EEPROM_COMMIT_DISABLED

void handleNotFound(EthernetClient *client) {
  String message = "File Not Found\n\n";
  client->print(message);
//  server.send(404, "text/plain", message);
}

void setup(void){
  Ser.begin(SERIAL_BAUD_DEFAULT);
  delay(2000);

// EEPROM Init
#ifndef EEPROM_DISABLED
  if ((EEPROM_readInt(0)!=8267) || (EEPROM_readInt(2)!=0)) {
    EEPROM_writeInt(0,8267);
    EEPROM_writeInt(2,0);
#if ENCODERS == ON
    EEPROM_writeLong(600,Axis1EncDiffLimit);
    EEPROM_writeLong(604,Axis2EncDiffLimit);
    EEPROM_writeLong(608,20);  // enc short term average samples
    EEPROM_writeLong(612,200); // enc long term average samples
    EEPROM_writeLong(616,0);   // enc rate comp
    EEPROM_writeLong(624,1);   // intpol phase
    EEPROM_writeLong(628,0);   // intpol mag
    EEPROM_writeLong(632,10);  // prop
#endif
  } else {  
#if ENCODERS == ON
    Axis1EncDiffLimit=EEPROM_readLong(600);
    Axis2EncDiffLimit=EEPROM_readLong(604);
#if AXIS1_ENC_RATE_CONTROL == ON
    Axis1EncStaSamples=EEPROM_readLong(608);
    Axis1EncLtaSamples=EEPROM_readLong(612);
    long l=EEPROM_readLong(616); Axis1EncRateComp=(float)l/1000000.0;
    Axis1EncIntPolPeriod=EEPROM_readLong(620);
    Axis1EncIntPolPhase =EEPROM_readLong(624);
    Axis1EncIntPolMag   =EEPROM_readLong(628);
    Axis1EncProp        =EEPROM_readLong(632);
    Axis1EncMinGuide    =EEPROM_readLong(636);
#endif
#endif
  }
#endif

  byte tb=0;
Again:
  char c=0;

  // clear the buffers and any noise on the serial lines
  for (int i=0; i<3; i++) {
    Ser.print(":#");
    delay(500);
    c=serialRecvFlush();
  }

  if (SERIAL_BAUD!=SERIAL_BAUD_DEFAULT) {

    // switch OnStep Serial up to ? baud
    Ser.print(HighSpeedCommsStr(SERIAL_BAUD));
    delay(200);
    int count=0; c=0; while (Ser.available() > 0) { count++; if (count == 1) c=Ser.read(); }
    if (c=='1') {
      Ser.begin(SERIAL_BAUD);
      delay(3000);
    } else {
      // got nothing back, toggle baud rate and try again
      tb++;
      if (tb == 11) tb=1;
      if (tb == 1) Ser.begin(SERIAL_BAUD_DEFAULT);
      if (tb == 6) Ser.begin(SERIAL_BAUD);
  
      if (tb == 1 || tb == 6) delay(2000);
#ifndef DEBUG_ON
      goto Again;
#else
      Ser.begin(9600);
#endif
    }
    
    // clear the buffers and any noise on the serial lines
    for (int i=0; i<3; i++) {
      Ser.print(":#");
      delay(50);
      serialRecvFlush();
    }
  }

#if W5500 == ON
  // reset a W5500
  pinMode(9, OUTPUT); 
  digitalWrite(9, LOW);
  delayMicroseconds(500);
  digitalWrite(9, HIGH);
  delayMicroseconds(1000);
  delay(1000);
#endif

  // Initialize the www server
  server.init();
  server.on("index.htm", handleRoot);
  server.on("configuration.htm", handleConfiguration);
  server.on("settings.htm", handleSettings);
  server.on("settings.txt", settingsAjax);
  server.on("control.htm", handleControl);
  server.on("control.txt", controlAjax);
#if ENCODERS == ON
  server.on("/enc.htm", handleEncoders);
  server.on("/encA.txt", encAjaxGet);
  server.on("/enc.txt", encAjax);
#endif
  server.on("/control.txt", controlAjax);
  server.on("/controlA.txt", controlAjaxGet);
  server.on("pec.htm", handlePec);
  server.on("pec.txt", pecAjax);
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  // Initialize the cmd server, timeout after 500ms
  cmdSvr.init(9999,500);

#if ENCODERS == ON
  encoders.init();
#endif
}

void loop(void){
  server.handleClient();
  cmdSvr.handleClient();
#if ENCODERS == ON
  encoders.poll();
#endif

  // check clients for data, if found get the command, send cmd and pickup the response, then return the response
  static char writeBuffer[40]="";
  static int writeBufferPos=0;
  while (cmdSvr.available()) {
    // get the data
    byte b=cmdSvr.read();
    writeBuffer[writeBufferPos]=b; writeBufferPos++; if (writeBufferPos>39) writeBufferPos=39; writeBuffer[writeBufferPos]=0;

    // send cmd and pickup the response
    if (b=='#' || ((strlen(writeBuffer)==1) && (b==(char)6))) {
      char readBuffer[40]="";
      readLX200Bytes(writeBuffer,readBuffer,CmdTimeout); writeBuffer[0]=0; writeBufferPos=0;

      // return the response, if we have one
      if (strlen(readBuffer)>0) {
        cmdSvr.print(readBuffer);
        delay(2);
      }

    } else {
      server.handleClient();
#if ENCODERS == ON
      encoders.poll();
#endif
    }
  }
}

const char* HighSpeedCommsStr(long baud) {
  if (baud==115200) { return ":SB0#"; }
  if (baud==57600) { return ":SB1#"; }
  if (baud==38400) { return ":SB2#"; }
  if (baud==28800) { return ":SB3#"; }
  if (baud==19200) { return ":SB4#"; } else { return ":SB5#"; }
}
