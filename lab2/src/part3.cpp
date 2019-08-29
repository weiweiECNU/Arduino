#include <MySignals.h>

uint8_t coming_value;
uint8_t circular_queue[1024];
uint16_t index=0;
#define QUEUESIZE 1024 

uint32_t sum=0;
uint16_t count=0;

void setup()
{
 Serial.begin(115200);
 MySignals.begin();
 MySignals.initSensorUART();
 MySignals.enableSensorUART(PULSIOXIMETER_MICRO);
}
 
void loop()
{
    if (MySignals.getPulsioximeterMicro() == 1) {
        coming_value = MySignals.pulsioximeterData.BPM;
        count ++;
        updateSum(sum, count);
        circular_queue[index] = coming_value;
        updateIndex(index);
        printLog(coming_value, count, sum);
}
}

void updateIndex(uint16_t &i){
    if (i == QUEUESIZE - 1)
        i = 0;
    else 
        i += 1;
}

void updateSum(uint32_t &sum, uint16_t count){
    if (count > QUEUESIZE)
        sum -= circular_queue[index];
    sum += coming_value;
}

void printLog(uint8_t coming_value, uint16_t count, uint32_t sum;){
    Serial.println("BPM:");
    Serial.println(coming_value);
    Serial.println("Count:");
    Serial.println(count);
    Serial.println("Sum:");
    Serial.println(sum);
    Serial.print("Average:");
    Serial.println(sum/count);
}