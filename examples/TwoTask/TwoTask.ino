#include <Taskino.h>


#define max_tasks 10

// false: rate monotonic, true: priority scheduling
Scheduler Sched(max_tasks, false);

void setup() {

  Serial.begin(9600);

 // void addTask(unsigned long arrival_time, int priority, long period, int deadline, int id, void (*callback)());

  Sched.addTask(millis(), 50, 400, 300, 1, &print);
  Sched.addTask(millis(), 30, 2500, 1500, 2, &print);
}

void loop() {
  // run the task scheduler
  Sched.runTasks();
}


void print(Task* t) {
 int id = t->id;
 Serial.print("Task ");
 Serial.println(id);
}
