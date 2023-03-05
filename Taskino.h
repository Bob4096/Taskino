#ifndef TASKINO_h
#define TASKINO_h

#include <Arduino.h>

struct Task;

class Scheduler {
public:
  Scheduler(int numTasks, bool use_priority_scheduling);
  void addTask(Task task);
  void addTask(unsigned long arrival_time, int priority, long period, int deadline, int id, void (*callback)(Task*));
  void pauseTask(int internal_index);
  void terminateTask(int internal_index);
  void runTasks();
  int getTaskid(void (*callback)(Task*));
  int getTaskIndexById(int id);
  Task getTaskById(int id);
  void modifyTask(int id, unsigned long arrival_time, int priority, long period, int deadline, void (*callback)(Task*));
  int getTotalNumDeadlineMisses();
  int getNumDeadlineMissesById(int id);
  int getCurrentTaskId();
  int getStartTime();
  int getInactivityTime();
private:
  Task* tasks;
  int numTasks;
  bool use_priority_scheduling;
  int current_task_id;
  void init(int i);
  int findNextTask();
  unsigned long start_time;
  unsigned long inactivity_time;
  bool sleeping;
  bool start;
};

struct Task {
  unsigned long arrival_time;
  int deadline;
  long period;
  int priority;
  int dmiss;
  bool paused;
  void (*callback)(Task*);
  int internal_index;
  int id;
  Scheduler* scheduler;
};

#endif