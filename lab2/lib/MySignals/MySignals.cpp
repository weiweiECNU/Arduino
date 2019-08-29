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

// include this library's description file
#include <MySignals.h>


/***********************************************************************************
   Constructors
 ***********************************************************************************/

//!******************************************************************************
//!   Name: MySignalsClass                                                      *
//!   Description: Constructor of the sensor class                              *
//!******************************************************************************
MySignalsClass::MySignalsClass(void)
{
  //_cspin = CS_PIN;
  _clockpin = SPI_CLOCK;
  _mosipin = SPI_MOSI;
  _misopin = SPI_MISO;

  //pinMode(_cspin, OUTPUT);
  pinMode(_clockpin, OUTPUT);
  pinMode(_mosipin, OUTPUT);
  pinMode(_misopin, INPUT);

  pinMode(SD_CS, OUTPUT);
  
  expanderState = B10100001;
  
	
}




/************************************************************************************
   Sensors Class
 ***********************************************************************************/


//*******************************************************************************
// Public Methods                               *
//*******************************************************************************

//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::begin()
{
  Wire.begin();
  SPI.begin();

  digitalWrite(SD_CS, HIGH); //Disable SD CS

  //Variable to store the value of the expander digital pins, initialized with default value
  //B10100001 default values,disable all SPI CS, turn off all powers
  expanderWrite(expanderState);

}
/*******************************************************************************/

//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::expanderWrite(byte data)
{
  Wire.beginTransmission(EXPANDER);
  Wire.write(data);
  Wire.endTransmission();
}

/*******************************************************************************/

//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

byte MySignalsClass::expanderRead()
{
  byte data;
  Wire.requestFrom(EXPANDER, 1);
  if (Wire.available())
  {
    data = Wire.read();
  }
  return data;
}

/*******************************************************************************/

int MySignalsClass::readADC(uint8_t adcnum)
{
  //SPI.end();
   
  digitalWrite(SD_CS, HIGH);                         //Disable SD SPI
  bitSet(MySignals.expanderState, EXP_ADC_CS);       //Enable ADC SPI

  MySignals.expanderWrite(MySignals.expanderState);



  if ((adcnum > 7) || (adcnum < 0)) return -1; // Wrong adc address return -1

  //digitalWrite(_cspin, HIGH);
  digitalWrite(_clockpin, LOW); //  # start clock low
  //digitalWrite(_cspin, LOW); //     # bring CS low

  int commandout = adcnum;
  commandout |= 0x18; //  # start bit + single-ended bit
  commandout <<= 3; //    # we only need to send 5 bits here

  for (int i = 0; i < 5; i++)
  {
    if (commandout & 0x80)
      digitalWrite(_mosipin, HIGH);
    else
      digitalWrite(_mosipin, LOW);

    commandout <<= 1;
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);

  }

  int adcout = 0;
  // read in one empty bit, one null bit and 10 ADC bits
  for (int i = 0; i < 12; i++)
  {
    digitalWrite(_clockpin, HIGH);
    digitalWrite(_clockpin, LOW);
    adcout <<= 1;
    if (digitalRead(_misopin))
      adcout |= 0x1;
  }
  //digitalWrite(_cspin, HIGH);

  adcout >>= 1; //      # first bit is 'null' so drop it



  bitClear(MySignals.expanderState, EXP_ADC_CS);    //Disable ADC SPI
  MySignals.expanderWrite(MySignals.expanderState);

  SPI.end();

  return adcout;
}


/*
  int MySignalsClass::readADC(uint8_t ADCPosition)
  {


  MySignals.expanderWrite(B00010000);

  SPI.begin();

  // disable global interrupts
  //cli();

  SPI.transfer((ADCPosition << 2) + 0b1100000);
  int value = SPI.transfer(0) << 2;
  value += (SPI.transfer(0) & 0b11000000) >> 6;

  // enable global interrupts
  //sei();


  MySignals.expanderWrite(B00000000);

  return value;
  }
*/
/*******************************************************************************/


//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::printWave(uint16_t air)
{
  for (int i = 0; i < (air / 5) ; i ++)
  {
    Serial.print(F(".."));
  }

  Serial.print(F(".."));
  Serial.print(F("\n"));
  delay(25);
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::initInterrupt(uint16_t sampleRate)
{

  // Configure Timer 1 interrupt

  // disable global interrupts
  cli();

  // set entire TCCR1A register to 0
  TCCR1A = 0;
  // same for TCCR1B
  TCCR1B = 0;

  sampleRate = map(sampleRate, 1, 20, 2000, 40000);

  // set compare match register to desired timer count to 10 ms
  OCR1A = sampleRate;
  // set compare match register to desired timer count to 20 ms
  //OCR1A = 40000;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler:
  // TCCR1B |= (1 << CS10);
  // Set CS11 bits for 8 prescaler:
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  // disable compare interrupt:
  //TIMSK1 = 0;
#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println(F("-----------------> SENSOR CLASS CONFIGURATION"));
  Serial.println(F("Timer 1 configuration"));
  Serial.println();
#endif
  //interrupts();

  interruptCounter = 0;
  // enable global interrupts
  sei();

}

/*******************************************************************************/


//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::pauseInterrupt(void)
{
  // disable global interrupts
  cli();

  // disable compare interrupt:
  TIMSK1 = 0;

#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println(F("-----------------> SENSOR CLASS CONFIGURATION"));
  Serial.println(F("Timer 1 pause"));
  Serial.println();
#endif

  // enable global interrupts
  sei();

}

/*******************************************************************************/

//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowWave();                    *
//!******************************************************************************

void MySignalsClass::resumeInterrupt(void)
{
  // disable global interrupts
  cli();

  // enable compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println(F("-----------------> SENSOR CLASS CONFIGURATION"));
  Serial.println(F("Timer 1 resume"));
  Serial.println();
#endif

  // enable global interrupts
  sei();

}

/*******************************************************************************/



//!******************************************************************************
//!   Name: printWave()                                                         *
//!   Description: Prints an analog value to wave form in the serial monitor    *
//!   Param : int air with the analogic value 0-1024                            *
//!   Returns: void                                                             *
//!   Example: MySignals.airflowWave();                                         *
//!******************************************************************************

void MySignalsClass::initSensorUART(void)
{
  pinMode(S0PIN, OUTPUT);
  pinMode(S1PIN, OUTPUT);
  pinMode(S2PIN, OUTPUT);
  pinMode(ENABLE, OUTPUT);

  digitalWrite(S0PIN, HIGH);
  digitalWrite(S1PIN, HIGH);
  digitalWrite(S2PIN, HIGH);
  digitalWrite(ENABLE, HIGH);

#if MYSIGNALS_DEBUG > 1
  Serial.println();
  Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
  Serial.println(F("Default - UART disabled"));
  Serial.println();
#endif
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MySignals.airflowWave();                   *
//!******************************************************************************

void MySignalsClass::enableSensorUART(uint8_t number)
{

  switch (number)
  {
    case GLUCOMETER:
      // Glucometer
      digitalWrite(ENABLE, HIGH);
      delay(10);
      Serial.begin(1200);
      delay(100);
      while (Serial.read() >= 0);
      digitalWrite(S0PIN, LOW);
      digitalWrite(S1PIN, LOW);
      digitalWrite(S2PIN, LOW);
      digitalWrite(ENABLE, LOW);
      delay(30);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Glucometer UART enabled"));
      Serial.println();
#endif
      break;

    case BLOODPRESSURE:
      // Blood Pressure
      digitalWrite(ENABLE, HIGH);
      delay(10);
      Serial.begin(19200);
      delay(10);
      while (Serial.read() >= 0);
      digitalWrite(S0PIN, HIGH);
      digitalWrite(S1PIN, LOW);
      digitalWrite(S2PIN, LOW);
      digitalWrite(ENABLE, LOW);
      delay(10);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Blodd pressure UART enabled"));
      Serial.println();
#endif
      break;

    case PULSIOXIMETER:
      // SPO2
      digitalWrite(ENABLE, HIGH);
      delay(20);

      Serial.begin(19200);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, LOW);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, LOW);
      delay(10);
      digitalWrite(ENABLE, LOW);

      delay(30);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Pulsioximeter UART enabled"));
      Serial.println();
#endif
      break;
      
    case PULSIOXIMETER_MICRO:
      // SPO2
      digitalWrite(ENABLE, HIGH);
      delay(20);

      Serial.begin(115200);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, LOW);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, LOW);
      delay(10);
      digitalWrite(ENABLE, LOW);

      delay(30);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Pulsioximeter UART enabled"));
      Serial.println();
#endif
      break;
      
    case SPIROMETER:

      digitalWrite(ENABLE, HIGH);
      delay(20);

      Serial.begin(9600);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, HIGH);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, LOW);
      delay(10);
      digitalWrite(ENABLE, LOW);
      delay(30);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Spirometer UART enabled"));
      Serial.println();
#endif
      break;

    case EEG:
      // EEG

      digitalWrite(ENABLE, HIGH);
      delay(20);

      //Serial.begin(57600);
      Serial.begin(38400);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, LOW);
      digitalWrite(S1PIN, LOW);
      digitalWrite(S2PIN, HIGH);
      delay(10);
      digitalWrite(ENABLE, LOW);

      delay(30);

#if MYHOSPITAL_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("EEG UART enabled"));
      Serial.println();
#endif

      break;

    case BLE:
      digitalWrite(ENABLE, HIGH);
      delay(20);

      Serial.begin(115200);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, HIGH);
      digitalWrite(S1PIN, LOW);
      digitalWrite(S2PIN, HIGH);
      delay(10);
      digitalWrite(ENABLE, LOW);

      //delay(500);
      delay(10);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Spirometer UART enabled"));
      Serial.println();
#endif
      break;

    case WIFI_ESP8266:
      digitalWrite(ENABLE, HIGH);
      delay(20);

      Serial.begin(115200);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, HIGH);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, HIGH);
      delay(10);
      digitalWrite(ENABLE, LOW);

      delay(1000);

      break;

    case EXPANSION:
      digitalWrite(ENABLE, HIGH);
      delay(20);
      
      //Serial.begin(115200);
      while (Serial.read() >= 0);

      digitalWrite(S0PIN, LOW);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, HIGH);
      delay(10);
      digitalWrite(ENABLE, LOW);

      delay(1000);
      break;

    default:
      // if nothing else matches, do the default
      digitalWrite(S0PIN, HIGH);
      digitalWrite(S1PIN, HIGH);
      digitalWrite(S2PIN, HIGH);
      digitalWrite(ENABLE, HIGH);

#if MYSIGNALS_DEBUG > 1
      Serial.println();
      Serial.println(F("-----------------> SENSOR UART CONFIGURATION"));
      Serial.println(F("Default UART disabled"));
      Serial.println();
#endif
      break;
  }
}

/*******************************************************************************/

//!******************************************************************************
//!   Name: printWave()                           *
//!   Description: Prints an analog value to wave form in the serial monitor  *
//!   Param : int air with the analogic value 0-1024              *
//!   Returns: void                             *
//!   Example: MySignals.airflowWave();                   *
//!******************************************************************************

void MySignalsClass::disableSensorUART(void)
{
  digitalWrite(S0PIN, HIGH);
  digitalWrite(S1PIN, HIGH);
  digitalWrite(S2PIN, HIGH);
  digitalWrite(ENABLE, HIGH);

}

/*******************************************************************************/



//!******************************************************************************
//! Name: initPositionSensor()                        *
//! Description: Initializes the position sensor and configure some values.   *
//! Param : void                                *
//! Returns: void                               *
//! Example: MyHospital.initPositionSensor();                 *
//!******************************************************************************

void MySignalsClass::initBodyPosition(void)
{
  // initialize I2C
  //Wire.begin();
  // address of the accelerometer
  Wire.beginTransmission(0x0A);

  // low pass filter, range settings
  Wire.write(0x20);
  Wire.write(0x05);
  Wire.endTransmission();

#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println("-----------------> BODY POSITION SENSOR");
  Serial.println("Accelerometer initialization");
#endif
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getBodyPosition()                       *
//!   Description: Returns the current body position.             *
//!   Param : void                              *
//!   Returns: uint8_t with the the position of the pacient.          *
//!   Example: uint8_t position = MyHospital.getBodyPosition();       *
//!******************************************************************************

uint8_t MySignalsClass::getBodyPosition(void)
{
  // Accelerometer acceleration buffer
  byte Version[3];
  // It stores current body position
  uint8_t bodyPos;

  // address of the accelerometer
  Wire.beginTransmission(0x0A);

  // reset the accelerometer
  Wire.write(0x04); // Y data
  Wire.endTransmission();

  // request 6 bytes from slave device
  Wire.requestFrom(0x0A, 1);

  // slave may send less than requested
  while (Wire.available())
  {
    // receive a byte as character
    Version[0] = Wire.read();
  }

  x_data = (int8_t)Version[0] >> 2;

  // address of the accelerometer
  Wire.beginTransmission(0x0A);

  // reset the accelerometer
  // Y data
  Wire.write(0x06);
  Wire.endTransmission();

  // request 6 bytes from slave device
  Wire.requestFrom(0x0A, 1);

  // slave may send less than requested
  while (Wire.available())
  {
    // receive a byte as character
    Version[1] = Wire.read();
  }

  y_data = (int8_t)Version[1] >> 2;

  // address of the accelerometer
  Wire.beginTransmission(0x0A);

  // reset the accelerometer
  // Y data
  Wire.write(0x08);
  Wire.endTransmission();

  // request 6 bytes from slave device
  Wire.requestFrom(0x0A, 1);

  // slave may send less than requested
  while (Wire.available())
  {
    // receive a byte as character
    Version[2] = Wire.read();
  }

  z_data = (int8_t)Version[2] >> 2;

  delay(100);

  // determinate the patient position
  bodyPos = bodyPosition();

#if MYHOSPITAL_DEBUG > 0

  Serial.println();
  Serial.println("-----------------> BODY POSITION SENSOR");
  // print the X Y Z acceleration raw data
  char bufferAcc[30];
  sprintf (bufferAcc, "Acceleration: X= %d  Y= %d  Z= %d  ", MySignals.x_data, MySignals.y_data, MySignals.z_data);
  Serial.println(bufferAcc);
  // print body position value
  Serial.print("Body Position value = ");
  Serial.println(bodyPos);
#endif

  return bodyPos;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: printPosition()                         *
//!   Description: Print the current body position              *
//!   Param : uint8_t position                        *
//!   Returns: void                             *
//!   Example: MyHospital.printPosition(position);              *
//!******************************************************************************

void MySignalsClass::printPosition( uint8_t position )
{
  if (position == 1)
  {
    Serial.println(F("Prone position"));
  }
  else if (position == 2)
  {
    Serial.println(F("Left lateral decubitus"));
  }
  else if (position == 3)
  {
    Serial.println(F("Rigth lateral decubitus"));
  }
  else if (position == 4)
  {
    Serial.println(F("Supine position"));
  }
  else if (position == 5)
  {
    Serial.println(F("Stand or sit position"));
  }
  else
  {
    Serial.println(F("non-defined position"));
  }
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getAcceleration()                       *
//!   Description: Returns an ACC value to represent the X Y Z axis       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.printAcceleration();                *
//!******************************************************************************

void MySignalsClass::getAcceleration( void )
{
  // convert the raw data to acceleration
  float LSB_parameter = 0.0625;
  MySignals.x_data = MySignals.x_data * LSB_parameter;
  MySignals.y_data = MySignals.y_data * LSB_parameter;
  MySignals.z_data = MySignals.z_data * LSB_parameter;

#if MYHOSPITAL_DEBUG > 0
  // convert float to strintg
  char bufferAcc[50];
  char x_acc_s[10];
  char y_acc_s[10];
  char z_acc_s[10];
  dtostrf (MySignals.x_data, 2, 3, x_acc_s);
  dtostrf (MySignals.y_data, 2, 3, y_acc_s);
  dtostrf (MySignals.z_data, 2, 3, z_acc_s);

  // print the X Y Z acceleration
  sprintf (bufferAcc, "Acceleration: X= %s  Y= %s  Z= %s  ", x_acc_s, y_acc_s, z_acc_s);
  Serial.println(bufferAcc);
#endif
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getAirFlow()                          *
//!   Description: Returns an analogic value to represent the air flow.   *
//!   Param : void                              *
//!   Returns: int with the airFlow value (0-1023).             *
//!   Example: int airFlow = MyHospital.getAirflow();             *
//!******************************************************************************

float MySignalsClass::getAirflow(uint8_t format)
{
  float sensorValue = readADC(ADC_AIRFLOW);

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> AIRFLOW SENSOR");
  Serial.print("Airflow raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5.0 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: airflow()                           *
//!   Description: Returns an analogic value to represent the EMG.      *
//!   Param : void                              *
//!   Returns: float with the EMG value in voltage              *
//!   Example: float volt = MyHospital.getEMG();                *
//!******************************************************************************

float MySignalsClass::getAirflow(void)
{
  return getAirflow(DATA);
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *

//!******************************************************************************

float MySignalsClass::getCalibratedAirflow(uint8_t samples, uint16_t delaySample, float offset, uint8_t format)
{
  //Local variables

  float samplesArray[samples];
  float sensorValue;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    // Read from analogic in.
    sensorValue = readADC(ADC_AIRFLOW);
    // enable global interrupts
    //sei();
    // binary to voltage conversion

    samplesArray[i] = sensorValue;

    delay(delaySample);
  }

  sensorValue = median(samplesArray, samples);
  sensorValue = sensorValue - offset;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> Airflow SENSOR");
  Serial.print("Calibrated ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: airflowBPM()                            *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::airflowPPM(void)
{
  airflowPeakCounter++;

  if (interruptNCCounter2 >= 100)
  {
    airflowDataPPMBalanced = 0;
  }

  airflowDataPrevious [2] = airflowDataPrevious [1];
  airflowDataPrevious [1] = airflowDataPrevious [0];
  airflowDataPrevious [0] = analogRead(A2);

  if (airflowDataPrevious [1] < airflowDataMin)
  {
    airflowDataMin = airflowDataPrevious [1];
  }

  if (airflowDataPrevious [1] > airflowDataMin)
  {
    airflowDataMin = 0.99995 * airflowDataMin + 0.00005 * airflowDataPrevious [1];
  }

  if (airflowDataPrevious [1] > airflowDataMax)
  {
    airflowDataMax = airflowDataPrevious [1];
  }

  if (airflowDataPrevious [1] < airflowDataMax)
  {
    airflowDataMax = (0.999995 * airflowDataMax + 0.000005 * airflowDataPrevious [1]);

    if (airflowDataMax < 50)
    {
      airflowDataMax = 50;
    }
  }

  if ((interruptNCCounter2 >= 100) && ((airflowDataMax - airflowDataMin) <= 30))
  {
    airflowDataPPMBalanced = 0;
  }

  uint16_t airflowTresholdUp   = airflowDataMin + 0.75 * (airflowDataMax - airflowDataMin);
  uint16_t airflowTresholdDown = airflowDataMin + 0.45 * (airflowDataMax - airflowDataMin);

  if ( airflowDataPrevious [1] <= airflowTresholdDown)
  {
    airflowFlagState = 1;
  }

  else if (airflowFlagState == 1
           && airflowDataPrevious [1] >= airflowTresholdUp
           && airflowDataPrevious [1] >  airflowDataPrevious [0]
           && airflowDataPrevious [1] >= airflowDataPrevious [2])
  {

    airflowPeakPointer [0] = airflowPeakPointer [1];
    airflowPeakPointer [1] = airflowPeakCounter;
    airflowDataPPM = 60 * 20 / (airflowPeakPointer [1] - airflowPeakPointer [0]);
    delay(50);

    airflowFlagState = 0;
    //Serial.println(F(" +1 "));

    if ((airflowDataPPM >= 2) && (airflowDataPPM <= 90) )
    {
      airflowDataBalanced[3] = airflowDataBalanced[2];
      airflowDataBalanced[2] = airflowDataBalanced[1];
      airflowDataBalanced[1] = airflowDataBalanced[0];
      airflowDataBalanced[0] = airflowDataPPM;

      if ( airflowDataBalanced[3] > 0 )
      {
        airflowDataPPMBalanced = ((airflowDataBalanced[3] * 3) + (airflowDataBalanced[2] * 2) + (airflowDataBalanced[1] * 2) + (airflowDataBalanced[0] * 1)) / 8;

#if MYHOSPITAL_DEBUG > 0
        Serial.println();
        Serial.println(F("-----------------> ECG SENSOR"));
        Serial.print(F("BPM rate balanced = "));
        Serial.println(airflowDataPPMBalanced);
#endif
      }
    }
  }
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getSkinConductance()                      *
//!   Description: Returns the value of skin conductance.           *
//!   Param : void                              *
//!   Returns: float with the value of skin conductance           *
//!   Example: float conductance = MyHospital.getSkinConductance();     *
//!******************************************************************************

float MySignalsClass::getGSR(uint8_t format)
{
  // Local variable declaration.
  float resistance;
  float conductance;

  // disable global interrupts
  //cli();
  // Read an analogic value from analogic2 pin.
  gsr_raw = readADC(ADC_GSR);
  // enable global interrupts
  //sei();
  //Serial.print("grs raw:");
  //Serial.println(gsr_raw);

  float voltage = gsr_raw * 5.0 / 1023;

  conductance = 2 * ((voltage - 0.5) / 100000);

  // Conductance calculation
  resistance = 1 / conductance;
  conductance = conductance * 1000000;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> GSR SENSOR");
  Serial.print("GSR conductance = ");
  Serial.println(conductance);
  Serial.print("GSR resistance = ");
  Serial.println(resistance);
  Serial.print("GSR voltage = ");
  Serial.println(voltage);
#endif

  if (conductance > 1.0)
  {
    if (format == 1) return conductance;
    else if (format == 2) return resistance;
    else if (format == 3) return voltage;
    else if (format == 4) return gsr_raw;
  }
  else return 0;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getSkinConductance()                      *
//!   Description: Returns the value of skin conductance.           *
//!   Param : void                              *
//!   Returns: float with the value of skin conductance           *
//!   Example: float conductance = MyHospital.getSkinConductance();     *
//!******************************************************************************

float MySignalsClass::getGSR(void)
{
  // Local variable declaration.
  float resistance;
  float conductance;

  // disable global interrupts
  //cli();
  // Read an analogic value from analogic2 pin.
  gsr_raw = readADC(ADC_GSR);
  // enable global interrupts
  //sei();

  float voltage = gsr_raw * 5.0 / 1023;

  conductance = 2 * ((voltage - 0.5) / 100000);

  // Conductance calculation
  resistance = 1 / conductance;
  conductance = conductance * 1000000;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> GSR SENSOR");
  Serial.print("GSR conductance = ");
  Serial.println(conductance);
  Serial.print("GSR resistance = ");
  Serial.println(resistance);
  Serial.print("GSR voltage = ");
  Serial.println(voltage);
#endif

  if (conductance > 1.0)
  {
    MySignals.GSRData.conductance = conductance;
    MySignals.GSRData.resistance = resistance;
    MySignals.GSRData.voltage =  voltage;
  }
  else return 0;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getCalibratedGSR(uint8_t samples, uint16_t delaySample, float offset, uint8_t format)
{
  //Local variables

  float samplesArray[samples];
  float sensorValue;

  float resistance;
  float conductance;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    // Read from analogic in.
    sensorValue = readADC(ADC_GSR);
    // enable global interrupts
    //sei();
    // binary to voltage conversion
    samplesArray[i] = sensorValue;

    delay(delaySample);

  }

  sensorValue = median(samplesArray, samples);
  sensorValue = sensorValue - offset;

  float voltage = sensorValue * 5.0 / 1023;

  conductance = 2 * ((voltage - 0.5) / 100000);

  // Conductance calculation
  resistance = 1 / conductance;
  conductance = conductance * 1000000;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> GSR SENSOR");
  Serial.print("Calibrated GSR conductance = ");
  Serial.println(conductance);
  Serial.print("Calibrated GSR resistance = ");
  Serial.println(resistance);
  Serial.print("Calibrated GSR voltage = ");
  Serial.println(voltage);
#endif


  if (conductance > 1.0)
  {
    if (format == 1) return conductance;
    else if (format == 2) return resistance;
    else if (format == 3) return voltage;
    else if (format == 4) return sensorValue;
  }
  else return 0;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getTemperature()                        *
//!   Description: Returns the corporal temperature.              *
//!   Param : void                              *
//!   Returns: float with the corporal temperature value.           *
//!   Example: float temperature = MyHospital.getTemperature();       *
//!******************************************************************************

float MySignalsClass::getTemperature(void)
{
  //Local variables
  float temperature; //Corporal Temperature
  float resistance;  //Resistance of MySignals.
  float ganancia = 5.0;
  float Vcc = 5.0;
  float refTension = 3.0; // Voltage Reference of Wheatstone bridge.
  float Ra = 4700.0; //Wheatstone bridge resistance.
  float Rc = 4700.0; //Wheatstone bridge resistance.
  float Rb = 820.0; //Wheatstone bridge resistance.

  // disable global interrupts
  //cli();
  int sensorValue = readADC(ADC_TEMPERATURE);
  // enable global interrupts
  //sei();

  float voltage = ((float)sensorValue * Vcc) / 1023; // binary to voltage conversion
  // Wheatstone bridge output voltage.
  voltage = voltage / ganancia;
  // Resistance sensor calculate
  float aux = (voltage / refTension) + Rb / (Rb + Ra);
  resistance = Rc * aux / (1 - aux);
  
  
  if (resistance >= 1822.8)
  {
    // if temperature between 25ºC and 29.9ºC. R(tª)=6638.20457*(0.95768)^t
    temperature = log(resistance / 6638.20457) / log(0.95768);
  }
  else
  {
    if (resistance >= 1477.1)
    {
      // if temperature between 30ºC and 34.9ºC. R(tª)=6403.49306*(0.95883)^t
      temperature = log(resistance / 6403.49306) / log(0.95883);
    }
    else
    {
      if (resistance >= 1204.8)
      {
        // if temperature between 35ºC and 39.9ºC. R(tª)=6118.01620*(0.96008)^t
        temperature = log(resistance / 6118.01620) / log(0.96008);
      }
      else
      {
        if (resistance >= 988.1)
        {
          // if temperature between 40ºC and 44.9ºC. R(tª)=5859.06368*(0.96112)^t
          temperature = log(resistance / 5859.06368) / log(0.96112);
        }
        else
        {
          if (resistance >= 811.7)
          {
            // if temperature between 45ºC and 50ºC. R(tª)=5575.94572*(0.96218)^t
            temperature = log(resistance / 5575.94572) / log(0.96218);
          }
        }
      }
    }
  }

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> TEMPERATURE SENSOR");
  Serial.print("temperature = ");
  Serial.println(temperature);
  Serial.print("resistance = ");
  Serial.println(resistance);
  Serial.print("voltage = ");
  Serial.println(voltage);
#endif

  return temperature;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getCalibrateTemperature()                   *
//!   Description: Returns the corporal temperature.              *
//!   Param : void                              *
//!   Returns: float with the corporal temperature value.           *
//!   Example: float temperature = MyHospital.getCalibrateTemperature();    *
//!******************************************************************************

float MySignalsClass::getCalibratedTemperature(uint8_t samples, uint16_t delaySample, float offset , uint8_t format)
{
  //Local variables
  float temperature; //Corporal Temperature
  float resistance;  //Resistance of MySignals.
  float ganancia = 5.0;
  float Vcc = 5.0;
  float refTension = 3.0; // Voltage Reference of Wheatstone bridge.
  float Ra = 4700.0; //Wheatstone bridge resistance.
  float Rc = 4700.0; //Wheatstone bridge resistance.
  float Rb = 820.0; //Wheatstone bridge resistance.
  float samplesArray[samples];
  float voltage;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    int sensorValue = readADC(ADC_TEMPERATURE);
    // enable global interrupts
    //sei();


    // binary to voltage conversion
    voltage = ((float)sensorValue * Vcc) / 1023;
    // Wheatstone bridge output voltage.
    voltage = voltage / ganancia;
    // Resistance sensor calculate
    float aux = (voltage / refTension) + Rb / (Rb + Ra);
    resistance = Rc * aux / (1 - aux);

    delay(delaySample);

    if (resistance >= 1822.8)
    {
      // if temperature between 25ºC and 29.9ºC. R(tª)=6638.20457*(0.95768)^t
      samplesArray[i] = log(resistance / 6638.20457) / log(0.95768);
    }
    else
    {
      if (resistance >= 1477.1)
      {
        // if temperature between 30ºC and 34.9ºC. R(tª)=6403.49306*(0.95883)^t
        samplesArray[i] = log(resistance / 6403.49306) / log(0.95883);
      }
      else
      {
        if (resistance >= 1204.8)
        {
          // if temperature between 35ºC and 39.9ºC. R(tª)=6118.01620*(0.96008)^t
          samplesArray[i] = log(resistance / 6118.01620) / log(0.96008);
        }
        else
        {
          if (resistance >= 988.1)
          {
            // if temperature between 40ºC and 44.9ºC. R(tª)=5859.06368*(0.96112)^t
            samplesArray[i] = log(resistance / 5859.06368) / log(0.96112);
          }
          else
          {
            if (resistance >= 811.7)
            {
              // if temperature between 45ºC and 50ºC. R(tª)=5575.94572*(0.96218)^t
              samplesArray[i] = log(resistance / 5575.94572) / log(0.96218);
            }
          }
        }
      }
    }
  }

  temperature = median(samplesArray, samples);
  temperature = temperature - offset;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> TEMPERATURE SENSOR");
  Serial.print("Calibrated temperature = ");
  Serial.println(temperature);
  Serial.print("Resistance = ");
  Serial.println(resistance);
  Serial.print("Voltage = ");
  Serial.println(voltage);
#endif

  if (format == 5) return temperature;
  if (format == 3) return voltage;
  else return resistance;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getECG(uint8_t format)
{
  // disable global interrupts
  //cli();
  // Read from analogic in.
  float sensorValue = readADC(ADC_ECG);

  // enable global interrupts
  //sei();
  // binary to voltage conversion

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> ECG SENSOR");
  Serial.print("ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getECG(void)
{
  return MySignals.getECG(DATA);
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getCalibratedECG(uint8_t samples, uint16_t delaySample, float offset, uint8_t format)
{
  //Local variables

  float samplesArray[samples];
  float sensorValue;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    // Read from analogic in.
    sensorValue = readADC(ADC_ECG);
    // enable global interrupts
    //sei();
    // binary to voltage conversion

    samplesArray[i] = sensorValue;

    delay(delaySample);
  }

  sensorValue = median(samplesArray, samples);
  sensorValue = sensorValue - offset;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> ECG SENSOR");
  Serial.print("Calibrated ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::ECGBPM(void)
{
  ECGPeakCounter++;

  if (interruptNCCounter >= 2000)
  {
    ECGDataBPMBalanced = 0;
  }

  ECGDataPrevious [2] = ECGDataPrevious [1];
  ECGDataPrevious [1] = ECGDataPrevious [0];
  ECGDataPrevious [0] = analogRead(A1);

  if (ECGDataPrevious[0] >= 500 || ECGDataPrevious[0] <= 200)
  {
    ECGCorrectMeasure = 0;
  }

  ECGDerDataPrevious [2] = ECGDerDataPrevious [1];
  ECGDerDataPrevious [1] = ECGDerDataPrevious [0];
  ECGDerDataPrevious [0] = (- ECGDataPrevious [0] + 2 * ECGDataPrevious [1] - ECGDataPrevious [2]) + 128;

  ECGDerData = ECGDerDataPrevious [0];

  if (ECGDerDataPrevious [1] < ECGDataMin)
  {
    ECGDataMin = ECGDerDataPrevious [1];
  }
  if (ECGDerDataPrevious [1] > ECGDataMin)
  {
    ECGDataMin = 0.99995 * ECGDataMin + 0.00005 * ECGDerDataPrevious[1];
  }

  if (ECGDerDataPrevious [1] > ECGDataMax)
  {
    ECGDataMax = ECGDerDataPrevious [1];
  }
  if (ECGDerDataPrevious [1] < ECGDataMax)
  {
    ECGDataMax = 0.99995 * ECGDataMax + 0.00005 * ECGDerDataPrevious [1];
  }

  uint16_t ECGTresholdUp   = ECGDataMin + 0.80 * (ECGDataMax - ECGDataMin);
  uint16_t ECGTresholdDown = ECGDataMin + 0.70 * (ECGDataMax - ECGDataMin);



  if ( ECGDerDataPrevious [1] <= ECGTresholdDown)
  {
    ECGFlagState = 1;
    //Serial.println(F(" +1 ");Serial.println(F(" +1 ");
  }

  else if (ECGFlagState == 1
           && ECGDerDataPrevious [1] >= ECGTresholdUp
           && ECGDerDataPrevious [1] >  ECGDerDataPrevious [0]
           && ECGDerDataPrevious [1] >= ECGDerDataPrevious [2]
           && (ECGDataMax - ECGDataMin >= 70))
  {
    ECGPeakPointer [0] = ECGPeakPointer [1];
    ECGPeakPointer [1] = ECGPeakCounter;
    ECGDataBPM = 60 * 200 / (ECGPeakPointer [1] - ECGPeakPointer [0]);
    delay(50);
    //Serial.println (ECGDataMax-ECGDataMin);
    ECGFlagState = 0;

    if ((ECGDataBPM >= 2) && (ECGDataBPM <= 180) )
    {

      ECGDataBalanced[3] = ECGDataBalanced[2];
      ECGDataBalanced[2] = ECGDataBalanced[1];
      ECGDataBalanced[1] = ECGDataBalanced[0];
      ECGDataBalanced[0] = ECGDataBPM;

      ECGCorrectMeasure = 1;

      if ( ECGDataBalanced[3] > 0 )
      {
        ECGDataBPMBalanced = ((((ECGDataBalanced[3] * 3) + (ECGDataBalanced[2] * 2) + (ECGDataBalanced[1] * 2) + (ECGDataBalanced[0] * 1)) / 8) * 0.2) + (ECGDataBPMBalancedPrev * 0.8);
        ECGDataBPMBalancedPrev = ECGDataBPMBalanced;

#if MYHOSPITAL_DEBUG > 0
        Serial.println();
        Serial.println(F("-----------------> ECG SENSOR"));
        Serial.print(F("BPM rate balanced = "));
        Serial.println(ECGDataBPMBalanced);
#endif
      }
    } else {
      ECGCorrectMeasure = 0;
    }
  }

}

/*******************************************************************************/




//!******************************************************************************
//!   Name: getEMG()                            *
//!   Description: Returns an analogic value to represent the EMG.      *
//!   Param : void                              *
//!   Returns: float with the EMG value in voltage              *
//!   Example: float volt = MyHospital.getEMG();                *
//!******************************************************************************

float MySignalsClass::getEMG(uint8_t format)
{
  // disable global interrupts
  //cli();
  // Read from analogic in.
  float sensorValue = readADC(ADC_EMG);
  // enable global interrupts
  //sei();

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> ECG SENSOR");
  Serial.print("ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getEMG()                            *
//!   Description: Returns an analogic value to represent the EMG.      *
//!   Param : void                              *
//!   Returns: float with the EMG value in voltage              *
//!   Example: float volt = MyHospital.getEMG();                *
//!******************************************************************************

float MySignalsClass::getEMG(void)
{
  return getEMG(DATA);
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getCalibratedEMG(uint8_t samples, uint16_t delaySample, float offset, uint8_t format)
{
  //Local variables

  float samplesArray[samples];
  float sensorValue;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    // Read from analogic in.
    sensorValue = readADC(ADC_EMG);
    // enable global interrupts
    //sei();
    // binary to voltage conversion

    samplesArray[i] = sensorValue;

    delay(delaySample);
  }

  sensorValue = median(samplesArray, samples);
  sensorValue = sensorValue - offset;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> EMG SENSOR");
  Serial.print("Calibrated ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::EMGCPM(void)
{
  EMGPeakCounter++;

  if (interruptNCCounter2 >= 100)
  {
    EMGDataCPMBalanced = 0;
  }

  EMGDataPrevious [2] = EMGDataPrevious [1];
  EMGDataPrevious [1] = EMGDataPrevious [0];
  EMGDataPrevious [0] = map(analogRead(A0), 250, 350, 0, 1024);
  //Serial.println("EMG");
  // Serial.println(EMGDataPrevious [0]);
  if (EMGDataPrevious [1] < EMGDataMin)
  {
    EMGDataMin = EMGDataPrevious [1];
  }

  if (EMGDataPrevious [1] > EMGDataMin)
  {
    EMGDataMin = 0.98 * EMGDataMin + 0.02 * EMGDataPrevious [1];
  }

  if (EMGDataPrevious [1] > EMGDataMax)
  {
    EMGDataMax = EMGDataPrevious [1];
  }

  if (EMGDataPrevious [1] < EMGDataMax)
  {
    EMGDataMax = 0.98 * EMGDataMax + 0.02 * EMGDataPrevious [1];
  }
  //Serial.println(EMGDataMax);
  uint16_t EMGTresholdUp   = EMGDataMin + 0.50 * (EMGDataMax - EMGDataMin);
  uint16_t EMGTresholdDown = EMGDataMin + 0.20 * (EMGDataMax - EMGDataMin);

  if ( EMGDataPrevious [1] <= EMGTresholdDown)
  {
    EMGFlagState = 1;
    //Serial.println(F(" +1 "));
  }

  else if (EMGFlagState == 1
           && EMGDataPrevious [1] >= EMGTresholdUp
           && EMGDataPrevious [1] >  EMGDataPrevious [0]
           && EMGDataPrevious [1] >= EMGDataPrevious [2])
  {
    EMGPeakPointer [0] = EMGPeakPointer [1];
    EMGPeakPointer [1] = EMGPeakCounter;
    EMGDataCPM = 60 * 20 / (EMGPeakPointer [1] - EMGPeakPointer [0]);
    delay(50);
    //Serial.println(F(" +2 "));
    EMGFlagState = 0;

    if ((EMGDataCPM >= 2) && (EMGDataCPM <= 60) )
    {
      EMGDataBalanced[3] = EMGDataBalanced[2];
      EMGDataBalanced[2] = EMGDataBalanced[1];
      EMGDataBalanced[1] = EMGDataBalanced[0];
      EMGDataBalanced[0] = EMGDataCPM;

      if ( EMGDataBalanced[3] > 0 )
      {
        EMGDataCPMBalanced = ((EMGDataBalanced[3] * 3) + (EMGDataBalanced[2] * 2) + (EMGDataBalanced[1] * 2) + (EMGDataBalanced[0] * 1)) / 8;

#if MYHOSPITAL_DEBUG > 0
        Serial.println();
        Serial.println(F("-----------------> ECG SENSOR"));
        Serial.print(F("BPM rate balanced = "));
        Serial.println(EMGDataCPMBalanced);
#endif
      }
    }
  }
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::initSnore()
{

  pinMode(BUTTON, INPUT);


#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println(F("-----------------> SNORE SENSOR"));
  Serial.println(F("Snore initialization"));
#endif
}
/*******************************************************************************/




/*******************************************************************************/


//!******************************************************************************
//!   Name: getEMG()                            *
//!   Description: Returns an analogic value to represent the EMG.      *
//!   Param : void                              *
//!   Returns: float with the EMG value in voltage              *
//!   Example: float volt = MyHospital.getEMG();                *
//!******************************************************************************

float MySignalsClass::getSnore(uint8_t format)
{
  // disable global interrupts
  //cli();
  // Read from analogic in.
  float sensorValue = readADC(ADC_SNORE);
  // enable global interrupts
  //sei();

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> AIRFLOW SENSOR");
  Serial.print("Airflow raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getEMG()                            *
//!   Description: Returns an analogic value to represent the EMG.      *
//!   Param : void                              *
//!   Returns: float with the EMG value in voltage              *
//!   Example: float volt = MyHospital.getEMG();                *
//!******************************************************************************

float MySignalsClass::getSnore(void)
{
  return getSnore(DATA);
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: getECG()                            *
//!   Description: Returns an analogic value to represent the ECG.      *
//!   Param : void                              *
//!   Returns: float with the ECG value in voltage              *
//!   Example: float volt = MyHospital.getECG();                *
//!******************************************************************************

float MySignalsClass::getCalibratedSnore(uint8_t samples, uint16_t delaySample, float offset, uint8_t format)
{
  //Local variables

  float samplesArray[samples];
  float sensorValue;


  for (int i = 0; i < samples; i++)
  {

    // disable global interrupts
    //cli();
    // Read from analogic in.
    sensorValue = readADC(ADC_SNORE);
    // enable global interrupts
    //sei();
    // binary to voltage conversion

    samplesArray[i] = sensorValue;

    delay(delaySample);
  }

  sensorValue = median(samplesArray, samples);
  sensorValue = sensorValue - offset;

#if MYHOSPITAL_DEBUG > 0
  Serial.println();
  Serial.println("-----------------> SNORE SENSOR");
  Serial.print("Calibrated ECG raw data = ");
  Serial.println(sensorValue);
#endif

  if (format == 3) return sensorValue = (float)sensorValue * 5 / 1023.0;
  else return sensorValue;
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::snoreSPM(void)
{
  snorePeakCounter++;

  if (interruptNCCounter2 >= 100)
  {
    snoreDataSPMBalanced = 0;
  }

  snoreDataPrevious [2] = snoreDataPrevious [1];
  snoreDataPrevious [1] = snoreDataPrevious [0];
  snoreDataPrevious [0] = analogRead(A5);

  if (snoreDataPrevious [1] < snoreDataMin)
  {
    snoreDataMin = snoreDataPrevious [1];
  }

  if (snoreDataPrevious [1] > snoreDataMin)
  {
    snoreDataMin = 0.99995 * snoreDataMin + 0.00005 * snoreDataPrevious [1];
  }

  if (snoreDataPrevious [1] > snoreDataMax)
  {
    snoreDataMax = snoreDataPrevious [1];
  }

  if (snoreDataPrevious [1] < snoreDataMax)
  {
    snoreDataMax = (0.999995 * snoreDataMax + 0.000005 * snoreDataPrevious [1]);

    if (snoreDataMax < 50)
    {
      snoreDataMax = 50;
    }
  }

  uint16_t snoreTresholdUp   = snoreDataMin + 0.55 * (snoreDataMax - snoreDataMin);
  uint16_t snoreTresholdDown = snoreDataMin + 0.20 * (snoreDataMax - snoreDataMin);

  if ( snoreDataPrevious [1] <= snoreTresholdDown)
  {
    snoreFlagState = 1;
  }

  else if (snoreFlagState == 1
           && snoreDataPrevious [1] >= snoreTresholdUp
           && snoreDataPrevious [1] >  snoreDataPrevious [0]
           && snoreDataPrevious [1] >= snoreDataPrevious [2])
  {
    snorePeakPointer [0] = snorePeakPointer [1];
    snorePeakPointer [1] = snorePeakCounter;
    snoreDataSPM = 60 * 20 / (snorePeakPointer [1] - snorePeakPointer [0]);
    delay(50);

    snoreFlagState = 0;
    //Serial.println(F(" +1 "));

    if ((snoreDataSPM >= 2) && (snoreDataSPM <= 60) )
    {
      snoreDataBalanced[3] = snoreDataBalanced[2];
      snoreDataBalanced[2] = snoreDataBalanced[1];
      snoreDataBalanced[1] = snoreDataBalanced[0];
      snoreDataBalanced[0] = snoreDataSPM;
      //Serial.println(SnoreDataPPM);
      if ( snoreDataBalanced[3] > 0 )
      {
        snoreDataSPMBalanced = ((snoreDataBalanced[3] * 3) + (snoreDataBalanced[2] * 2) + (snoreDataBalanced[1] * 2) + (snoreDataBalanced[0] * 1)) / 8;

#if MYHOSPITAL_DEBUG > 0
        Serial.println();
        Serial.println(F("-----------------> ECG SENSOR"));
        Serial.print(F("BPM rate balanced = "));
        Serial.println(snoreDataSPMBalanced);
#endif
      }
    }
  }
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************



/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

void MySignalsClass::playSoundSnore(uint16_t frequency, uint16_t duration)
{
  tone(SPEAKER, frequency, duration);

#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println("-----------------> SNORE SENSOR");
  Serial.println("Snore play sound");
#endif
}

/*******************************************************************************/



//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MySignals.airflowBPM();                    *
//!******************************************************************************

void MySignalsClass::playAudioSnore()
{

  pinMode(SPEAKER, OUTPUT);

  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.

  // Use internal clock (datasheet p.160)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));

  // Set fast PWM mode  (p.157)
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);


  // Do non-inverting PWM on pin OC2B (p.155)
  // On the Arduino this is pin 3.
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~_BV(COM2B0);
  TCCR2A &= ~(_BV(COM2A1) | _BV(COM2A0));
  // No prescaler (p.158)
  TCCR2B = (TCCR2B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

 
  OCR2B = pgm_read_byte(&soundalarm_data[0]);
  

  // Set up Timer 1 to send a sample every interrupt.

  cli();

    // Set CTC mode (Clear Timer on Compare Match) (p.133)
    // Have to set OCR1A *after*, otherwise it gets reset to 0!
    TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
    TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));

    // No prescaler (p.134)
    TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);

    // Set the compare register (OCR1A).
    // OCR1A is a 16-bit register, so we have to do this with
    // interrupts disabled to be safe.
    OCR1A = F_CPU / SAMPLERATE;    // 16e6 / 8000 = 2000

    // Enable interrupt when TCNT1 == OCR1A (p.136)
    TIMSK1 |= _BV(OCIE1A);

  
    MySignals.lastSample = pgm_read_byte(&soundalarm_data[soundalarm_length - 1]);
  

  MySignals.sample = 0;
  sei();

}


/*******************************************************************************/


//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MySignals.airflowBPM();                    *
//!******************************************************************************

void MySignalsClass::stopAudioSnore(void)
{
    // Disable playback per-sample interrupt.
    TIMSK1 &= ~_BV(OCIE1A);

    // Disable the per-sample timer completely.
    TCCR1B &= ~_BV(CS10);

    // Disable the PWM timer.
    TCCR2B &= ~_BV(CS10);

  digitalWrite(SPEAKER, LOW);

}

//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

bool MySignalsClass::getButtonSnore(void)
{
  bool buttonState = digitalRead(BUTTON);

#if MYHOSPITAL_DEBUG > 1
  Serial.println();
  Serial.println("-----------------> SNORE SENSOR");
  Serial.println("Snore button");
#endif

  return buttonState;
}

/*******************************************************************************/




uint8_t MySignalsClass::getPulsioximeterMini()
{
  const uint8_t BUFFER_TOTAL_NUMBER = 20;
  uint8_t buffer_uart[BUFFER_TOTAL_NUMBER];
  memset(buffer_uart, 0x00, sizeof(buffer_uart));
  uint8_t i = 0;
  bool exit_while = 0;
  unsigned long previous = millis();
  //capturo buffer
  while (((millis() - previous) < 100) && exit_while == 0)
  {
    if (Serial.available() > 0)
    {
      buffer_uart[i] = Serial.read();
      i++;
      //No escribir mas alla de mi buffer, chafaria otras cosas indeterminadas
      if (i == BUFFER_TOTAL_NUMBER)
      {
        exit_while = 1;
      }
    }

    //avoid millis overflow problem after approximately 50 days
    if ( millis() < previous ) previous = millis();
  }

  /*
    for (int i = 0; i < BUFFER_TOTAL_NUMBER; i++)
    {
    Serial.print("Byte ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(buffer_uart[i], HEX);

    }
  */


  uint8_t buffer_temp = 0;
  bool data_found = 0;
  uint8_t j = 0;
  //Busco la estructura de paquete del SPO2 mini
  for (j = 0; j < BUFFER_TOTAL_NUMBER - 4; j++)
  {
    //Hago mascara para solo mirar el primer bit del byte
    buffer_temp = buffer_uart[j] & 0b10000000;
    //si es "1", chequear el del siguiente byte
    if (buffer_temp != 0)
    {
      //si entro, es que si era 1
      buffer_temp = buffer_uart[j + 1] & 0b10000000;
      //si es "0", chequear el del siguiente byte
      if (buffer_temp == 0)
      {
        //si entro, es que si era 0
        buffer_temp = buffer_uart[j + 2] & 0b10000000;
        //si es "0", chequear el del siguiente byte
        if (buffer_temp == 0)
        {
          //si entro, es que si era 0
          buffer_temp = buffer_uart[j + 3] & 0b10000000;
          //si es "0", chequear el del siguiente byte
          if (buffer_temp == 0)
          {
            //si entro, es que si era 0
            buffer_temp = buffer_uart[j + 4] & 0b10000000;
            //si es "0", chequear el del siguiente byte
            if (buffer_temp == 0)
            {
              //si entro, es que si era 0
              buffer_temp = buffer_uart[j + 5] & 0b10000000;
              //si es "0", chequear el del siguiente byte
              if (buffer_temp != 0)
              {
                //si entro, es que si era 1, he encontrado un paquete entero de spo2 mini
                data_found = 1;
                break;
              }
            }
          }
        }
      }
    }
  }
  
  
  
  if(data_found == 1)
  {
    /*
	  byte 1 -> SIGNAL STRENGHT (bits 0-3)
	  byte 2 -> PULSE WAVEFORM (bits 0-6)
	  byte 3 -> PULSE HIGH (bit 6)
	  byte 4 -> PULSE LOW (bits 0-6)
	  byte 5 -> SPO2 (bits 0-6)
    */
	uint8_t byte_read[6];
	uint8_t sensorValueBPM;
	uint8_t sensorValueO2;
	
	byte_read[1] = buffer_uart[j];
    byte_read[2] = buffer_uart[j+1];
    byte_read[3] = buffer_uart[j+2];
    byte_read[4] = buffer_uart[j+3];
    byte_read[5] = buffer_uart[j+4];
    
     //Comprobacion de si el dato es valido, esos bytes en concreto deben ser 0
    byte_read[1] &= 0b00110000;
    if (byte_read[1] != 0)
    {
      //Dato no valido
      //Serial.println("R 1");
      return 2;
    }


	byte_read[3] &= 0b01000000;
	byte_read[4] &= 0b01111111;

	if (byte_read[3] == 0)
	{
	  sensorValueBPM = byte_read[4];
	}

	if (byte_read[3] == 0b01000000)
	{
	  sensorValueBPM = byte_read[4] + 0b10000000;
	}

	sensorValueO2 = byte_read[5];


	if ((sensorValueO2 >= 50) && (sensorValueO2 <= 100) && (sensorValueBPM >= 30) && (sensorValueBPM <= 200) )
	{
	  pulsioximeterData.BPM = sensorValueBPM;
	  pulsioximeterData.O2 = sensorValueO2;

	  return 1;
	}
	else
	{
	  //Serial.println("Dato no valido");
	  //Dato no valido
	  return 2;
	  //Serial.println("R 3");
	}
					
  }
  else
  {
    return 0;
  }
  
}

/*******************************************************************************/



uint8_t MySignalsClass::getPulsioximeterMicro()
{
  const uint8_t BUFFER_TOTAL_NUMBER = 200;
  uint8_t buffer_uart[BUFFER_TOTAL_NUMBER];
  memset(buffer_uart, 0x00, sizeof(buffer_uart));
  uint8_t i = 0;
  bool exit_while = 0;
  unsigned long previous = millis();
  
  
 
  //Envio trama inicial
  spo2_micro_send_init_frame();
  

  //capturo buffer
  while (((millis() - previous) < 100) && exit_while == 0)
  {
    if (Serial.available() > 0)
    {
	  //Serial.println("Leo dato");
      buffer_uart[i] = Serial.read();
      i++;
      //No escribir mas alla de mi buffer, chafaria otras cosas indeterminadas
      if (i == BUFFER_TOTAL_NUMBER)
      {
        exit_while = 1;
        //Serial.println("salgo por tope");
      }
    }

    //avoid millis overflow problem after approximately 50 days
    if ( millis() < previous ) previous = millis();
  }

  
  /*
    for (int i = 0; i < BUFFER_TOTAL_NUMBER; i++)
    {
		Serial.print("Byte ");
		Serial.print(i);
		Serial.print(":");
		Serial.println(buffer_uart[i], HEX);

    }
  */

  
  bool data_found = 0;
  uint8_t j = 0;
  
  
  for (j = 0; j < BUFFER_TOTAL_NUMBER - 9; j++)
  {
	  //Init byte in real time data package is 0x01 (bit 7 is 0)
	  //The next 8 bytes always have bit 7 = 1

		//Detect init byte of a real time data package
        if (buffer_uart[j] == 0x01)
        {
			
          if ((buffer_uart[j + 1] & 0b10000000) == 0x80) //bit 7 must be 1
		  { 
			 if ((buffer_uart[j + 2] & 0b10000000) == 0x80) //bit 7 must be 1
			 {
				 
				if ((buffer_uart[j + 3] & 0b10000000) == 0x80) //bit 7 must be 1
			    { 
					
					if ((buffer_uart[j + 4] & 0b10000000) == 0x80) //bit 7 must be 1
			        { 
						
					    if ((buffer_uart[j + 5] & 0b10000000) == 0x80) //bit 7 must be 1
			            { 
							
							if ((buffer_uart[j + 6] & 0b10000000) == 0x80) //bit 7 must be 1
			                 { 
								 
								 if ((buffer_uart[j + 7] & 0b10000000) == 0x80) //bit 7 must be 1
			                     { 
									 
									 if ((buffer_uart[j + 8] & 0b10000000) == 0x80) //bit 7 must be 1
			                         { 
				 
										 //Detect init byte of the next real time data package
										 if (buffer_uart[j + 9] == 0x01)
										 {
											data_found = 1;
											break;
										 }
									}
								 } 
							 }			 
						}				 
					}					 
			    }
			 } 
		   }
          
        }
  } 
  
  //Serial.print("j:");
  //Serial.println(j);
  
  if(data_found == 1)
  {
	  uint8_t sensorValueBPM;
      uint8_t sensorValueO2;
		
	  sensorValueBPM = buffer_uart[j+5] - 128;
	  sensorValueO2 = buffer_uart[j+6]  - 128;

	  if ((sensorValueO2 >= 50) && (sensorValueO2 <= 100) && (sensorValueBPM >= 30) && (sensorValueBPM <= 200))
	  {
		pulsioximeterData.BPM = sensorValueBPM;
		pulsioximeterData.O2 = sensorValueO2;

		return 1;
	  }
	  else
	  {
		return 2;
	  }
  }
  else
  {
	 return 0;
  }
}




//Si devuelve 0 no hay sensor
//Si devuelve 1 senser Micro detectado
//Si devuelve 2 sensor Mini detectado

uint8_t MySignalsClass::getStatusPulsioximeterGeneral()
{
  //habilito UART y vacio contenido de la uart anterior
  enableSensorUART(PULSIOXIMETER_MICRO);

  //control.SerialFlush();

  //Envio trama inicial
  spo2_micro_send_init_frame();
  
  uint8_t buffer_uart[100];
  memset(buffer_uart, 0x00, sizeof(buffer_uart));
  uint8_t i = 0;
  bool exit_while = 0;
  unsigned long previous = millis();
  //capturo buffer tras haberle enviado el mensaje de inicio
  while (((millis() - previous) < 5) && exit_while == 0)
  {
    if (Serial.available() > 0)
    {
      buffer_uart[i] = Serial.read();
      i++;
      //No escribir mas alla de mi buffer, chafaria otras cosas indeterminadas
      if (i == 100)
      {
        exit_while = 1;
      }
    }

    //avoid millis overflow problem after approximately 50 days
    if ( millis() < previous ) previous = millis();
  }

  /*
    for (int i = 0; i < 100; i++)
    {
    Serial.print("Byte ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(buffer_uart[i], HEX);

    }
  */

  //Primero compruebo si el buffer esta vacio por completo
  bool empty_buffer = 1;

  for (uint8_t j = 0; j < 100; j++)
  {
    if (buffer_uart[j] != 0)
    {
      empty_buffer = 0;
      break;
    }
  }

  if (empty_buffer == 1)
  {
    return 0;
  }


  //No buscar hasta el final del buffer, si la cadena ocupa 4 bytes, solo buscar hasta final menos 4
  for (uint8_t j = 0; j < 100 - 4; j++)
  {
    //Mensaje de inicio que contesta spo2 micro
    if (buffer_uart[j] == 0x0C)
    {
      if (buffer_uart[j + 1] == 0x80)
      {
        if (buffer_uart[j + 2] == 0x0C)
        {
          if (buffer_uart[j + 3] == 0x80)
          {
            //Serial.println("Razon 1");
            return 1;
          }
        }
      }
    }


    //Segundo Mensaje de inicio que contesta spo2 micro
    if (buffer_uart[j] == 0x02)
    {
      if (buffer_uart[j + 1] == 0x80)
      {
        if (buffer_uart[j + 2] == 0x80)
        {
          if (buffer_uart[j + 3] == 0xa0)
          {
            //Serial.println("Razon 2");
            return 1;
          }
        }
      }
    }



    //Tercer Mensaje que contesta spo2 micro si ya esta iniciado de antes
    //entender un mensaje de dato que de micro directamente
    //no solo el de entrada, porque podria pillarlo arrancado ya
    
	  //Init byte in real time data package is 0x01 (bit 7 is 0)
	  //The next 8 bytes always have bit 7 = 1

		//Detect init byte of a real time data package
        if (buffer_uart[j] == 0x01)
        {
          if ((buffer_uart[j + 1] & 0b10000000) == 0x80) //bit 7 must be 1
		  { 
			  
			 if ((buffer_uart[j + 2] & 0b10000000) == 0x80) //bit 7 must be 1
			 {
				if ((buffer_uart[j + 3] & 0b10000000) == 0x80) //bit 7 must be 1
			    { 
					
					if ((buffer_uart[j + 4] & 0b10000000) == 0x80) //bit 7 must be 1
			        { 
						
					    if ((buffer_uart[j + 5] & 0b10000000) == 0x80) //bit 7 must be 1
			            { 
							
							if ((buffer_uart[j + 6] & 0b10000000) == 0x80) //bit 7 must be 1
			                 { 
								 
								 if ((buffer_uart[j + 7] & 0b10000000) == 0x80) //bit 7 must be 1
			                     { 
									 
									 if ((buffer_uart[j + 8] & 0b10000000) == 0x80) //bit 7 must be 1
			                         { 
				 
										 //Detect init byte of the next real time data package
										 if (buffer_uart[j + 9] == 0x01)
										 {
											 //Serial.println("Razon 3");
                                            return 1;
										 }
									 }
								 } 
							 }			 
						}				 
					}					 
			    }
			 } 
		   }
          
        }
  
  }


  //Si despues del bucle anterior sigo aqui, es que hay algo no reconocido conectado
  //podria ser el sensor mini asi que voy a comprobarlo
  enableSensorUART(PULSIOXIMETER);

  memset(buffer_uart, 0x00, sizeof(buffer_uart));
  i = 0;
  exit_while = 0;

  //capturo buffer
  while (((millis() - previous) < 100) && exit_while == 0)
  {
    if (Serial.available() > 0)
    {
      buffer_uart[i] = Serial.read();
      i++;
      //No escribir mas alla de mi buffer, chafaria otras cosas indeterminadas
      if (i == 100)
      {
        exit_while = 1;
      }
    }

    //avoid millis overflow problem after approximately 50 days
    if ( millis() < previous ) previous = millis();
  }

  /*
    for (int i = 0; i < 100; i++)
    {
    Serial.print("Byte ");
    Serial.print(i);
    Serial.print(":");
    Serial.println(buffer_uart[i], HEX);

    }
  */

  //Busco la estructura de paquete del SPO2 mini
  for (uint8_t j = 0; j < 100 - 4; j++)
  {
    //Hago mascara para solo mirar el primer bit del byte

    //si es "1", chequear el del siguiente byte
    if ((buffer_uart[j] & 0b10000000) != 0)
    {
      //si entro, es que si era 1

      //si es "0", chequear el del siguiente byte
      if ((buffer_uart[j + 1] & 0b10000000) == 0)
      {
        //si entro, es que si era 0
       
        //si es "0", chequear el del siguiente byte
        if ((buffer_uart[j + 2] & 0b10000000) == 0)
        {
          //si entro, es que si era 0
   
          //si es "0", chequear el del siguiente byte
          if ((buffer_uart[j + 3] & 0b10000000) == 0)
          {
            //si entro, es que si era 0

            //si es "0", chequear el del siguiente byte
            if ((buffer_uart[j + 4] & 0b10000000) == 0)
            {
              //si entro, es que si era 0
            
              //si es "0", chequear el del siguiente byte
              if ((buffer_uart[j + 5] & 0b10000000) != 0)
              {
                //si entro, es que si era 1, he encontrado un paquete entero de spo2 mini
                return 2;
              }
            }
          }
        }
      }
    }
  }

  return 0;


}




void MySignalsClass::spo2_micro_send_init_frame()
{
  byte m1[] = {0x7D, 0x81};
  
 
/*
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA7);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  
 
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA2);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA0);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////

  Serial.write(m1, sizeof(m1));
  Serial.write(0xB0);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xAC);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xB3);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA8);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xAA);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA9);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
 */
  ///////////////
  Serial.write(m1, sizeof(m1));
  Serial.write(0xA1);
  for (int i = 0; i < 6; i++)
  {
    Serial.write(0x80);
  }
  ///////////////

}












//!******************************************************************************
//!   Name: ECGBPM()                              *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************
/*
  uint8_t MySignalsClass::initEEG()
  {
  pinMode(EXP_VCC_BT, OUTPUT);
  pinMode(EXP_KEY_BT, OUTPUT);
  digitalWrite(EXP_VCC_BT, LOW);
  delay(100);

  digitalWrite(EXP_KEY_BT, LOW);
  delay(100);

    digitalWrite(CONTROLKEY, HIGH);
    delay(100);
    digitalWrite(POWER, HIGH);
    delay(100);

    digitalWrite(ENABLE, LOW);
    delay(20);

    Serial.begin(9600);
    while (Serial.read() >= 0);

    digitalWrite(S0PIN, LOW);
    digitalWrite(S1PIN, LOW);
    digitalWrite(S2PIN, HIGH);
    delay(10);
    digitalWrite(ENABLE, HIGH);
    delay(30);

    Serial.println (F("AT+UART=57600,0,0"));
    delay(30);

    Serial.println (F("AT+ROLE=1"));
    delay(30);

    Serial.println (F("AT+PSWD=0000"));
    delay(30);

    Serial.println (F("AT+CMODE=0"));
    delay(30);

    // MAC Mindwave
    Serial.println (F("AT+BIND=2068,9D,C22446."));
    delay(30);

    Serial.println (F("AT+INIT"));
    delay(30);

    Serial.println (F("AT+IAC=9E8B33"));
    delay(30);

    Serial.println (F("AT+CLASS=0"));
    delay(30);

    Serial.println (F("AT+INQM=1,9,48"));
    delay(30);

    Serial.println (F("AT+INQ"));
    delay(30);

    Serial.println (F("AT+PAIR= 2068,9D,C22446"));
    delay(30);

    // MAC Mindwave
    Serial.println (F("AT+LINK=2068,9D,C22446"));
    delay(30);


    digitalWrite(POWER, LOW);
    delay(100);
    digitalWrite(CONTROLKEY, LOW);
    delay(100);
  }
*/

/*******************************************************************************/






/*
  //!******************************************************************************
  //!   Name: ECGBPM()                              *
  //!   Description: It prints air flow BPM in the serial monitor       *
  //!   Param : void                              *
  //!   Returns: void                             *
  //!   Example: MyHospital.airflowBPM();                   *
  //!******************************************************************************

  uint8_t MySignalsClass::getEEG()
  {

  pauseInterrupt();
  uint8_t data1 ;
  uint8_t data2 ;
  uint8_t data3 ;
  bool EEG_flag = 0;

  unsigned long previous = millis();
  if (Serial.available())

  {

    while (EEG_flag == 0 )
    {

      data1 = data2;
      data2 = data3;
      data3 = readOneByte();

      if ((millis() - previous) >= 1000) {
        EEG_flag = 1;
      }

      if (data1 == 170 && data2 == 170 && data3 == 32)
      {
        EEG_flag = 1;

        EEGData.signal = readOneByte() * 50;
        for (uint8_t i = 0; i < 32; i++)
        {
          char aux = readOneByte();


          if (i == 27)
          {
            EEGData.concentration =  readOneByte();

          }
          if (i == 28)
          {
            EEGData.meditation = readOneByte();

            Serial.print(F("Concentracion: "));
            Serial.println(EEGData.concentration);
            EEG_flag = 1;

          }
        }

      }

    }


  }

  else
  {
    resumeInterrupt();
    return 0;
  }
  resumeInterrupt();
  }

*/
/*******************************************************************************/











//!******************************************************************************
//! Name: readBloodPressureSensor()                     *
//! Description: Initializes the BloodPressureSensor MySignals.          *
//! Param : void                                *
//! Returns: void                               *
//! Example: MyHospital.initBloodPressureSensor();                *
//!******************************************************************************

bool MySignalsClass::getBloodPressure(void)
{

  uint8_t finish_bp = 0;

  Serial.write("e");
  delay(10000);

  unsigned long previous = millis();
  do
  {

    while (Serial.available() > 0 )
    {

      uint8_t data = 0x00;

      data = Serial.read();
      //delayMicroseconds(150);
      //Serial.println(data,HEX);

      if (data == 0x67)
      {
        //Serial.println("Dat1");
        delay(10);

        if (Serial.read() == 0x2f)
        {
          //Serial.println("Dat2");
          //delay(10);
          uint8_t buffer[13];
          memset(buffer, 0x00, sizeof(buffer));

          for (uint8_t i = 0; i < 11; i++)
          {
            buffer[i] = Serial.read();
            //delay(10);
          }


          uint8_t sh = buffer[0] - 48;
          uint8_t sm = buffer[1] - 48;
          uint8_t sl = buffer[2] - 48;
          bloodPressureData.systolic = (sh * 100) + (sm * 10) + sl;

          uint8_t dh = buffer[4] - 48;
          uint8_t dm = buffer[5] - 48;
          uint8_t dl = buffer[6] - 48;
          bloodPressureData.diastolic = (dh * 100) + (dm * 10) + dl;

          uint8_t ph = buffer[8] - 48;
          uint8_t pm = buffer[9] - 48;
          uint8_t pl = buffer[10] - 48;
          bloodPressureData.pulse = (ph * 100) + (pm * 10) + pl;


          finish_bp = 1;
        }
      }

    }

  }
  while ((finish_bp == 0) && ((millis() - previous) < 60000));

  //Turn off blood pressure
  Serial.write("i");

  return finish_bp;

}




/*******************************************************************************/


//!******************************************************************************
//!   Name: readGlucometer()                          *
//!   Description: It reads the data stored in the glucometer         *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.readGlucometer();                 *
//!******************************************************************************

void MySignalsClass::getGlucose(void)
{

  delay(10);

  Serial.write("U"); // Start communication command.
  delay(800); // Wait while receiving data.

  Serial.print(F("\n"));
  if (Serial.available() > 0)
  {
    glucoseLength = Serial.read();// The protocol sends the number of measures

    if (glucoseLength > 5)
    {
      glucoseLength = 5;
    }

    Serial.read(); // Read one dummy data

    for (int i = 0; i < glucoseLength; i++)
    { // The protocol sends data in this order
      glucometerData[i].year = Serial.read();
      glucometerData[i].month = Serial.read();
      glucometerData[i].day = Serial.read();
      glucometerData[i].hour = Serial.read();
      glucometerData[i].minutes = Serial.read();

      Serial.read(); // Byte of separation must be 0x00.

      glucometerData[i].glucose = Serial.read();
      glucometerData[i].meridian = Serial.read();

      Serial.read(); // CheckSum 1
      Serial.read(); // CheckSum 2
    }
  }
}
/*******************************************************************************/





//!******************************************************************************
//!   Name: getSpirometer()                         *
//!   Description: Returns an analogic value to represent the air flow.   *
//!   Param : void                              *
//!   Returns: int with the airFlow value (0-1023).             *
//!   Example: int airFlow = MySignals.getAirflow();              *
//!******************************************************************************

void MySignalsClass::getSpirometer()
{
  char data;


  Serial.println(F("MB+1200RCD?"));
  spir_measures = 0;
  delay(300);

  while (Serial.available() > 0 )
  {

    data = Serial.read();

    if (data == 'O')
    {
      data = Serial.read();
      if (data == 'K')
      {
        data = Serial.read();
        if (data == 0xD)
        { //CR
          data = Serial.read();
          if (data == 0xA)
          { //LF

            bool flag = 0;
            while (flag == 0)
            {
              data = Serial.read(); //read 2
              if (data == 50)
              {
                readSpiroMeasure(spir_measures);
                spir_measures++;


                if (spir_measures == 7)
                {
                  Serial.println(F("Limite 7 medidas!"));
                  flag = 1;
                }
              }
              else
              {
                flag = 1;
              }
            }

          }
        }
      }

    }

  }

#if MYSIGNALS_DEBUG > 0
  Serial.println();
  Serial.println(F("-----------------> SPIROMETER SENSOR"));
#endif
}



void MySignalsClass::readSpiroMeasure(uint8_t _spir_measures)
{
  Serial.read(); //read 0
  uint8_t spir_year_tens = Serial.read() - 48;
  uint8_t spir_year_units = Serial.read() - 48;
  spirometerData[_spir_measures].spir_year = (spir_year_tens * 10) + spir_year_units;

  Serial.read(); //read -

  uint8_t spir_month_tens = Serial.read() - 48;
  uint8_t spir_month_units = Serial.read() - 48;
  spirometerData[_spir_measures].spir_month = (spir_month_tens * 10) + spir_month_units;

  Serial.read(); //read -

  uint8_t spir_day_tens = Serial.read() - 48;
  uint8_t spir_day_units = Serial.read() - 48;
  spirometerData[_spir_measures].spir_day = (spir_day_tens * 10) + spir_day_units;

  Serial.read(); //read espace

  uint8_t spir_hour_tens = Serial.read() - 48;
  uint8_t spir_hour_units = Serial.read() - 48;
  spirometerData[_spir_measures].spir_hour = (spir_hour_tens * 10) + spir_hour_units;

  Serial.read(); //read :

  uint8_t spir_minutes_tens = Serial.read() - 48;
  uint8_t spir_minutes_units = Serial.read() - 48;
  spirometerData[_spir_measures].spir_minutes = (spir_minutes_tens * 10) + spir_minutes_units;

  Serial.read(); //read :
  Serial.read(); //read seconds
  Serial.read(); //read seconds
  Serial.read(); //read :
  Serial.read(); //read espace

  uint8_t spir_pef_high = Serial.read() - 48;
  uint8_t spir_pef_medium = Serial.read() - 48;
  uint8_t spir_pef_low = Serial.read() - 48;
  spirometerData[_spir_measures].spir_pef = (spir_pef_high * 100) + (spir_pef_medium * 10) + spir_pef_low;

  Serial.read(); //read L
  Serial.read(); //read /
  Serial.read(); //read m
  Serial.read(); //read i
  Serial.read(); //read n
  Serial.read(); //read -

  uint8_t spir_fev_high = Serial.read() - 48;
  uint8_t spir_fev_medium = Serial.read() - 48;
  uint8_t spir_fev_low = Serial.read() - 48;
  spirometerData[_spir_measures].spir_fev = (spir_fev_high * 100) + (spir_fev_medium * 10) + spir_fev_low;

  Serial.read(); //read L
  Serial.read(); //read CR
  Serial.read(); //read LF
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: airflowBPM()                            *
//!   Description: It prints air flow BPM in the serial monitor       *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: MyHospital.airflowBPM();                   *
//!******************************************************************************

bool MySignalsClass::deleteSpiroMeasures()
{
  Serial.println(F("MB+FLASHRST"));
  delay(300);

  while (Serial.available() > 0 )
  {
    char data = Serial.read();
    if (data == 'O') {
      data = Serial.read();
      if (data == 'K') {
        data = Serial.read();
        if (data == 0xD) { //CR
          data = Serial.read();
          if (data == 0xA) { //LF
            return 1;

          }
        }
      }
    }
    else
    {
      return 0;
    }
  }
}

/*******************************************************************************/



/*
  Function: Function tu USE UART0 of the shield to print debug messages on the serial port
  Returns:
  Parameters:
  Values:
*/
void MySignalsClass::print(char * msg)
{
  // Select UART0 to send debug messages.
  // By selecting this UART we prevent to send serial data to the module which wil laffect to the module.
  delay(10);
  digitalWrite(ENABLE, HIGH);

  delay(10);
  Serial.print(msg);
  delay(10);

  // Select again the UART1
  digitalWrite(ENABLE, LOW);
  delay(10);
}




/*
  Function: Function tu USE UART0 of the shield to print debug messages on the serial port
  Returns:
  Parameters:
  Values:
*/
void MySignalsClass::println(char * msg)
{
  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);
  Serial.println(msg);

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);
}

void MySignalsClass::println()
{
  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  Serial.println();

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);
}


// this function is always used to print debug messages.
void MySignalsClass::PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);

  for (uint8_t a = 0; a < length; a++)
  {
    if (data[a] < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(data[a], HEX);
    Serial.print(F(" "));
  }

  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}


void MySignalsClass::PrintHex16(uint16_t *data, uint8_t length) // prints 16-bit data in hex with leading zeroes
{
  digitalWrite(ENABLE, HIGH);
  delay(10);

  for (uint8_t a = 0; a < length; a++)
  {
    uint8_t MSB = byte(data[a] >> 8);
    uint8_t LSB = byte(data[a]);

    if (MSB < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(MSB, HEX);
    Serial.print(F(" "));
    if (LSB < 0x10)
    {
      Serial.print(F("0"));
    }
    Serial.print(LSB, HEX);
    Serial.print(F(" "));
  }
  digitalWrite(ENABLE, LOW);
  delay(10);
}





/*******************************************************************************/




//*******************************************************************************
// Private Methods                                *
//*******************************************************************************

//!******************************************************************************
//!   Name: median()                            *
//!   Description: It check the version of the library            *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: median();                      *
//!******************************************************************************

//! Swap data for blood pressure measure

float MySignalsClass::median(float* array, uint8_t length)
{
  int k, l, m, n;
  float aux[length];

  // Initialization of the auxiliar array
  for (k = 0; k < length; k++) aux[k] = -100000;
  aux[0] = array[0];

  // Ordering of the sensor from lower to higher value
  for (l = 1; l < length; l++)
  {
    for (m = 0; m < l + 1; m++)
    {
      if (array[l] > aux[m])
      {
        for (n = length - 1; n > m; n--) aux[n] = aux[n - 1];
        aux[m] = array[l];
        m = l + 1;
        continue;
      }
    }
  }

  // The value in the central position of the array is returned
  return aux[int(length / 2)];
}

/*******************************************************************************/


//!******************************************************************************
//!   Name: bodyPosition()                          *
//!   Description: Assigns a value depending on body position         *
//!   Param : void                              *
//!   Returns: void                             *
//!   Example: bodyPosition();                        *
//!******************************************************************************

uint8_t MySignalsClass::bodyPosition(void)
{
  uint8_t bodyPos;

  if (z_data >= 12)
  {
    bodyPos = 1;
  }

  else if (y_data >= 12)
  {
    bodyPos = 2;
  }

  else if (y_data <= -12)
  {
    bodyPos = 3;
  }

  else if (z_data <= -12)
  {
    bodyPos = 4;
  }
  else if (x_data <= -12)
  {
    bodyPos = 5;
  }
  else
  {
    bodyPos = 6;
  }

  return bodyPos;
}



uint8_t MySignalsClass::getStatusBP()
{
  while (Serial.available() > 0 )
  {
    Serial.read();
  }
  uint8_t data = 0x00;
  Serial.write("a");
  delay(100);
  while (Serial.available() > 0 )
  {
    data = Serial.read();
    //Serial.println(data,HEX);
  }
  //data = 'a';
  if (data == 0x61)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


uint8_t MySignalsClass::getStatusSpiro()
{

  while (Serial.available() > 0 )
  {
    Serial.read();
  }
  uint8_t data = 0x00;
  Serial.println(F("MB+CONNECT"));
  delay(100);

  while (Serial.available() > 0 )
  {

    data = Serial.read();
    if (data == 'O')
    {
      data = Serial.read();
      if (data == 'K')
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
  }
  return 0;
}



void MySignalsClass::enableMuxUART(void)
{
  delay(10);
  digitalWrite(ENABLE, LOW);
  delay(10);

}

void MySignalsClass::disableMuxUART(void)
{
  delay(10);
  digitalWrite(ENABLE, HIGH);
  delay(10);
}
/*******************************************************************************/


void MySignalsClass::serialFlush()
{
  while (Serial.read() >= 0)
    ; // do nothing
}



/*******************************************************************************/


void MySignalsClass::cleanEEPROM(unsigned int eeaddress, unsigned int num_chars)
{

  for (int i = 0; i < num_chars; i++)
  {
    //Write 1 byte function, null value
    Wire.beginTransmission(0x50);
    Wire.write((int)((eeaddress + i) >> 8)); // MSB
    Wire.write((int)((eeaddress + i) & 0xFF)); // LSB
    Wire.write('\0');
    Wire.endTransmission();

    delay(8);
  }


}


void MySignalsClass::writeByteEEPROM(unsigned int eeaddress, byte data )
{
  Wire.beginTransmission(0x50);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();

  delay(8);
}




byte MySignalsClass::readByteEEPROM(unsigned int eeaddress )
{
  byte rdata = 0xFF;

  Wire.beginTransmission(0x50);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(0x50, 1);

  if (Wire.available()) rdata = Wire.read();

  return rdata;
}


void MySignalsClass::writeEEPROM(unsigned int eeaddress, char* data)
{
  // Uses Page Write for 24LC256
  // Allows for 64 byte page boundary
  // Splits string into max 16 byte writes
  unsigned char i = 0, counter = 0;
  unsigned int  address;
  unsigned int  page_space;
  unsigned int  page = 0;
  unsigned int  num_writes;
  unsigned int  data_len = 0;
  unsigned char first_write_size;
  unsigned char last_write_size;
  unsigned char write_size;

  // Calculate length of data
  do
  {
    data_len++;
  }
  while (data[data_len]);

  //Añado uno mas para que escriba el 0 final de fin de cadena.
  //data_len++;

  // Calculate space available in first page
  page_space = int(((eeaddress / 64) + 1) * 64) - eeaddress;

  // Calculate first write size
  if (page_space > 16) {
    first_write_size = page_space - ((page_space / 16) * 16);
    if (first_write_size == 0) first_write_size = 16;
  }
  else
    first_write_size = page_space;

  // calculate size of last write
  if (data_len > first_write_size)
    last_write_size = (data_len - first_write_size) % 16;

  // Calculate how many writes we need
  if (data_len > first_write_size)
    num_writes = ((data_len - first_write_size) / 16) + 2;
  else
    num_writes = 1;

  i = 0;
  address = eeaddress;
  for (page = 0; page < num_writes; page++)
  {
    if (page == 0) write_size = first_write_size;
    else if (page == (num_writes - 1)) write_size = last_write_size;
    else write_size = 16;

    Wire.beginTransmission(0x50);
    Wire.write((int)((address) >> 8));   // MSB
    Wire.write((int)((address) & 0xFF)); // LSB
    counter = 0;
    do {
      Wire.write((byte) data[i]);
      i++;
      counter++;
    }
    while ((data[i]) && (counter < write_size));
    Wire.endTransmission();
    address += write_size; // Increment address for next write

    delay(8);  // needs 5ms for page write
  }
}

void MySignalsClass::writeEEPROM(unsigned int eeaddress, char* data, unsigned int data_len)
{
  // Uses Page Write for 24LC256
  // Allows for 64 byte page boundary
  // Splits string into max 16 byte writes
  unsigned char i = 0, counter = 0;
  unsigned int  address;
  unsigned int  page_space;
  unsigned int  page = 0;
  unsigned int  num_writes;
  unsigned char first_write_size;
  unsigned char last_write_size;
  unsigned char write_size;


  // Calculate space available in first page
  page_space = int(((eeaddress / 64) + 1) * 64) - eeaddress;

  // Calculate first write size
  if (page_space > 16) {
    first_write_size = page_space - ((page_space / 16) * 16);
    if (first_write_size == 0) first_write_size = 16;
  }
  else
    first_write_size = page_space;

  // calculate size of last write
  if (data_len > first_write_size)
    last_write_size = (data_len - first_write_size) % 16;

  // Calculate how many writes we need
  if (data_len > first_write_size)
    num_writes = ((data_len - first_write_size) / 16) + 2;
  else
    num_writes = 1;

  i = 0;
  address = eeaddress;
  for (page = 0; page < num_writes; page++)
  {
    if (page == 0) write_size = first_write_size;
    else if (page == (num_writes - 1)) write_size = last_write_size;
    else write_size = 16;

    Wire.beginTransmission(0x50);
    Wire.write((int)((address) >> 8));   // MSB
    Wire.write((int)((address) & 0xFF)); // LSB
    counter = 0;
    do {
      Wire.write((byte) data[i]);
      i++;
      counter++;
    }
    while ((data[i]) && (counter < write_size));
    Wire.endTransmission();
    address += write_size; // Increment address for next write

    delay(8);  // needs 5ms for page write
  }
}



/***********************************************************************************
   interrupt service routine
 **********************************************************************************/

// This is called at 8000 Hz to load the next sample.
ISR(TIMER1_COMPA_vect)
{
	

	if (MySignals.sample >= soundalarm_length) 
	{
		if (MySignals.sample == soundalarm_length + MySignals.lastSample) 
		{
			MySignals.stopAudioSnore();
		}
		else 
		{
			OCR2B = soundalarm_length + MySignals.lastSample - MySignals.sample;                
		}
	}
	else 
	{
	   OCR2B = pgm_read_byte(&soundalarm_data[MySignals.sample]);             
	}
    


    ++MySignals.sample;
}




/***********************************************************************************
   Preinstantiate Objects
 **********************************************************************************/

MySignalsClass MySignals = MySignalsClass();





