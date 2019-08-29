#include <Esplora.h> 

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  Esplora.writeRGB(255, 0, 0); // turn on LED
  delay(3000);                     // wait for 1 second
  Esplora.writeRGB(0, 255, 0); 
  delay(3000);                     // wait for 1 second
  Esplora.writeRGB(0, 0, 255); 
  delay(1000);                     // wait for 1 second
}

