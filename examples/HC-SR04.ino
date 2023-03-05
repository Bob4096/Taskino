#include <taskino.h>

const int trigPin = 28;
const int echoPin = 18;

#define max_tasks 10

Scheduler Sched(max_tasks, false);

// variables to store distance and duration of sound wave
volatile long duration = 40;
volatile bool pulseReceived = false;
volatile unsigned long pulseStartTime = 0;
float distance;

void setup() {
  // initialize serial communication
  Serial.begin(9600);

  // define pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // attach interrupt to echo pin
  attachInterrupt(digitalPinToInterrupt(echoPin), pulseHandler, CHANGE);

  // add task to measure distance
  Sched.addTask(millis(), 10, 500, 300, 1, &measureDistance);
  Sched.addTask(millis(), 30, 2000, 1500, 777, &printInfo);
}

void loop() {
  // run the task scheduler
  Sched.runTasks();
}

void measureDistance(Task* t) {
  if (pulseReceived) {
    pulseReceived = false;

    // convert the duration to distance in cm
    distance = float(duration) / 58.2;

    // print the distance to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // send another pulse to the sensor to start the measurement
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  }
}

void pulseHandler() {

  if(!pulseReceived){
  if (digitalRead(echoPin) == HIGH) {
    pulseStartTime = micros();
  } else {
    duration = micros() - pulseStartTime;
    pulseReceived = true;
  }
  }
}



void printInfo(Task* t) {
  
  Serial.print(F("Total deadline misses:"));
  Scheduler* S = t->scheduler;
  Serial.println(S->getTotalNumDeadlineMisses());


}
