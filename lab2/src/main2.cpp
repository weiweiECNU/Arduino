#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h"

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

uint8_t graphic_x_BPM = 0;
uint8_t graphic_x_Otwo = 0;
uint8_t valReadOtwo;
uint8_t valReadBPM;
uint8_t graphic_prevRead;
uint8_t graphic_prevRead2;

uint8_t avg=0;
uint8_t sum=0;
uint8_t lastValue = 0;
uint8_t Buffer[1023];
uint8_t index=0;
uint8_t count=0;

#define graphic_low_extrem 230
#define graphic_high_extrem 50
#define graphic_low_extremHalf 115
#define graphic_high_extremHalf 120
#define graphic_left_extrem 0
#define graphic_right_extrem 320
#define WIDTH 1024

void setup(){
  Serial.begin(115200);
  MySignals.begin();
  MySignals.initSensorUART();
  MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK); 

}

void loop(){
  if (MySignals.getPulsioximeterMicro() == 1) {

    valReadOtwo = (uint16_t)MySignals.pulsioximeterData.O2;
    valReadBPM = (uint16_t)MySignals.pulsioximeterData.BPM;
    tft.drawString("SP02:", 10, 30, 4);
    tft.drawNumber(valReadOtwo, 100, 30, 4);
    valReadOtwo = map(valReadOtwo, 50, 110,115, 50); 
    printGraphic(valReadOtwo, 0);



    tft.drawString("BPM:", 10, 150, 4);
    tft.drawNumber(valReadBPM, 100, 150, 4);
 
    
    valReadBPM = map(valReadBPM, 50, 180,230, 120);
    printGraphic2(valReadBPM, 0);

  }
}

uint16_t nextIndex(uint16_t value){
  if(value+1>WIDTH){
    return value = 0;
    }
  else return value+= 1;
  }


void printGraphic(uint16_t value, uint8_t delay_time)
{
  if (value < graphic_high_extrem)
    value = graphic_high_extrem;
  if (value > graphic_low_extremHalf)
    value = graphic_low_extremHalf;


  //Pinta la linea solo a partir de que ya exista al menos 1 valor
  if (graphic_x_Otwo > graphic_left_extrem + 1)
    tft.drawLine(graphic_x_Otwo - 1, graphic_prevRead, graphic_x_Otwo, value, ILI9341_WHITE);

  //Wave refresh (barre pantalla pintando una linea)
  tft.drawLine(graphic_x_Otwo + 1, graphic_high_extrem, graphic_x_Otwo + 1, graphic_low_extremHalf, ILI9341_BLACK);


  graphic_prevRead = value;
  graphic_x_Otwo++;

  delay(delay_time);

  if (graphic_x_Otwo == graphic_right_extrem)
    graphic_x_Otwo = graphic_left_extrem;
  SPI.end();
}


void printGraphic2(uint16_t value, uint8_t delay_time)
{
  if (value < graphic_high_extremHalf)
    value = graphic_high_extremHalf;
  if (value > graphic_low_extrem)
    value = graphic_low_extrem;


  //Pinta la linea solo a partir de que ya exista al menos 1 valor
  if (graphic_x_BPM > graphic_left_extrem + 1)
    tft.drawLine(graphic_x_BPM - 1, graphic_prevRead2, graphic_x_BPM, value, ILI9341_WHITE);

  //Wave refresh (barre pantalla pintando una linea)
  tft.drawLine(graphic_x_BPM + 1, graphic_high_extremHalf, graphic_x_BPM + 1, graphic_low_extrem, ILI9341_BLACK);


  graphic_prevRead2 = value;
  graphic_x_BPM++;

  delay(delay_time);

  if (graphic_x_BPM == graphic_right_extrem)
    graphic_x_BPM = graphic_left_extrem;
  SPI.end();
}





  //  count ++;
  //   if(count<WIDTH){
  //       Buffer[index] = valReadBPM;
  //       sum+= valReadBPM;
  //       avg = sum/count;
  //       index = nextIndex(index);
  //       Serial.print("count:");
  //       Serial.println(count);
  //       Serial.print("sum:");
  //       Serial.println(sum);
  //       Serial.print("average:");
  //       Serial.println(avg);
  //   }
  // else{
  //       lastValue = Buffer[index]; 
  //       Buffer[index] = valReadBPM;          
  //       sum-= lastValue;
  //       sum+= valReadBPM;
  //       avg = sum/WIDTH;
  //       index = nextIndex(index);
  //       Serial.println(avg);
  //   }