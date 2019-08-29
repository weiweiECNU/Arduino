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

    Version:           0.1
    Design:            David Gascon
    Implementation:    Luis Martin / Victor Boria
*/

#include <MySignals.h>
#include <Wire.h>
#include "SPI.h"


void setup()
{
  Serial.begin(9600);
  MySignals.begin();
  MySignals.initSensorUART();
  MySignals.enableSensorUART(SPIROMETER);



  while (MySignals.getStatusSpiro() == 0)
  {
    delay(100);
  }
  MySignals.println("Spirometer is ON");

}

void loop()
{
  MySignals.getSpirometer();



  MySignals.disableMuxUART();
  Serial.print(F("Number of measures:"));
  Serial.println(MySignals.spir_measures);
  Serial.println();

  for (int i = 0; i < MySignals.spir_measures; i++)
  {

    Serial.print(MySignals.spirometerData[i].spir_pef);
    Serial.print(F("L/min "));

    Serial.print(MySignals.spirometerData[i].spir_fev);
    Serial.print(F("L "));

    Serial.print(MySignals.spirometerData[i].spir_hour);
    Serial.print(F(":"));
    Serial.print(MySignals.spirometerData[i].spir_minutes);

    Serial.print(F(" "));

    Serial.print(MySignals.spirometerData[i].spir_year);
    Serial.print(F("-"));
    Serial.print(MySignals.spirometerData[i].spir_month);
    Serial.print(F("-"));
    Serial.println(MySignals.spirometerData[i].spir_day);


  }
  Serial.println(F("************"));
  MySignals.enableMuxUART();
  delay(5000);

}
