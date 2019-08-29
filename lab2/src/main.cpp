#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h"

Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

uint16_t spo2_graphic_x = 0;
uint16_t spo2_graphic_prev_read;
uint16_t bpm_graphic_x = 0;
uint16_t bpm_graphic_prev_read;

uint16_t spo2_valRead;
uint16_t bpm_valRead;

#define GRAPHIC_LOW_EXTREM_SPO2 115
#define GRAPHIC_HIGH_EXTREM_SPO2 0
#define GRAPHIC_LOW_EXTREM_BPM 230
#define GRAPHIC_HIGH_EXTREM_BPM 120

#define GRAPHIC_LEFT_EXTREM 0
#define GRAPHIC_RIGHT_EXTREM 320

#define INDEX_POSX 10
#define INDEX_SIZE 4

void setup()
{
  MySignals.begin();
  MySignals.initSensorUART();
  MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
}

void loop()
{
  if (MySignals.getPulsioximeterMicro() == 1)
  {
    spo2_valRead = MySignals.pulsioximeterData.O2;
    bpm_valRead = MySignals.pulsioximeterData.BPM;

    tft.drawString("SP02:", INDEX_POSX, GRAPHIC_HIGH_EXTREM_SPO2 + 30, INDEX_SIZE);
    tft.drawNumber(spo2_valRead, INDEX_POSX + 90, GRAPHIC_HIGH_EXTREM_SPO2 + 30, INDEX_SIZE);
    spo2_valRead = map(spo2_valRead, 0, 100, GRAPHIC_HIGH_EXTREM_SPO2, GRAPHIC_LOW_EXTREM_SPO2);
    printGraphic(spo2_valRead, 0, GRAPHIC_HIGH_EXTREM_SPO2, GRAPHIC_LOW_EXTREM_SPO2, spo2_graphic_x, spo2_graphic_prev_read);

    tft.drawString("BPM:", INDEX_POSX, GRAPHIC_HIGH_EXTREM_BPM + 30, INDEX_SIZE);
    tft.drawNumber(bpm_valRead, INDEX_POSX + 90, GRAPHIC_HIGH_EXTREM_BPM + 30, INDEX_SIZE);
    bpm_valRead = map(bpm_valRead, 50, 200, GRAPHIC_HIGH_EXTREM_BPM, GRAPHIC_LOW_EXTREM_BPM);
    printGraphic(bpm_valRead, 0, GRAPHIC_HIGH_EXTREM_BPM, GRAPHIC_LOW_EXTREM_BPM, bpm_graphic_x, bpm_graphic_prev_read);
    delay(1000);
  }
}

void printGraphic(uint16_t value, uint8_t delay_time, uint16_t high_extrem, uint16_t low_extrem, uint16_t &graphic_x, uint16_t &graphic_prev_read)
/*
 * We reuse the printGraphic function of TFT example in the tutorial of MySignals HW v2 at 6.1.2.4.
 * Sweep screen and painting a line presenting the data sent by sensor.
 * 
 * value : The data sent and mapped.
 * delay_time : The time waiting for next data.
 * high_extrem : The high extrem posY of the graphic in the screen.
 * low_extrem : The low extrem posY of the graphic in the screen.
 * graphic_x ：The posX of the point drawn in the graphic. Add 1 when a new data enter.
 * graphic_prev_read: The value of last value. Used to draw a line between last data and the new one.
*/
{
  if (value < high_extrem)
    value = high_extrem;
  if (value > low_extrem)
    value = low_extrem;

  //Paint the line only after at least 1 value already exists
  if (graphic_x > GRAPHIC_LEFT_EXTREM + 1)
    tft.drawLine(graphic_x - 1, graphic_prev_read, graphic_x, value, ILI9341_WHITE);

  //Wave refresh (sweep screen painting a line)
  tft.drawLine(graphic_x + 1, high_extrem, graphic_x + 1, low_extrem, ILI9341_BLACK);

  graphic_prev_read = value;
  graphic_x++;

  delay(delay_time);

  if (graphic_x == GRAPHIC_RIGHT_EXTREM)
    graphic_x = GRAPHIC_LEFT_EXTREM;
  SPI.end();
}

// void printIndex( const char index[], uint16_t high_extrem, uint16_t low_extrem, uint16_t valRead)
// {
//     tft.drawString(index, INDEX_POSX, high_extrem + 30, INDEX_SIZE);
//     tft.drawNumber(valRead, INDEX_POSX + 90, high_extrem + 30, INDEX_SIZE);
<<<<<<< HEAD
// }
=======
// }
>>>>>>> 8ea8cec822c5b3fc8c76a3e83c6bc93be6504b04
