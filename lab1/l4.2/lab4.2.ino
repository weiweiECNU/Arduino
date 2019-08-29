#include <Esplora.h>
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  int soundValue=Esplora.readMicrophone();
  Serial.println(soundValue);// put your main code here, to run repeatedly:
if(soundValue ==0)
    {
       Esplora.writeRGB(0, 0, 0);
      }
else if(soundValue < 50)
    {
       Esplora.writeRGB(0, 255, 0);
      }
    else if(soundValue < 100)
    {
       Esplora.writeRGB(255, 255, 0);
      }
      else
    {
       Esplora.writeRGB(255, 0, 0);
      }
delay(100);
}
