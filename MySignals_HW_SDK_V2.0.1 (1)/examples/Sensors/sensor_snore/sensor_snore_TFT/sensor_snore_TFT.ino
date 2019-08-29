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

#define graphic_low_extrem 230
#define graphic_high_extrem 30
#define graphic_left_extrem 0
#define graphic_right_extrem 320


Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);


//! It stores the current value of x position in the LCD.
uint16_t graphic_x;
//! It stores the current value of the MySignals. 
uint16_t valRead;
//! It stores the previous value of the MySignals. 
uint16_t graphic_prevRead;


void setup(void) 
{
  Serial.begin(115200);
  
  MySignals.begin();

  MySignals.initSnore();
      
  tft.init();
  tft.setRotation(3);
  
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0,0,320,30,ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE); 
  tft.drawString("Snore",5,5,4); 
}

void loop() 
{

  valRead = (uint16_t)MySignals.getSnore(DATA);
    
  valRead = map(valRead, 0, 1023, 230, 30);
  
  printGraphic(valRead, 0);
  
}



void printGraphic(uint16_t value, uint8_t delay_time)
{

  if (value < graphic_high_extrem)
  {
    value = graphic_high_extrem;
  }
  if (value > graphic_low_extrem)
  {
    value = graphic_low_extrem;
  }


  //Pinta la linea solo a partir de que ya exista al menos 1 valor
  if (graphic_x > graphic_left_extrem + 1)
  {
    tft.drawLine(graphic_x - 1, graphic_prevRead, graphic_x, value, ILI9341_RED);
  }

  //Wave refresh (barre pantalla pintando una linea)
  tft.drawLine(graphic_x + 1, graphic_high_extrem, graphic_x + 1, graphic_low_extrem, ILI9341_WHITE);


  graphic_prevRead = value;
  graphic_x++;

  delay(delay_time);

  if (graphic_x == graphic_right_extrem)
  {
    graphic_x = graphic_left_extrem;
  }
  SPI.end();
}
