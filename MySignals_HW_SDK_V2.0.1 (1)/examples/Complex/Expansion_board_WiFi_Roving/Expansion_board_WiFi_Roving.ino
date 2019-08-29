/*

    Copyright (C) 2017 Libelium Comunicaciones Distribuidas S.L.
   http://www.libelium.com

    By using it you accept the MySignals Terms and Conditions.
    You can find them at: http://libelium.com/legal

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Version:           2.0
    Design:            David Gascon
    Implementation:    Luis Martin / Victor Boria
*/

#include <MySignals.h>
#include <Wire.h>
#include "SPI.h"


//Enter here your wifi network data
#define wifi_ssid "XXXXXXX"
#define wifi_password "XXXXXXXX"


void setup()
{
  //Write here you correct baud rate
  Serial.begin(9600);
  
  MySignals.begin();

  /*
    expanderState is initialized with B10100001

    Expansor pin library names:

    EXP_3G_POWER
    EXP_ESP8266_POWER
    EXP_BT_POWER
    EXP_BT_KEY
    EXP_ADC_CS
    EXP_BLE_FLOW_CONTROL
    EXP_BLE_POWER
    EXP_ROVING_POWER
  */
  //Enable WiFi module power (expansion board) -> bit7:0
  bitClear(MySignals.expanderState, EXP_ROVING_POWER);  
  MySignals.expanderWrite(MySignals.expanderState);

  MySignals.initSensorUART();
  MySignals.enableSensorUART(EXPANSION);

  sendCommand("exit\r", "EXIT", 2000);
  
  delay(2000);

  if (enterConfig(2000)) 
  {
    sendCommand("leave\r", "DeAuth", 2000);
    delay(1000);
    
    // Sets DHCP and TCP protocol
    sendCommand("set ip dhcp 1\r", "AOK", 2000);
    delay(1000);
    
    sendCommand("set ip protocol 18\r", "AOK", 2000);
    delay(1000);

    // Configures the way to join the network AP, sets the encryption of the
    // network and joins it
    sendCommand("set wlan join 0\r", "AOK", 2000); //The auto-join feature is disabled
    delay(1000);
    
    char aux_str[50];
    snprintf(aux_str, sizeof(aux_str), "set wlan phrase %s\r", wifi_password);
    sendCommand(aux_str, "AOK", 2000);
    delay(1000);
    
    snprintf(aux_str, sizeof(aux_str), "join %s\r", wifi_ssid);
    uint8_t answer = sendCommand(aux_str, "Associated", 10000);


    if (answer == 1)
    {

      snprintf(aux_str, sizeof(aux_str), "Connected to \"%s\"", wifi_ssid);
      MySignals.println(aux_str);
      delay(5000);
    }

    else
    {
      snprintf(aux_str, sizeof(aux_str), "Error connecting to: \"%s\"", wifi_ssid);
      MySignals.println(aux_str);
      delay(1000);
    }
  }
  else
  {
    MySignals.println("Wifi no OK");
  }
}

void loop()
{
  delay(5000);
}




int8_t sendCommand(const char* Command, const char* expected_answer, unsigned int timeout) 
{
  char response[300];
  uint8_t x = 0,  answer = 0;
  unsigned long previous;

  memset(response, 0, sizeof(response));    // Initialize the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  Serial.println(Command);    // Send Command

  x = 0;
  previous = millis();

  // this loop waits for the answer
  do 
  {
    if (Serial.available() != 0) 
    {
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}


int8_t enterConfig(unsigned int timeout)
{
  char response[100];
  uint8_t x = 0,  answer = 0;
  unsigned long previous;

  memset(response, 0, 100);    // Initialize the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  Serial.print("$$$");    // Send Command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    if (Serial.available() != 0) 
    {
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial.read();
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, "CMD") != NULL)
      {
        answer = 1;
      }
    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}



