#include <Arduino.h> //necessary for platformio

void setup() {

  pinMode(16, OUTPUT); //D0
  pinMode(5, OUTPUT);  //D1
  pinMode(4, OUTPUT);  //D2
  pinMode(0, OUTPUT);  //D3
  
}

// the loop function runs over and over again forever
void loop() {
 
  digitalWrite(16, LOW);   // LOW lights optoisolator LED to ground - turns on relay
  delay(2000);             // wait for a 2 seconds
 
  digitalWrite(16, HIGH);  // HIGH kills optoisolator LED - turns OFF relay
  delay(2000);             // wait for a 2 seconds
 
  
  digitalWrite(5, LOW);    // LOW lights optoisolator LED to ground - turns on relay
  delay(2000);             // wait for a 2 seconds
 
  digitalWrite(5, HIGH);   // HIGH kills optoisolator LED - turns OFF relay
  delay(2000);             // wait for a 2 seconds
 
 
  digitalWrite(4, LOW);    // LOW lights optoisolator LED to ground - turns on relay
  delay(2000);             // wait for a 2 seconds
 
  digitalWrite(4, HIGH);   // HIGH kills optoisolator LED - turns OFF relay
  delay(2000);             // wait for a 2 seconds

 
  digitalWrite(0, LOW);    // LOW lights optoisolator LED to ground - turns on relay
  delay(2000);             // wait for a 2 seconds
  
  digitalWrite(0, HIGH);   // HIGH kills optoisolator LED - turns OFF relay
  delay(2000);             // wait for a 2 seconds
 
}
