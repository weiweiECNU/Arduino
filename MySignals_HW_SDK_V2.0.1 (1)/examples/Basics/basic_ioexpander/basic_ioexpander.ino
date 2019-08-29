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

void setup()
{
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
}

void loop()
{
  //Enable 3G module power -> bit0:0
  bitClear(MySignals.expanderState, EXP_3G_POWER);  
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Disable 3G module power -> bit0:1
  bitSet(MySignals.expanderState, EXP_3G_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

  //Enable WiFi ESP8266 Power -> bit1:1
  bitSet(MySignals.expanderState, EXP_ESP8266_POWER);  
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Disable WiFi ESP8266 Power -> bit1:0
  bitClear(MySignals.expanderState, EXP_ESP8266_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

  
  //Enable BT2.0 module power -> bit2:1
  bitSet(MySignals.expanderState, EXP_BT_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);

  //Disable BT2.0 module power -> bit2:0
  bitClear(MySignals.expanderState, EXP_BT_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);


  //Enable BT2.0 Key to communication mode -> bit3:1
  bitSet(MySignals.expanderState, EXP_BT_KEY);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);

  //Enable BT2.0 Key to AT mode -> bit3:0
  bitClear(MySignals.expanderState, EXP_BT_KEY);    
  MySignals.expanderWrite(MySignals.expanderState);
  
  delay(1000);
  
  //Enable ADC SPI -> bit4: 1
  bitSet(MySignals.expanderState, EXP_ADC_CS);   
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Disable ADC SPI -> bit4: 0
  bitClear(MySignals.expanderState, EXP_ADC_CS);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

  //Disable BLE UART flow control -> bit5: 1
  bitSet(MySignals.expanderState, EXP_BLE_FLOW_CONTROL);   
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Enable BLE UART flow control -> bit5: 0
  bitClear(MySignals.expanderState, EXP_BLE_FLOW_CONTROL);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

  //Enable BLE module power -> bit6: 1
  bitSet(MySignals.expanderState, EXP_BLE_POWER);   
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Disable BLE module power -> bit6: 0
  bitClear(MySignals.expanderState, EXP_BLE_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);

    
  //Enable WiFi module power (expansion board) -> bit7:0
  bitClear(MySignals.expanderState, EXP_ROVING_POWER);  
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
  
  //Disable WiFi module power (expansion board) -> bit7:1
  bitSet(MySignals.expanderState, EXP_ROVING_POWER);    
  MySignals.expanderWrite(MySignals.expanderState);

  delay(1000);
}


