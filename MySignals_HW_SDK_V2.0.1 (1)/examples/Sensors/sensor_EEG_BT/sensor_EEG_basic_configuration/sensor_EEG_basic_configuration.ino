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
#include "Wire.h"
#include "SPI.h"

int8_t answer;


void setup()
{
  //Serial.begin(57600); // Communication mode baudrate with mindwave
  Serial.begin(38400);   // AT mode in HC-05 is always at this baudrate
  
  //You can use the configuration examples with the correct
  //AT+UART=**** command in order to change the baudrate

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
  
  //Enable BT2.0 module power -> bit2:1
  bitSet(MySignals.expanderState, EXP_BT_POWER);    
  //Enable BT2.0 Key to AT mode -> bit3:0
  bitClear(MySignals.expanderState, EXP_BT_KEY);  
  MySignals.expanderWrite(MySignals.expanderState);

  //Reset the module to enter command mode
  
  //Disable BT2.0 module power -> bit2:0
  bitClear(MySignals.expanderState, EXP_BT_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);

  //Enable BT2.0 module power -> bit2:1
  bitSet(MySignals.expanderState, EXP_BT_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);
  
  MySignals.initSensorUART();
  MySignals.enableSensorUART(EEG);
  //After enabling UART EEG you always need to intilize the UART at the right baudrate
  Serial.begin(38400);  // AT mode in HC-05 is always at this baudrate

  delay(1000);
  // Checks if the BT module is started
  answer = sendATcommand("AT", "OK", 6000);
  if (answer == 0)
  {
    MySignals.println("Error");
    // waits for an answer from the module
    while (answer == 0)
    {
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 6000);
    }
  }
  else if (answer == 1)
  {
    MySignals.println("BT HC05 succesfully working!");
  }
  
  //You better first try these commands via Gateway in a serial communication
  //program like Cutecom

  sendATcommand("AT+UART=57600,0,0", "0K", 2000);
  delay(5000);

  sendATcommand("AT+ROLE=1", "0K", 2000);
  delay(5000);

  sendATcommand("AT+PSWD=0000", "0K", 2000);
  delay(5000);

  sendATcommand("AT+CMODE=0", "0K", 2000);
  delay(5000);

  //Write you mindwave MAC for example: AT+PAIR=2068,9D,3F4DD1
  sendATcommand("AT+BIND=****,**,******", "0K", 2000); 
  delay(5000);

  sendATcommand("AT+INIT", "0K", 2000);
  delay(5000);

  sendATcommand("AT+IAC=9E8B33", "0K", 2000);
  delay(5000);

  sendATcommand("AT+CLASS=0", "0K", 2000);
  delay(5000);

  sendATcommand("AT+INQM=1,9,48", "0K", 2000);
  delay(5000);

  //Scan for BT devices MACS
  sendATcommand("AT+INQ", "0K", 2000);
  delay(20000);

  //For example: AT+PAIR=2068,9D,3F4DD1,20
  sendATcommand("AT+PAIR=****,**,******,20", "0K", 2000); //mindwave mac
  delay(10000);

  //For example: AT+LINK=2068,9D,3F4DD1
  sendATcommand("AT+LINK=****,**,****", "0K", 2000); //mindwave mac
  delay(10000);
}

void loop()
{

}


int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout)
{

  uint8_t x = 0,  answer = 0;
  char response[500];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while ( Serial.available() > 0) Serial.read();   // Clean the input buffer

  delay(1000);
  Serial.println(ATcommand);    // Send the AT command


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do 
  {
    if (Serial.available() != 0)
    {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
        //MySignals.println(response);
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}

