void setup() {
  // Initiate the serial communication.
  // Set the speed to 9600 baud (bits per second).
  // See http://arduino.cc/en/Serial/Begin for more details.
  Serial.begin(9600);
}
void loop() {
  // Write a message to the serial port
  Serial.println("Hello, Aaron");
  Serial.print("How are you today?\n");
  // Pause for 1 second
  Serial.println("42");
  Serial.println(42);
  delay(1000);
}
