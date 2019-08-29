/*
 *
 *  Copyright (C) 2017 Libelium Comunicaciones Distribuidas S.L.
 * http://www.libelium.com
 *
 *  By using it you accept the MySignals Terms and Conditions.
 *  You can find them at: http://libelium.com/legal
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version:           2.0
 *  Design:            David Gascon
 *  Implementation:    Luis Martin / Victor Boria
 */ 

#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <UTouch.h>
#include <MySignals.h>
#include <MySignals_BLE.h>
#include <Wire.h>
#include <SPI.h>



Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);


UTouch  myTouch(14,21,15,16,17);

int touch_x;
int touch_y;


void setup(void) 
{
  Serial.begin(115200);
  MySignals.begin();
  
  Wire.begin();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  
}

void loop() 
{
 
  if (myTouch.dataAvailable())
  {
    myTouch.read();
    touch_x=myTouch.getX();
    touch_y=myTouch.getY();
    Serial.print("x=");
    Serial.print(touch_x);
    Serial.print("  y=");
    Serial.println(touch_y);
  }
}





