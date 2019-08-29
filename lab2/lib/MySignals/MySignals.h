/*

    Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
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

    Version:           2.0.2
    Design:            David Gascon
    Implementation:    Luis Martin / Victor Boria
*/

// Ensure this library description is only included once
#ifndef MySignals_h
#define MySignals_h


/***********************************************************************************
   Includes
 **********************************************************************************/

#include "Arduino.h"
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "sounddata.h"
#include "Wire.h"
#include "SPI.h"



/***********************************************************************************
   Definitions & Declarations
 **********************************************************************************/
/* Debug mode. Possible values are:
   0 = no debug messages will be printed
   1 = raw data of each sensor will be printed
   2 = all messages of internal communication will be printed
*/
#define MYSIGNALS_DEBUG      0

// Pin definition
#if defined(__AVR_ATmega328P__)
#define SPI_MISO            12
#define SPI_MOSI            11
#define SPI_CLOCK           13
#endif



#if defined(__AVR_ATmega2560__)
#define SPI_MISO            50
#define SPI_MOSI            51
#define SPI_CLOCK           52
#endif



// Sensor UART definitions
#define S0PIN               4
#define S1PIN               5
#define S2PIN               6
#define ENABLE              7

#define EXP_3G_POWER    0
#define EXP_ESP8266_POWER 1
#define EXP_BT_POWER    2
#define EXP_BT_KEY      3
#define EXP_ADC_CS      4
#define EXP_BLE_FLOW_CONTROL 5
#define EXP_BLE_POWER   6
#define EXP_ROVING_POWER  7

#define DEFAULTSENSOR     0
#define GLUCOMETER        1
#define BLOODPRESSURE     2
#define PULSIOXIMETER     3
#define SPIROMETER        4
#define EEG               5
#define BLE               6
#define EXPANSION         7
#define WIFI_ESP8266      8
#define PULSIOXIMETER_MICRO   9


#define TFT_CS  10
#define TFT_DC  9

#define SD_CS 8

#define ADC_EMG               0
#define ADC_ECG               1
#define ADC_AIRFLOW           2
#define ADC_GSR               3
#define ADC_TEMPERATURE       4
#define ADC_SNORE             5
#define ADC_6                 6
#define ADC_7                 7

// Format definitions
#define CONDUCTANCE           1
#define RESISTANCE            2
#define VOLTAGE               3
#define DATA                  4
#define TEMPERATURE           5
#define PULSE                 6
#define SPO2                  7

// Snore definitions
#define SAMPLERATE            8000
#define SPEAKER               3
#define BUTTON                2

#define ALARM                 1
#define SOUNDSAMPLE1          2
#define SOUNDSAMPLE2          3

#define EXPANDER              56

//Spirometer
#define MAX_SPIRO_MEASURES 2

#define soundalarm_length 4000


const char msg_0[] PROGMEM = "Welcome to MySignals";
const char msg_1[] PROGMEM = "BLE init ok";
const char msg_2[] PROGMEM = "BLE init fail";
const char msg_3[] PROGMEM = "Slave mode ok";
const char msg_4[] PROGMEM = "Slave mode fail";
const char msg_5[] PROGMEM = "Bondable mode ok";
const char msg_6[] PROGMEM = "Bondable mode fail";
const char msg_7[] PROGMEM = "SM parameters ok";
const char msg_8[] PROGMEM = "SM parameters fail";
const char msg_9[] PROGMEM = "Waiting connections...";
const char msg_10[] PROGMEM = "Connection detected...";
const char msg_11[] PROGMEM = "Passkey:";
const char msg_12[] PROGMEM = "Connected!   ";
const char msg_13[] PROGMEM = "Connection failed. Reseting";
const char msg_14[] PROGMEM = "Sensor list:";
const char msg_15[] PROGMEM = "Disconnected";
const char msg_16[] PROGMEM = "OK";
const char msg_17[] PROGMEM = "Fail";
const char msg_18[] PROGMEM = "WiFi:";
const char msg_19[] PROGMEM = "BT2.0:";
const char msg_20[] PROGMEM = "Airflow:";
const char msg_21[] PROGMEM = "Temperature:";
const char msg_22[] PROGMEM = "Position:";
const char msg_23[] PROGMEM = "GSR:";
const char msg_24[] PROGMEM = "ECG:";
const char msg_25[] PROGMEM = "EMG:";
const char msg_26[] PROGMEM = "Snore:";
const char msg_27[] PROGMEM = "Blood pressure:";
const char msg_28[] PROGMEM = "SPO2:";
const char msg_29[] PROGMEM = "Glucometer:";
const char msg_30[] PROGMEM = "Spirometer:";
const char msg_31[] PROGMEM = "Touch:";
const char msg_32[] PROGMEM = "EEPROM:";
const char msg_33[] PROGMEM = "Expansion board:";

const char* const  table_MISC[] PROGMEM =
{
  msg_0,  msg_1,  msg_2,  msg_3,  msg_4,  msg_5,  msg_6,  msg_7,  msg_8,  msg_9,
  msg_10, msg_11, msg_12, msg_13, msg_14, msg_15, msg_16, msg_17, msg_18, msg_19,
  msg_20, msg_21, msg_22, msg_23, msg_24, msg_25, msg_26, msg_27, msg_28, msg_29,
  msg_30, msg_31, msg_32, msg_33,
};


/***********************************************************************************
   Sensor Class
 **********************************************************************************/

class MySignalsClass
{
    //*******************************************************************************
    // Private Methods                                                                 *
    //**********************************************************************************

  private:

    //***************************************************************************
    // Private Functions                                                        *
    //***************************************************************************
    //int _clockpin, _mosipin, _misopin, _cspin;
    int _clockpin, _mosipin, _misopin;

    //! Assigns a value depending on body position

    float median(float* array, uint8_t length);


    //! Assigns a value depending on body position

    uint8_t bodyPosition(void);


    //! Assigns a value depending on body position
    byte readOneByte(void);


    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void readSpiroMeasure(uint8_t _spir_measures);

    //***************************************************************************
    // Private Variables                                                        *
    //***************************************************************************
    //! Airflow BPM

    unsigned long airflowPeakCounter;
    unsigned long airflowPeakPointer [2];
    int airflowDataBalanced[4];
    int airflowDataPrevious[3];
    uint16_t airflowDataMin;
    uint16_t airflowDataMax;
    bool airflowFlagState;


    //! ECG BPM

    unsigned long ECGPeakCounter;
    unsigned long ECGPeakPointer [2];
    int ECGDataBalanced[4];
    int ECGDataPrevious[3];
    int ECGDerDataPrevious[3];
    uint8_t ECGDataMin;
    uint8_t ECGDataMax;
    bool ECGFlagState;


    //! EMG BPM

    unsigned long EMGPeakCounter;
    unsigned long EMGPeakPointer [2];
    int EMGDataBalanced[4];
    int EMGDataPrevious[3];
    uint16_t EMGDataMin;
    uint16_t EMGDataMax;
    bool EMGFlagState;


    //! Snore BPM

    unsigned long snorePeakCounter;
    unsigned long snorePeakPointer [2];
    int snoreDataBalanced[4];
    int snoreDataPrevious[3];
    uint16_t snoreDataMin;
    uint16_t snoreDataMax;
    bool snoreFlagState;


    //! It stores the systolic pressure value
    int systolic;

    //! It stores the diastolic pressure value
    int diastolic;

    //!It stores the number of data of the glucometer
    uint8_t length;




    //*******************************************************************************
    // Public Methods                                                               *
    //*******************************************************************************

  public:

    //***************************************************************************
    //Constructor of the class                                                  *
    //***************************************************************************

    //! Class constructor.
    MySignalsClass(void);

    //***************************************************************************
    // Public Functions                                                         *
    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void begin();

    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    byte expanderRead();

    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void expanderWrite(byte data);

    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    int readADC(uint8_t adcnum);


    //***************************************************************************


    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void printWave(uint16_t air);


    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void initSensorUART(uint8_t number);
    void initSensorUART(void);

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void enableSensorUART(uint8_t number);

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void disableSensorUART(void);


    //***************************************************************************

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void initInterrupt(uint16_t sampleRate);

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void pauseInterrupt(void);

    //!  Prints air flow wave form in the serial monitor
    /*!
      \param int air : analogic value to print
      \return void
    */
    void resumeInterrupt(void);


    //***************************************************************************



    void enableMuxUART(void);
    void disableMuxUART(void);


    void serialFlush();


    // Debug printing functions
    void print(char * msg);
    void println(char * msg);
    void println();
    void PrintHex8(uint8_t *data, uint8_t length);
    void PrintHex16(uint16_t *data, uint8_t length);


    //***************************************************************************

    //! Initializes the position sensor and configure some values
    /*!
      \param void
      \return void
    */
    void initBodyPosition(void);

    //! Returns the body position
    /*!
      \param void
      \return uint8_t : the position of the pacient
          1 == Supine position
          2 == Left lateral decubitus
          3 == Rigth lateral decubitus
          4 == Prone position
          5 == Stand or sit position
    */
    uint8_t getBodyPosition(void);

    //! Prints the current body position
    /*!
      \param uint8_t position : the current body position
      \return void
    */
    void printPosition(uint8_t position);

    //! Prints the current body position
    /*!
      \param uint8_t position : the current body position
      \return void
    */
    void getAcceleration();


    //***************************************************************************

    //! Returns an analogic value to represent the air flow
    /*!
      \param void
      \return int : The value (0-1023) read from the analogic in
    */
    float getAirflow(uint8_t format);
    float getAirflow(void);

    //! Returns an analogic value to represent the Electrocardiography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getCalibratedAirflow(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);

    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    void airflowPPM(void);


    //***************************************************************************

    //! Returns the value of skin conductance
    /*!
      \param void
      \return float : The skin conductance value
    */

    float getGSR(uint8_t format);
    float getGSR(void);

    //! Returns an analogic value to represent the Electrocardiography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getCalibratedGSR(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);


    //***************************************************************************

    //! Returns the corporal temperature
    /*!
      \param void
      \return float : The corporal temperature value
    */

    float getTemperature(void);

    //! Returns the corporal temperature calibrated
    /*!
      \param void
      \return float : The corporal temperature value
    */

    float getCalibratedTemperature(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);


    //***************************************************************************

    //! Returns an analogic value to represent the Electrocardiography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getECG(uint8_t format);
    float getECG(void);

    //! Returns an analogic value to represent the Electrocardiography

    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getCalibratedECG(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);

    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    void ECGBPM(void);


    //***************************************************************************

    //! Returns an analogic value to represent the Electromyography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getEMG(uint8_t format);
    float getEMG(void);

    //! Returns an analogic value to represent the Electrocardiography

    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getCalibratedEMG(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);

    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    void EMGCPM(void);


    //***************************************************************************

    //! Initializes the position sensor and configure some values
    /*!
      \param void
      \return void
    */
    void initSnore();

    //! Returns an analogic value to represent the Electromyography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */


    //! Returns an analogic value to represent the Electromyography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getSnore(uint8_t format);
    float getSnore(void);

    //! Returns an analogic value to represent the Electrocardiography
    /*!
      \param void
      \return float : The analogic value (0-5V)
    */

    float getCalibratedSnore(uint8_t samples, uint16_t delaySample, float offset, uint8_t format);

    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    void snoreSPM(void);



    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    void playSoundSnore(uint16_t frequency, uint16_t duration);

    
    void playAudioSnore();
    
    void stopAudioSnore();
    
    
    //!  Prints air flow BPM in the serial monitor
    /*!
      \param void
      \return void
    */
    bool getButtonSnore(void);


    //***************************************************************************

    //! Initializes the pulsioximeter sensor and configure some values

    /*!
      \param void
      \return void
    */

    uint8_t getPulsioximeterMini();
    uint8_t getPulsioximeterMicro();
    uint8_t getStatusPulsioximeterGeneral();
    void spo2_micro_send_init_frame();
    
    
    bool spo2_micro_detected;
    bool spo2_mini_detected;
    //***************************************************************************



    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    uint8_t initEEG(void);

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    uint8_t getEEG();


    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    uint8_t getStatusBP();


    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    bool getBloodPressure(void);




    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void getGlucose(void);


    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    uint8_t getStatusSpiro();


    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void getSpirometer();




    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    bool deleteSpiroMeasures();


    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void getScaleBLE(void);


    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void getPulsioximeterBLE(void);



    //***************************************************************************
    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    void getBloodPressureBLE(void);



    //***************************************************************************

    //!  Prints air flow BPM in the serial monitor
    /*!
        \param void
        \return void
    */
    bool getGlucoseBLE(void);

    void cleanEEPROM(unsigned int eeaddress, unsigned int num_chars);
	void writeByteEEPROM(unsigned int eeaddress, byte data);
	byte readByteEEPROM(unsigned int eeaddress);
	void writeEEPROM(unsigned int eeaddress, char* data);
	void writeEEPROM(unsigned int eeaddress, char* data, unsigned int data_len);


    //***************************************************************************
    // Public Variables                             *
    //***************************************************************************


    //!Interrupt  variables
    bool airflowFlagPPM;
    bool ECGFlagBPM;
    bool EMGFlagCPM;
    bool snoreFlagSPM;

    uint16_t interruptCounter;
    uint16_t interruptNCCounter;
    uint16_t interruptNCCounter2;
    uint8_t airflow_ppm;


    //!Airflow parameters
    uint8_t airflowDataPPM;
    uint8_t airflowDataPPMBalanced;

    uint8_t ECGDataBPM;
    uint16_t ECGDataBPMBalanced;
    uint16_t ECGDataBPMBalancedPrev;
    uint8_t ECGCorrectMeasure;
    uint16_t ECGDerData;

    uint8_t EMGDataCPM;
    uint8_t EMGDataCPMBalanced;

    uint8_t snoreDataSPM;
    uint8_t snoreDataSPMBalanced;

    //!Accelerometer axis parameters
    float x_data;
    float y_data;
    float z_data;

    //!Struct to store data of the GSR sensor
    struct GSRDataStruct
    {
      float resistance;
      float conductance;
      float voltage;
    };
    GSRDataStruct GSRData;

    uint16_t gsr_raw;


    //! Snore alarm parameters
    volatile uint16_t sample;
    byte lastSample;
    uint8_t audioNumber;

	

    //!Struct to store data of the pulsioximeter sensor
    struct pulsioximeterDataStruct
    {
      uint8_t BPM;
      uint8_t O2;
    };
    pulsioximeterDataStruct pulsioximeterData;


    //!Struct to store data of the GSR sensor
    uint8_t spir_measures;

    struct spirometerDataStruct
    {
      uint8_t spir_year;
      uint8_t spir_month;
      uint8_t spir_day;
      uint8_t spir_hour;
      uint8_t spir_minutes;
      uint16_t spir_pef;
      uint16_t spir_fev;  //Dividir entre 100 para obtener litros
    };
    spirometerDataStruct  spirometerData[1];


    //!Struct to store data of the blood pressure sensor
    struct bloodPressureDataVector
    {
      uint16_t systolic;
      uint16_t diastolic;
      uint16_t pulse;
    };
    bloodPressureDataVector bloodPressureData;

    //!Struct to store data of the glucometer
    uint8_t glucoseLength;

    //!Struct to store data of the glucometer.
    struct glucoseDataVector
    {
      uint8_t year;
      uint8_t month;
      uint8_t day;
      uint8_t hour;
      uint8_t minutes;
      uint8_t glucose;
      uint8_t meridian;
    };

    //!Vector to store the glucometer measures and dates.
    glucoseDataVector glucometerData[1];

    uint8_t expanderState;

};





/***********************************************************************************
   Extern Class name
 **********************************************************************************/

extern MySignalsClass MySignals;


#endif

