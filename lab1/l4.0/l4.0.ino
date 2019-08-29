#include <Esplora.h>
void setup() {
 //unsigned int readLightSensor(); 
//unsigned int readMicrophone();
//int readTemperature(0); 
// scale = 0 for Celcius, scale = 1 for Farenheit
//int readAccelerometer(); 
// axis = {0,1,2} for x, y and z-axis respectively
 // put your setup code here, to run once:
 Serial.begin(9600);

}

void loop() {
  int lightValue = Esplora.readLightSensor();
  Serial.println(lightValue);
  if(lightValue < 600)
    {
       Esplora.writeRGB(255, 255, 255);
      }
   else
   {
      Esplora.writeRGB(0, 0, 0);
    }
  delay(100);// put your main code here, to run repeatedly:

}
