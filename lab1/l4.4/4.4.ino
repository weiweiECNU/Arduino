#include <Esplora.h>

void setup()
{
 Serial.begin(9600);
}

void loop()
{
 int x_axis = Esplora.readAccelerometer(X_AXIS);
 int y_axis = Esplora.readAccelerometer(Y_AXIS);
 int z_axis = Esplora.readAccelerometer(Z_AXIS);
if (x_axis>=5 && x_axis<=10)
{
  Esplora.writeRGB(0, 255, 0);
  }
else if (x_axis>=50)
{
  Esplora.writeRGB(255, 0, 0);
  }
else
{
   Esplora.writeRGB( (x_axis-7) * 4, 255, 0);
  }



//if(x_axis>=5 && x_axis<=10 && y_axis>=10 && y_axis<=15 && z_axis>=120 && z_axis<=127)
//{
  //Esplora.writeRGB(0, 255, 0);
  //}
  //if(x_axis>=50 && x_axis<=60)
//  {
    
   //  Esplora.writeRGB(255, , 0);
 // }
  //if(y_axis >=75 && y_axis <=85)
// {
 //  Esplora.writeRGB(255, 153, 0);
 // }
    
 Serial.print("x: ");
 Serial.print(x_axis);
 Serial.print("\ty: ");
 Serial.print(y_axis);
 Serial.print("\tz: ");
 Serial.println(z_axis);

 delay(100);
}
