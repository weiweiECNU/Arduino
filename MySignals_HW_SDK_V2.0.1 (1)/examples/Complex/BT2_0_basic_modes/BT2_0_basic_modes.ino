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
  MySignals.begin();

}

void loop()
{
  //Mode: communication    -   Power: off
  bitClear(MySignals.expanderState, EXP_BT_POWER); //Disable BT2.0 module power -> bit2:0
  bitSet(MySignals.expanderState, EXP_BT_KEY);     //Enable BT2.0 Key to communication mode -> bit3:1
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

  //Mode: communication    -   Power:  on
  bitSet(MySignals.expanderState, EXP_BT_POWER);   //Enable BT2.0 module power -> bit2:1
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(5000);

  //Mode: AT    -   Power:  off
  bitClear(MySignals.expanderState, EXP_BT_POWER); //Disable BT2.0 module power -> bit2:0 
  bitClear(MySignals.expanderState, EXP_BT_KEY);   //Enable BT2.0 Key to AT mode -> bit3:0
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);
  
  //Mode: AT    -   Power:  on
  bitSet(MySignals.expanderState, EXP_BT_POWER);   //Enable BT2.0 module power -> bit2:1
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(5000);
}

