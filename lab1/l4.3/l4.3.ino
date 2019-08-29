#include <Esplora.h>
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  int tempValue=Esplora.readTemperature(0);
  Serial.println(tempValue);// put your main code here, to run repeatedly:
if(tempValue <=20)
    {
       Esplora.writeRGB(153, 51, 255);
      }
else 
    {
       Esplora.writeRGB(255, 0, 0);
      }
    
delay(1000);
}
