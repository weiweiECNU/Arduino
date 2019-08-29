#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h"

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

uint16_t graphic_x = 0;
uint16_t graphic_x2 = 0;
uint16_t valRead;
uint16_t bpmRead;
uint16_t graphic_prevRead;
uint16_t graphic_prevRead2;
#define graphic_low_extrem 230
#define graphic_high_extrem 50
#define graphic_left_extrem 0
#define graphic_right_extrem 320
#define graphic_low_extremHalf 115
#define graphic_high_extremHalf 120
void setup(){
  Serial.begin(115200);
  MySignals.begin();
  MySignals.initSensorUART();
  MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0,0,320,30,ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); 
  tft.drawString("SENSOR",5,5,4);
  tft.drawLine(graphic_left_extrem, graphic_low_extrem, 
             graphic_right_extrem, graphic_high_extrem, ILI9341_RED);
}

void loop(){
  if (MySignals.getPulsioximeterMicro() == 1) {
    valRead = (uint16_t)MySignals.pulsioximeterData.O2;
    bpmRead = (uint16_t)MySignals.pulsioximeterData.BPM;
    tft.drawString("SP02:", 10, 100, 4);
    tft.drawNumber(MySignals.pulsioximeterData.O2, 100, 100, 4); 
    valRead = map(valRead, 50, 110,115, 50); 
    printGraphic(valRead, 0);
    tft.drawString("BPM:", 10, 200, 4);
    tft.drawNumber(MySignals.pulsioximeterData.BPM, 100, 200, 4); 
    bpmRead = map(bpmRead, 50, 180,230, 120); 
    printGraphic2(bpmRead, 0);
  }
}

void printGraphic(uint16_t value, uint8_t delay_time)//print 02
{
  if (value < graphic_high_extrem)
    value = graphic_high_extrem;
  if (value > graphic_low_extremHalf)
    value = graphic_low_extremHalf;


  //Pinta la linea solo a partir de que ya exista al menos 1 valor
  if (graphic_x > graphic_left_extrem + 1)
    tft.drawLine(graphic_x - 1, graphic_prevRead, graphic_x, value, ILI9341_BLUE);

  //Wave refresh (barre pantalla pintando una linea)
  tft.drawLine(graphic_x + 1, graphic_high_extrem, graphic_x + 1, graphic_low_extremHalf, ILI9341_WHITE);


  graphic_prevRead = value;
  graphic_x++;

  delay(delay_time);

  if (graphic_x == graphic_right_extrem)
    graphic_x = graphic_left_extrem;
  SPI.end();
}


void printGraphic2(uint16_t value, uint8_t delay_time)//print mpb
{
  if (value < graphic_high_extremHalf)
    value = graphic_high_extremHalf;
  if (value > graphic_low_extrem)
    value = graphic_low_extrem;


  //Pinta la linea solo a partir de que ya exista al menos 1 valor
  if (graphic_x2 > graphic_left_extrem + 1)
    tft.drawLine(graphic_x2 - 1, graphic_prevRead2, graphic_x2, value, ILI9341_RED);

  //Wave refresh (barre pantalla pintando una linea)
  tft.drawLine(graphic_x2 + 1, graphic_high_extremHalf, graphic_x2 + 1, graphic_low_extrem, ILI9341_WHITE);


  graphic_prevRead2 = value;
  graphic_x2++;

  delay(delay_time);

  if (graphic_x2 == graphic_right_extrem)
    graphic_x2 = graphic_left_extrem;
  SPI.end();
}


#include <MySignals.h>
#include <Adafruit_ILI9341_AS.h>
#include "Wire.h"
#include "SPI.h"

uint16_t bmpRead;
uint16_t lastValue;

uint32_t avg=0;
uint32_t sum=0;
uint16_t circularIndex=0;
uint16_t count=0;
uint16_t circularBuffer[1023];
#define NUM 1024
void setup() {
  MySignals.begin();
  MySignals.initSensorUART();
  MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
  Serial.begin(115200);
}
void loop() {
  Serial.println();
  if (MySignals.getPulsioximeterMicro() == 1) {

  bmpRead = (uint16_t)MySignals.pulsioximeterData.BPM;
  count ++;
  if(count<NUM){
    circularBuffer[circularIndex] = bmpRead;
    sum+= bmpRead;
    avg = sum/count;
    circularIndex = nextIndex(circularIndex);
    Serial.print("count:");
Serial.println(count);
Serial.print("sum:");
Serial.println(sum);
Serial.print("average:");
    Serial.println(avg);
    }
  else{
    lastValue = circularBuffer[circularIndex]; 
    circularBuffer[circularIndex] = bmpRead;          
    sum-= lastValue;
    sum+= bmpRead;
    avg = sum/NUM;
    circularIndex = nextIndex(circularIndex);
    Serial.println(avg);
    }
  }
}
uint16_t nextIndex(uint16_t value){
  if(value+1>NUM){
    return value = 0;
    }
  else return value+= 1;
  }
