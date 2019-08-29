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



Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);


int valuePulse;
int valueSPO2;
uint8_t pulsioximeter_state = 0;

void setup() 
{

  Serial.begin(19200);
  MySignals.begin();
  
  MySignals.initSensorUART();
  MySignals.enableSensorUART(PULSIOXIMETER);
  
  tft.init();   
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  
  tft.drawString("SPO2:", 0, 0, 2);
}

void loop() 
{

  if (MySignals.spo2_micro_detected == 0 && MySignals.spo2_mini_detected == 0)
  {
    uint8_t statusPulsioximeter = MySignals.getStatusPulsioximeterGeneral();

    if (statusPulsioximeter == 1)
    {
      MySignals.spo2_mini_detected = 0;
      MySignals.spo2_micro_detected = 1;
      
      tft.drawString("Micro detected", 0, 100, 2);
    }
    else if (statusPulsioximeter == 2)
    {
      MySignals.spo2_mini_detected = 1;
      MySignals.spo2_micro_detected = 0;

      tft.drawString("Mini detected", 0, 100, 2);
    }
    else
    {
      MySignals.spo2_micro_detected = 0;
      MySignals.spo2_mini_detected = 0;
    }
  }


  if (MySignals.spo2_micro_detected == 1)
  {
    MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
    delay(10);
    uint8_t getPulsioximeterMicro_state = MySignals.getPulsioximeterMicro();

    if (getPulsioximeterMicro_state == 1)
    {
      tft.drawNumber(MySignals.pulsioximeterData.BPM, 0, 30, 2);
      tft.drawNumber(MySignals.pulsioximeterData.O2, 0, 45, 2);
     
    }
    else if (getPulsioximeterMicro_state == 2)
    {
      //Finger out or calculating
    }
    else
    {
      MySignals.spo2_micro_detected = 0;
      //SPO2 Micro lost connection
    }
  }


  if (MySignals.spo2_mini_detected == 1)
  {
    MySignals.enableSensorUART(PULSIOXIMETER);

    uint8_t getPulsioximeterMini_state = MySignals.getPulsioximeterMini();

    if (getPulsioximeterMini_state == 1)
    {
      tft.drawNumber(MySignals.pulsioximeterData.BPM, 0, 30, 2);
      tft.drawNumber(MySignals.pulsioximeterData.O2, 0, 45, 2);
    }
    else if (getPulsioximeterMini_state == 2)
    {
      //Finger out or calculating
    }
    else if (getPulsioximeterMini_state == 0)
    {
      MySignals.spo2_mini_detected = 0;
      //SPO2 Mini lost connection
    }
  }

  delay(1000);
}





