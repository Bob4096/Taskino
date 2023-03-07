#ifndef TASKINO_h
#define TASKINO_h

#include <Arduino.h>

struct Task;

class Scheduler {
public:
  Scheduler(int16_t numTasks, bool use_priority_scheduling);
  void addTask(Task task);
  void addTask(uint16_t32_t arrival_time, int16_t priority, uint32_t period, int16_t deadline, int16_t id, void (*callback)(Task*));
  void pauseTask(int16_t int16_ternal_index);
  void terminateTask(int16_t int16_ternal_index);
  void runTasks();
  int16_t getTaskid(void (*callback)(Task*));
  int16_t getTaskIndexById(int16_t id);
  Task getTaskById(int16_t id);
  void modifyTask(int16_t id, uint16_t32_t arrival_time, int16_t priority, uint32_t period, int16_t deadline, void (*callback)(Task*));
  int16_t getTotalNumDeadlineMisses();
  int16_t getNumDeadlineMissesById(int16_t id);
  int16_t getCurrentTaskId();
  int16_t getStartTime();
  int16_t getInactivityTime();
private:
  Task* tasks;
  int16_t numTasks;
  bool use_priority_scheduling;
  int16_t current_task_id;
  void init(int16_t i);
  int16_t findNextTask();
  uint16_t32_t start_time;
  uint16_t32_t inactivity_time;
  bool sleeping;
  bool start;
};

struct Task {
  uint16_t32_t arrival_time;
  int16_t deadline;
  uint32_t period;
  int16_t priority;
  int16_t dmiss;
  bool paused;
  void (*callback)(Task*);
  int16_t int16_ternal_index;
  int16_t id;
  Scheduler* scheduler;
};

#endif