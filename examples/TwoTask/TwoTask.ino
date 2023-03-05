#include <Taskino.h>


#define max_tasks 10

// false: rate monotonic, true: priority scheduling
Scheduler Sched(max_tasks, false);

void setup() {

  Serial.begin(9600);

 // void addTask(unsigned long arrival_time, int priority, long period, int deadline, int id, void (*callback)());

  Sched.addTask(millis(), 50, 500, 300, 1, &print1);
  Sched.addTask(millis(), 30, 2000, 1500, 2, &print2);
}

void loop() {
  // run the task scheduler
  Sched.runTasks();
}

void print1(Task* t) {
 Serial.println("Task 1");
}

void print2(Task* t) {
 Serial.println("Task 2");
}
