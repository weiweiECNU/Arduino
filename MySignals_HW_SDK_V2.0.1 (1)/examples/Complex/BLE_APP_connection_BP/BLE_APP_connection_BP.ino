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
#include <MySignals_BLE.h>
#include <Wire.h>
#include <SPI.h>

char buffer_tft[30];

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

unsigned long previous;


// Sensor list
bool selected_airflow;
bool selected_ecg;
bool selected_emg;
bool selected_gsr;
bool selected_position;
bool selected_snore;
bool selected_temp;
bool selected_spiro;
bool selected_eeg;
bool selected_spo2_uart;
bool selected_bp_uart;
bool selected_gluco_uart;
bool selected_scale_ble;
bool selected_spo2_ble;
bool selected_bp_ble;
bool selected_gluco_ble;

uint8_t blood_syst_low;
uint8_t blood_syst_high;
uint8_t blood_dias_low;
uint8_t blood_dias_high;
uint8_t blood_bpm_low;
uint8_t blood_bpm_high;


void setup()
{
  MySignals.begin();

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  tft.drawString("Connect blood pressure to continue", 0, 0, 2);

  Serial.begin(115200);

  MySignals.initSensorUART();

  MySignals.enableSensorUART(BLOODPRESSURE);

  while (MySignals.getStatusBP() == 0)
  {
    delay(100);
  }

  delay(100);

  if (MySignals.getBloodPressure() == 1)
  {
    if (MySignals.bloodPressureData.systolic != 0)
    {

      blood_syst_low = MySignals.bloodPressureData.systolic & 0b0000000011111111;
      blood_syst_high = (MySignals.bloodPressureData.systolic & 0b1111111100000000) / 256;

      blood_dias_low = MySignals.bloodPressureData.diastolic & 0b0000000011111111;
      blood_dias_high = (MySignals.bloodPressureData.diastolic & 0b1111111100000000) / 256;

      blood_bpm_low = MySignals.bloodPressureData.pulse & 0b0000000011111111;
      blood_bpm_high = (MySignals.bloodPressureData.pulse & 0b1111111100000000) / 256;

    }
  }

 MySignals.enableSensorUART(BLE);

  //Enable BLE module power -> bit6: 1
  bitSet(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);

  //Enable BLE UART flow control -> bit5: 0
  bitClear(MySignals.expanderState, EXP_BLE_FLOW_CONTROL);
  MySignals.expanderWrite(MySignals.expanderState);


  //Disable BLE module power -> bit6: 0
  bitClear(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);

  delay(500);

  //Enable BLE module power -> bit6: 1
  bitSet(MySignals.expanderState, EXP_BLE_POWER);
  MySignals.expanderWrite(MySignals.expanderState);
  delay(1000);

  MySignals_BLE.initialize_BLE_values();

  //Clean the input serial buffer
  while (Serial.available() > 0 )
  {
    Serial.read();
  }



  if (MySignals_BLE.initModule() == 1)
  {

    if (MySignals_BLE.sayHello() == 1)
    {
      //TFT message: "BLE init ok";
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[1])));
      tft.drawString(buffer_tft, 0, 15, 2);
    }
    else
    {
      //TFT message:"BLE init fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[2])));
      tft.drawString(buffer_tft, 0, 15, 2);


      while (1)
      {
      };
    }
  }
  else
  {
    //TFT message: "BLE init fail"
    strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[2])));
    tft.drawString(buffer_tft, 0, 15, 2);

    while (1)
    {
    };
  }
}


void loop()
{

  //1. SET MODE: SLAVE (VISIBLE TO APP)
  while ((MySignals_BLE.ble_mode_flag == master_mode))
  {

    if (MySignals_BLE.setMode(slave_mode) == 0)
    {
      //TFT message:  "Slave mode ok";
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[3])));
      tft.drawString(buffer_tft, 0, 30, 2);


      MySignals_BLE.ble_mode_flag = slave_mode;
    }
    else
    {
      //TFT message:  "Slave mode fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[4])));
      tft.drawString(buffer_tft, 0, 30, 2);


      MySignals_BLE.hardwareReset();
      delay(100);
      MySignals_BLE.initialize_BLE_values();
    }
  }



  //2. SET BONDABLE MODE
  if (MySignals_BLE.bond_mode_and_mitm == 0)
  {
    if (MySignals_BLE.setBondableMode(BLE_ENABLE_BONDING) == 0)
    {

      //TFT message:  "Bondable mode ok"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[5])));
      tft.drawString(buffer_tft, 0, 45, 2);



      //3. SET SM PARAMETERS
      if (MySignals_BLE.setSMParameters(BLE_ENABLE_MITM) == 0)
      {
        //TFT message:  "SM parameters ok"
        strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[7])));
        tft.drawString(buffer_tft, 0, 60, 2);


        MySignals_BLE.bond_mode_and_mitm = 1;

      }
      else
      {
        //TFT message:  "SM parameters fail"
        strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[8])));
        tft.drawString(buffer_tft, 0, 60, 2);

        MySignals_BLE.hardwareReset();
        delay(100);
        MySignals_BLE.initialize_BLE_values();
      }
    }
    else
    {
      //TFT message:  "Bondable mode fail"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[6])));
      tft.drawString(buffer_tft, 0, 45, 2);

      MySignals_BLE.hardwareReset();
      delay(100);
      MySignals_BLE.initialize_BLE_values();
    }
  }


  //3. BONDING AND CONNECTION CONFIGURATION
  if ((MySignals_BLE.ble_mode_flag == slave_mode) && (MySignals_BLE.bonded_and_connected_flag == 0))
  {

    MySignals_BLE.bonding_correct = 0;
    MySignals_BLE.app_connected_flag = 0;
    MySignals_BLE.bonding_fail = 0;

    /////////////////////

    //TFT message:  "Waiting connections..."
    strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[9])));
    tft.drawString(buffer_tft, 0, 75, 2);


    uint8_t flag = MySignals_BLE.waitEvent(500);

    if (flag == BLE_EVENT_CONNECTION_STATUS)
    {
      MySignals_BLE.app_connected_flag = 1;
    }
    else if (flag == BLE_EVENT_SM_BOND_STATUS)
    {
      if (MySignals_BLE.event[6] == 0x01)
      {
        MySignals_BLE.bonding_correct = 1;
        delay(1000);
      }
    }
    else if (flag == 0)
    {
      // If there are no events, then no one tried to connect
    }
    else if (flag == BLE_EVENT_ATTRIBUTES_VALUE)
    {
      //Already connected
      MySignals_BLE.app_connected_flag = 1;
      MySignals_BLE.bonding_correct = 1;
      MySignals_BLE.bonded_and_connected_flag = 1;
    }
    else
    {
      // Other event received from BLE module
    }

    /////////////////////

    if ((MySignals_BLE.bonding_correct == 1) || MySignals_BLE.app_connected_flag == 1)
    {
      //TFT message:  "Connection detected..."
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[10])));
      tft.drawString(buffer_tft, 0, 90, 2);

      previous = millis();

      while ((MySignals_BLE.bonded_and_connected_flag == 0) && (MySignals_BLE.bonding_fail == 0))
      {
        //	Timeout 30 sg
        if ((millis() - previous) > 30000)
        {
          MySignals_BLE.bonding_fail = 1;
        }

        flag = MySignals_BLE.waitEvent(1000);

        if (flag == 0)
        {
          //Do nothing
        }
        else if (flag == BLE_EVENT_SM_PASSKEY_DISPLAY)
        {

          uint32_t passkey_temp =
            uint32_t(MySignals_BLE.event[5]) +
            uint32_t(MySignals_BLE.event[6]) * 256 +
            uint32_t(MySignals_BLE.event[7]) * 65536 +
            uint32_t(MySignals_BLE.event[8]) * 16777216;

          //TFT message:  "Passkey:";"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[11])));
          tft.drawString(buffer_tft, 0, 105, 2);
          tft.drawNumber(passkey_temp, 50, 105, 2);
        }

        else if (flag == BLE_EVENT_ATTRIBUTES_VALUE)
        {
          //Already connected
          MySignals_BLE.app_connected_flag = 1;
          MySignals_BLE.bonding_correct = 1;
          MySignals_BLE.bonded_and_connected_flag = 1;
        }

        else if (flag == BLE_EVENT_SM_BOND_STATUS)
        {

          if (MySignals_BLE.event[6] == 0x01)
          {
            //Man-in-the-Middle mode correct
            MySignals_BLE.bonding_correct = 1;
          }
        }

        else if (flag == BLE_EVENT_CONNECTION_FEATURE_IND)
        {
          //Do nothing
        }

        else if (flag == BLE_EVENT_CONNECTION_VERSION_IND)
        {
          //Do nothing
        }

        else if (flag == BLE_EVENT_SM_BONDING_FAIL)
        {
          MySignals_BLE.bonded_and_connected_flag = 0;
          MySignals_BLE.bonding_fail = 1;
        }
        else if (flag == BLE_EVENT_CONNECTION_STATUS)
        {

          if (MySignals_BLE.event[5] == 0x03)
          {
            //Connection correct
            MySignals_BLE.app_connected_flag = 1;

          }
        }
        else if (flag == BLE_EVENT_CONNECTION_DISCONNECTED)
        {
          MySignals_BLE.bonded_and_connected_flag = 0;
          MySignals_BLE.bonding_fail = 1;
        }
        else
        {
          //Do nothing
        }


        if (MySignals_BLE.bonding_correct && MySignals_BLE.app_connected_flag)
        {
          //TFT message:  "Connected!"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[12])));
          tft.drawString(buffer_tft, 0, 120, 2);

          //TFT message:  "Sensor list:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[14])));
          tft.drawString(buffer_tft, 0, 135, 2);

          //// SENSORS

          //TFT message:  "Blood pressure:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[27])));
          tft.drawString(buffer_tft, 0, 150, 2);

          MySignals_BLE.bonded_and_connected_flag = 1;
        }

      }


      // Si el bonding ha fallado reiniciar el modulo y recargar pagina
      if (MySignals_BLE.bonding_fail == 1)
      {
        //TFT message:  "Connection failed. Reseting"
        strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[13])));
        tft.drawString(buffer_tft, 0, 120, 2);

        MySignals_BLE.bonded_and_connected_flag = 1;
        MySignals_BLE.hardwareReset();
        delay(100);
        MySignals_BLE.initialize_BLE_values();
      }
    }
  }




  //4. READ SENSOR LIST AND UPDATE VALUES OF SENSORS
  if ((MySignals_BLE.ble_mode_flag == slave_mode) && (MySignals_BLE.app_connected_flag == 1))
  {

    //MySignals.enableSensorUART(BLE);
    //MySignals.pauseInterrupt();
    if (MySignals_BLE.readLocalAttribute(handle_3_0) == 0)
    {

      sprintf(buffer_tft, "%X %X %X  ", MySignals_BLE.attributeValue[1], MySignals_BLE.attributeValue[0], MySignals_BLE.attributeValue[2]);
      tft.drawString(buffer_tft, 100, 135, 2);


      selected_airflow    = MySignals_BLE.attributeValue[0] & 0b00000001;
      selected_gluco_uart = MySignals_BLE.attributeValue[0] & 0b00000010;
      selected_spiro      = MySignals_BLE.attributeValue[0] & 0b00000100;
      selected_gluco_ble  = MySignals_BLE.attributeValue[0] & 0b00001000;
      selected_bp_uart    = MySignals_BLE.attributeValue[0] & 0b00010000;
      selected_bp_ble     = MySignals_BLE.attributeValue[0] & 0b00100000;
      selected_scale_ble  = MySignals_BLE.attributeValue[0] & 0b01000000;
      selected_ecg        = MySignals_BLE.attributeValue[0] & 0b10000000;

      selected_eeg        = MySignals_BLE.attributeValue[1] & 0b00000001;
      selected_emg        = MySignals_BLE.attributeValue[1] & 0b00000010;
      selected_gsr        = MySignals_BLE.attributeValue[1] & 0b00000100;
      selected_position   = MySignals_BLE.attributeValue[1] & 0b00001000;
      selected_snore      = MySignals_BLE.attributeValue[1] & 0b00010000;
      selected_spo2_uart  = MySignals_BLE.attributeValue[1] & 0b00100000;
      selected_spo2_ble   = MySignals_BLE.attributeValue[1] & 0b01000000;
      selected_temp       = MySignals_BLE.attributeValue[1] & 0b10000000;
    }



    if (selected_bp_uart)
    {

      uint8_t bp_vector[6] =
      {
        blood_dias_low, blood_dias_high,
        blood_syst_low, blood_syst_high,
        blood_bpm_low, blood_bpm_high
      };
      SPI.end();

      tft.fillRect(70, 152, 160, 11, ILI9341_BLACK);
      tft.drawNumber(MySignals.bloodPressureData.diastolic, 110, 150, 2);
      tft.drawNumber(MySignals.bloodPressureData.systolic, 140, 150, 2);
      tft.drawNumber(MySignals.bloodPressureData.pulse, 170, 150, 2);

      // Write local attributes
      MySignals.enableSensorUART(BLE);
      //sensor.pauseInterrupt();
      MySignals_BLE.writeLocalAttribute(handle_3_7, bp_vector, 6);
      //sensor.resumeInterrupt();



    }



    // parse the status: 0 not connected; 1 connected; 2 encrypted;
    // 4 connection completed; 8 parameters changed
    MySignals.enableSensorUART(BLE);
    //MySignals.pauseInterrupt();
    if (MySignals_BLE.getStatus(MySignals_BLE.connection_handle) == 0)
    {

      //TFT message:  "Disconnected"
      strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[15])));
      tft.drawString(buffer_tft, 0, 120, 2);

      MySignals_BLE.hardwareReset();
      delay(100);
      MySignals_BLE.initialize_BLE_values();
    }
    //MySignals.resumeInterrupt();
  }




}
