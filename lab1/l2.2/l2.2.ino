#include <Esplora.h> 

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  Esplora.writeRGB(255, 0, 0); // RED
  delay(1000);                     // wait for 1 second
  Esplora.writeRGB(255, 153, 0); // orange
  delay(1000); 
   Esplora.writeRGB(255, 255, 0); // yellow
  delay(1000); 
  
  Esplora.writeRGB(0, 255, 0); //Green
  delay(1000);                     // wait for 1 second
 
  
  Esplora.writeRGB(0, 0, 255);  // Blue
  delay(1000);                     // wait for 1 second
   Esplora.writeRGB(102, 102, 204); // indigo
  delay(1000);                     // wait for 1 second
   Esplora.writeRGB(153, 51, 255); // violet
  delay(1000);                     // wait for 1 second
}
