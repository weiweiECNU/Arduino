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
#include "Wire.h"
#include "SPI.h" 


void setup() 
{
  Serial.begin(115200);
  MySignals.begin();  
}


void loop() 
{
 
  float conductance = MySignals.getCalibratedGSR(100, 1, 0, CONDUCTANCE);
  float resistance = MySignals.getCalibratedGSR(100, 1, 0, RESISTANCE);
  float conductanceVol = MySignals.getCalibratedGSR(100, 1, 0, VOLTAGE);

  Serial.print("Conductance : ");       
  Serial.print(conductance, 2);  
  Serial.println("");         

  Serial.print("Resistance : ");       
  Serial.print(resistance, 2);  
  Serial.println("");    

  Serial.print("Conductance Voltage : ");       
  Serial.print(conductanceVol, 4);  
  Serial.println("");

  Serial.print("\n");

  // wait for a second  
  delay(1000);            
}




