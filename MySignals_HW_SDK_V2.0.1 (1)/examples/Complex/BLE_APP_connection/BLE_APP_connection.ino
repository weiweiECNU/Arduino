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
uint8_t sensor_list_mode;

uint8_t spir_measure_individual;
uint8_t gluco_measure_individual;

uint8_t last_measure_hour_spiro;
uint8_t last_measure_minutes_spiro;
uint8_t last_measure_number_spiro;

uint8_t last_measure_hour_gluco;
uint8_t last_measure_minutes_gluco;
uint8_t last_measure_number_gluco;



void setup()
{
  MySignals.begin();

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  //TFT message: Welcome to MySignals
  strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[0])));
  tft.drawString(buffer_tft, 0, 0, 2);

  Serial.begin(115200);

  MySignals.initSensorUART();

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
        //  Timeout 30 sg
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

          //// SENSORES

          //TFT message:  "Airflow:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[20])));
          tft.drawString(buffer_tft, 0, 150, 2);

          //TFT message:  "Temperature:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[21])));
          tft.drawString(buffer_tft, 0, 165, 2);

          //TFT message:  "Position:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[22])));
          tft.drawString(buffer_tft, 0, 180, 2);

          //TFT message:  "GSR:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[23])));
          tft.drawString(buffer_tft, 0, 195, 2);

          //TFT message:  "ECG:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[24])));
          tft.drawString(buffer_tft, 0, 210, 2);

          //TFT message:  "EMG:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[25])));
          tft.drawString(buffer_tft, 0, 225, 2);

          //TFT message:  "Snore:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[26])));
          tft.drawString(buffer_tft, 0, 240, 2);

          //TFT message:  "SPO2:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[28])));
          tft.drawString(buffer_tft, 0, 255, 2);

          //TFT message:  "Glucometer:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[29])));
          tft.drawString(buffer_tft, 0, 270, 2);

          //TFT message:  "Spirometer:"
          strcpy_P((char*)buffer_tft, (char*)pgm_read_word(&(table_MISC[30])));
          tft.drawString(buffer_tft, 0, 285, 2);

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

    MySignals.enableSensorUART(BLE);
    //MySignals.pauseInterrupt();
    if (MySignals_BLE.readLocalAttribute(handle_3_0) == 0)
    {

      sprintf(buffer_tft, "%X %X %X  ", MySignals_BLE.attributeValue[1], MySignals_BLE.attributeValue[0], MySignals_BLE.attributeValue[2]);
      tft.drawString(buffer_tft, 100, 135, 2);

      sensor_list_mode = MySignals_BLE.attributeValue[2];

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
    //MySignals.resumeInterrupt();


    if (selected_gluco_uart)
    {
      MySignals.enableSensorUART(GLUCOMETER);

      delay(10);
      MySignals.getGlucose();

      if (
        ((MySignals.glucoseLength > 0)
         && (MySignals.glucometerData[0].hour != last_measure_hour_gluco)
         && (MySignals.glucometerData[0].minutes != last_measure_minutes_gluco)
         && (MySignals.glucometerData[0].hour != 255))
        ||
        ((MySignals.glucoseLength > 0)
         && (last_measure_number_gluco != MySignals.glucoseLength)
         && (MySignals.glucometerData[0].hour != 255))
      )
      {
        last_measure_hour_gluco = MySignals.glucometerData[0].hour;
        last_measure_minutes_gluco = MySignals.glucometerData[0].minutes;
        last_measure_number_gluco = MySignals.glucoseLength;

        if (MySignals.glucometerData[MySignals.glucoseLength - 1].glucose != 0)
        {
          //Last measure
          uint8_t gluco_vector[9] =
          {
            MySignals.glucometerData[MySignals.glucoseLength - 1].year,
            MySignals.glucometerData[MySignals.glucoseLength - 1].month,
            MySignals.glucometerData[MySignals.glucoseLength - 1].day,
            MySignals.glucometerData[MySignals.glucoseLength - 1].hour,
            MySignals.glucometerData[MySignals.glucoseLength - 1].minutes,
            MySignals.glucometerData[MySignals.glucoseLength - 1].glucose,
            MySignals.glucometerData[MySignals.glucoseLength - 1].meridian,
            MySignals.glucoseLength,
            MySignals.glucoseLength
          };

          tft.fillRect(70, 272, 100, 11, ILI9341_BLACK);
          tft.drawNumber(MySignals.glucometerData[MySignals.glucoseLength - 1].glucose, 80, 270, 2);

          SPI.end();
          //MySignals.pauseInterrupt();
          // Write local attributes
          MySignals.enableSensorUART(BLE);
          MySignals_BLE.writeLocalAttribute(handle_3_9, gluco_vector, 9);
          //MySignals.resumeInterrupt();
        }
      }
    }



    if (selected_spiro)
    {
      MySignals.enableSensorUART(SPIROMETER);
      if (MySignals.getStatusSpiro() == 1)
      {
        delay(10);
        MySignals.getSpirometer();

        if (
          ((MySignals.spir_measures > 0)
           && (MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef < 1000)
           && (MySignals.spirometerData[0].spir_hour != last_measure_hour_spiro)
           && (MySignals.spirometerData[0].spir_minutes != last_measure_minutes_spiro))
          ||
          ((MySignals.spir_measures > 0)
           && (MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef < 1000)
           && (last_measure_number_spiro != MySignals.spir_measures))
        )
        {

          last_measure_hour_spiro = MySignals.spirometerData[0].spir_hour;
          last_measure_minutes_spiro = MySignals.spirometerData[0].spir_minutes;
          last_measure_number_spiro = MySignals.spir_measures;


          if (MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef < 1000)
          {
            uint8_t spir_pef_low = MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef & 0b0000000011111111;
            uint8_t spir_pef_high = (MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef & 0b1111111100000000) / 256;

            uint8_t spir_fev_low = MySignals.spirometerData[MySignals.spir_measures - 1].spir_fev & 0b0000000011111111;
            uint8_t spir_fev_high = (MySignals.spirometerData[MySignals.spir_measures - 1].spir_fev & 0b1111111100000000) / 256;

            //Last measure
            uint8_t spir_vector[11] =
            {
              MySignals.spirometerData[MySignals.spir_measures - 1].spir_year,
              MySignals.spirometerData[MySignals.spir_measures - 1].spir_month,
              MySignals.spirometerData[MySignals.spir_measures - 1].spir_day,
              MySignals.spirometerData[MySignals.spir_measures - 1].spir_hour,
              MySignals.spirometerData[MySignals.spir_measures - 1].spir_minutes,
              spir_pef_low,
              spir_pef_high,
              spir_fev_low,
              spir_fev_high,
              MySignals.spir_measures,
              MySignals.spir_measures
            };


            tft.fillRect(70, 287, 100, 11, ILI9341_BLACK);
            tft.drawNumber(MySignals.spirometerData[MySignals.spir_measures - 1].spir_pef, 80, 285, 2);


            SPI.end();
            //MySignals.pauseInterrupt();
            // Write local attributes
            MySignals.enableSensorUART(BLE);
            MySignals_BLE.writeLocalAttribute(handle_3_10, spir_vector, 11);
            //MySignals.resumeInterrupt();
          }
        }
      }
    }

    if (selected_spo2_uart)
    {
      MySignals.enableSensorUART(PULSIOXIMETER);

      if (MySignals.spo2_micro_detected == 0 && MySignals.spo2_mini_detected == 0)
      {
        uint8_t statusPulsioximeter = MySignals.getStatusPulsioximeterGeneral();

        if (statusPulsioximeter == 1)
        {
          MySignals.spo2_mini_detected = 0;
          MySignals.spo2_micro_detected = 1;

          //tft.drawString("Micro detected", 0, 100, 2);
        }
        
        else if (statusPulsioximeter == 2)
        {
          MySignals.spo2_mini_detected = 1;
          MySignals.spo2_micro_detected = 0;

          //tft.drawString("Mini detected", 0, 100, 2);
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
          tft.fillRect(70, 257, 100, 11, ILI9341_BLACK);
          tft.drawNumber(MySignals.pulsioximeterData.BPM, 80, 255, 2);
          tft.drawNumber(MySignals.pulsioximeterData.O2, 150, 255, 2);;

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

/*
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
*/
      
    }

    if (selected_airflow)
    {
      // PPM flag initialization
      //MySignals.airflowFlagPPM = 1;

      //MySignals.airflow_ppm = MySignals.airflowDataPPMBalanced;
      SPI.end();
      uint16_t airflow_raw = (uint16_t)MySignals.getAirflow(DATA);

      uint8_t airflow_raw_low = airflow_raw & 0b0000000011111111;
      uint8_t airflow_raw_high = (airflow_raw & 0b1111111100000000) / 256;


      tft.fillRect(70, 152, 100, 11, ILI9341_BLACK);
      tft.drawNumber(airflow_raw, 80, 150, 2);
      tft.drawNumber(MySignals.airflow_ppm, 150, 150, 2);

      uint8_t airflow_vector[3] =
      {
        airflow_raw_low, airflow_raw_high, MySignals.airflow_ppm
      };

      MySignals.enableSensorUART(BLE);
      //MySignals.pauseInterrupt();
      MySignals_BLE.writeLocalAttribute(handle_3_5, airflow_vector, 3);
      //MySignals.resumeInterrupt();
    }


    if (selected_temp)
    {

      SPI.end();
      uint16_t temp_dummy =  MySignals.getTemperature() * 100;
      uint8_t temp_low = temp_dummy & 0b0000000011111111;
      uint8_t temp_high = (temp_dummy & 0b1111111100000000) / 256;


      tft.fillRect(70, 167, 100, 11, ILI9341_BLACK);
      tft.drawFloat(float(temp_dummy / 100.0), 1, 80, 165, 2);



      uint8_t temp_vector[2] =
      {
        temp_low, temp_high
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_2, temp_vector, 2);

    }


    if (selected_position)
    {
      SPI.end();

      uint8_t position = MySignals.getBodyPosition();

      tft.fillRect(70, 182, 100, 11, ILI9341_BLACK);
      tft.drawNumber(position, 80, 180, 2);

      uint8_t position_vector[4] =
      {
        position, MySignals.x_data, MySignals.y_data, MySignals.z_data
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_1, position_vector, 4);

    }




    if (selected_gsr)
    {
      SPI.end();

      MySignals.getGSR();

      tft.fillRect(70, 197, 100, 11, ILI9341_BLACK);
      tft.drawNumber(MySignals.gsr_raw, 80, 195, 2);

      uint8_t gsr_raw_low = MySignals.gsr_raw & 0b0000000011111111;
      uint8_t gsr_raw_high = (MySignals.gsr_raw & 0b1111111100000000) / 256;


      uint8_t gsr_vector[2] =
      {
        gsr_raw_low, gsr_raw_high
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_6, gsr_vector, 2);

    }


    if (selected_ecg)
    {
      SPI.end();

      MySignals.ECGFlagBPM = 1;

      uint16_t ecg_raw = MySignals.getECG();

      tft.fillRect(70, 212, 100, 11, ILI9341_BLACK);
      tft.drawNumber(ecg_raw, 80, 210, 2);

      uint8_t ecg_raw_low = ecg_raw & 0b0000000011111111;
      uint8_t ecg_raw_high = (ecg_raw & 0b1111111100000000) / 256;

      uint8_t ecg_vector[3] =
      {
        ecg_raw_low, ecg_raw_high, MySignals.ECGDataBPMBalanced
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_4, ecg_vector, 3);

    }

/*
    if (selected_emg)
    {
      SPI.end();
      //MySignals.EMGFlagCPM = 1;
      uint16_t emg_raw = MySignals.getEMG();

      uint8_t emg_raw_low = emg_raw & 0b0000000011111111;
      uint8_t emg_raw_high = (emg_raw & 0b1111111100000000) / 256;

      tft.fillRect(70, 227, 100, 11, ILI9341_BLACK);
      tft.drawNumber(emg_raw, 80, 225, 2);

      uint8_t emg_vector[3] =
      {
        emg_raw_low, emg_raw_high, MySignals.EMGDataCPMBalanced
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_3, emg_vector, 3);

    }
*/
    
/*
    if (selected_snore)
    {
      SPI.end();

      //MySignals.snoreFlagSPM = 1;
      uint16_t snore_raw = MySignals.getSnore();

      uint8_t snore_raw_low = snore_raw & 0b0000000011111111;
      uint8_t snore_raw_high = (snore_raw & 0b1111111100000000) / 256;

      tft.fillRect(70, 242, 100, 11, ILI9341_BLACK);
      tft.drawNumber(snore_raw, 80, 240, 2);

      uint8_t snore_vector[3] =
      {
        snore_raw_low, snore_raw_high, MySignals.snoreDataSPMBalanced
      };

      MySignals.enableSensorUART(BLE);
      MySignals_BLE.writeLocalAttribute(handle_3_11, snore_vector, 3);

    }

*/



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
