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

#define phone_number "XXXXXXX"

void setup()
{
  //Write here you correct baud rate
  Serial.begin(115200);

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

  //Enable 3G module power -> bit0:0
  bitClear(MySignals.expanderState, EXP_3G_POWER);
  MySignals.expanderWrite(MySignals.expanderState);

  MySignals.initSensorUART();
  MySignals.enableSensorUART(EXPANSION);
  
 
  delay(2000);

  // checks if the module is started
  uint8_t answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    delay(3000);
    // waits for an answer from the module
    while (answer == 0)
    {
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);
    }
  }
  MySignals.println("Connecting network");
  
  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
    sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

  MySignals.print("Setting SMS mode...");
  
  sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
  
  MySignals.println("Sending SMS");
  
  char aux_string[30];
  sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
  answer = sendATcommand(aux_string, ">", 2000);    // send the SMS number
  if (answer == 1)
  {
    Serial.println(F("Test-Arduino-Hello World"));
    Serial.write(0x1A);
    answer = sendATcommand("", "OK", 20000);
    
    if (answer == 1)
    {
      MySignals.println("Sent");    
    }
    else
    {
      MySignals.println("error");
    }
  }
  else
  {
    MySignals.println("error");
  }

  delay(1000);
}

void loop()
{
  delay(5000);
}




int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout)
{

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', sizeof(response));    // Initialize the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {

    if (Serial.available() != 0) {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}



