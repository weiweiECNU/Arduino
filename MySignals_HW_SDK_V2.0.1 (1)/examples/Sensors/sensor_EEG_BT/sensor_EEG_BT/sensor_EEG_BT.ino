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

#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h"

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

#define MAX_CONCENTRATION 300
#define REPETITION 3

int i;
int m;


byte generatedChecksum = 0;
byte checksum = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;
int  payloadLength = 0;
long lastReceivedPacket = 0;
boolean bigPacket = false;

byte aux, concentration;
uint8_t counter = 0;


void setup()
{

  MySignals.begin();
  
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.drawString("Concentration:", 0, 0, 2);
  
  Serial.begin(57600);     // Communication mode baudrate with mindwave
  //Serial.begin(38400);   // AT mode in HC-05 is always at this baudrate

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
  //Enable BT2.0 Key to communication mode -> bit3:1
  bitSet(MySignals.expanderState, EXP_BT_KEY);
  MySignals.expanderWrite(MySignals.expanderState);

  //Reset the module to enter communication mode

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
  Serial.begin(57600);

  delay(1000);

  tft.drawNumber(concentration, 0, 30, 2);
}


void loop() 
{
  if (ReadOneByte() == 170) 
  {
    if (ReadOneByte() == 170) 
    {
      if (ReadOneByte() == 32) 
      {
        for (i = 0; i < 32; i++) 
        {
          aux = ReadOneByte();

          if (i == 28) 
          {
            concentration = ReadOneByte();
            tft.fillRect(0, 30, 40, 20, ILI9341_BLACK);
            tft.drawNumber(concentration, 0, 30, 2);

            if (concentration >= MAX_CONCENTRATION) 
            {
              counter++;
            }
            else
            {
              counter = 0;
            }
          }
        }
      }
    }
  }

  if (counter >= REPETITION)
  {
    counter = 0;

  }
}



byte ReadOneByte()
{
  int ByteRead;
  while (!Serial.available());
  ByteRead = Serial.read();
  return ByteRead;
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
  do {

    if (Serial.available() != 0)
    {
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
        MySignals.println(response);
      }
    }
    // Waits for the asnwer with time out
  }
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}



