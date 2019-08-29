#include<Esplora.h>
void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
int slideValue = Esplora.readSlider();
Serial.println(slideValue);
if(slideValue>=0 && slideValue<=127)
{
  Esplora.writeRGB(255, 1.2*slideValue, 0);
  }
else if(slideValue>=128 && slideValue<=255)
{
  Esplora.writeRGB(255, 0.8*slideValue, 0);
  }  

delay(1000);
}
