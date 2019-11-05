//#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_AS.h>
#include <MySignals.h>
#include "Wire.h"
#include "SPI.h"
//
//Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(TFT_CS, TFT_DC);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  MySignals.begin();
  MySignals.initSnore();
  MySignals.initBodyPosition();


}

void loop()
{

//   tft.drawString("Sending to server...", 0, 60, 2);

  float temperature = MySignals.getTemperature();
  String sent = String(temperature);

  Serial.println(sent);
  //Serial.flush();
  float snore = MySignals.getSnore(VOLTAGE);
  String sent2 = String(snore);
  uint8_t position = MySignals.getBodyPosition();
  String sent3 = String(position);
  Serial.println(sent + "_" + sent2 + "_" + sent3);
  Serial.flush();
  delay(1000);
}
