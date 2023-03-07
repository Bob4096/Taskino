#include "Taskino.h"

Scheduler::Scheduler(int numTasks, bool use_priority_scheduling) {
  this->tasks = new Task[numTasks];
  this->numTasks = numTasks;
  this->use_priority_scheduling = use_priority_scheduling;
  this->current_task_id = -1;
  this->start_time = -1;
  this->sleeping = true;
  this->start = true;

  for (int i = 0; i < numTasks; i++) {
    init(i);
  }
}

void Scheduler::init(int i) {

  tasks[i].arrival_time = 0;
  tasks[i].period = -7;
  tasks[i].priority = 0;
  tasks[i].deadline = 0;
  tasks[i].dmiss = 0;
  tasks[i].paused = false;
  tasks[i].callback = nullptr;
  tasks[i].internal_index = -1;
  tasks[i].id = -1;
  tasks[i].scheduler = this;
}


void Scheduler::addTask(Task task) {
  for (int i = 0; i < numTasks; i++) {
    if (tasks[i].id < 0) {
      tasks[i] = task;
      tasks[i].internal_index = i;
      break;
    }
  }
}

void Scheduler::addTask(unsigned long arrival_time, int priority, long period, int deadline, int id, void (*callback)(Task*)) {
  for (int i = 0; i < numTasks; i++) {
    if (tasks[i].id < 0) {
      tasks[i].arrival_time = arrival_time;
      tasks[i].priority = priority;
      tasks[i].period = period;
      tasks[i].deadline = deadline;
      tasks[i].internal_index = i;
      tasks[i].id = id;
      tasks[i].callback = callback;
      break;
    }
  }
}


void Scheduler::pauseTask(int internal_index) {
  tasks[internal_index].paused = true;
}

void Scheduler::terminateTask(int internal_index) {
  init(internal_index);
}

int Scheduler::getCurrentTaskId() {
  return current_task_id;
}

int Scheduler::getStartTime() {
  return start_time;
}

int Scheduler::getInactivityTime() {
  return inactivity_time;
}

  int Scheduler::getTaskIndexById(int id) {
    for (int i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return i;  // return the index of the task
      }
    }
    return -1;  // return -1 if the task is not found
  }

  Task Scheduler::getTaskById(int id) {
    for (int i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return tasks[i];  // return a copy of the Task object
      }
    }
    return Task();  // return an empty Task object if the task is not found
  }

  void Scheduler::modifyTask(int id, unsigned long arrival_time, int priority, long period, int deadline, void (*callback)(Task*)) {
    for (int i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        tasks[i].arrival_time = arrival_time;
        tasks[i].priority = priority;
        tasks[i].period = period;
        tasks[i].deadline = deadline;
        tasks[i].callback = callback;

        break;  // stop searching once the task is found
      }
    }
  }

  // Returns the total number of times all tasks have missed their deadlines
  int Scheduler::getTotalNumDeadlineMisses() {
    int total = 0;
    for (int i = 0; i < numTasks; i++) {
      if (tasks[i].id != -1) {  // only count deadline misses for tasks that exist
        total += tasks[i].dmiss;
      }
    }
    return total;
  }

  // Returns the number of times a task with the given id has missed its deadline
  int Scheduler::getNumDeadlineMissesById(int id) {
    for (int i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return tasks[i].dmiss;
      }
    }
    return -1;  // return -1 if the task is not found
  }

  int Scheduler::findNextTask() {
    // Initialize variables to track the index, priority, and time to deadline of the next task to execute
    int task_index = -1;
    int task_priority = -1;
    unsigned long task_minTimeToNextDeadline = 999999;

    // Get the current time in milliseconds
    unsigned long current_time = millis();

    // Iterate through all tasks to find the next task to execute
    for (int i = 0; i < numTasks; i++) {
      // Only consider tasks that are not paused and have an arrival time that has already passed
      if (!tasks[i].paused && tasks[i].arrival_time <= current_time && tasks[i].id != -1) {
        // If using priority scheduling, find the task with the highest priority
        if (use_priority_scheduling) {
          if (task_priority < tasks[i].priority) {
            task_index = i;
            task_priority = tasks[i].priority;
          }
          // If using rate monotonic scheduling, find the task with the earliest deadline
        } else {
          // Calculate the time until the task's deadline
          int time_to_deadline = tasks[i].deadline + tasks[i].arrival_time - current_time;
          if (task_minTimeToNextDeadline > time_to_deadline) {
            task_index = i;
            task_minTimeToNextDeadline = time_to_deadline;
          }
        }
      }
    }
    return task_index;
  }

  void Scheduler::runTasks() {

    if (start) {
      start = false;
      start_time = millis();
    }
    // Find the index of the next task to execute
    int task_index = findNextTask();

    // If a suitable task was found, execute its callback and update its arrival time
    if (task_index >= 0) {
      // Set the current task ID to the ID of the chosen task
      current_task_id = tasks[task_index].id;

      // Execute the task's callback
      Task* t = &tasks[task_index];
      tasks[task_index].callback(t);

      // Update the task's arrival time
      tasks[task_index].arrival_time += tasks[task_index].period;

      // Check if the task missed its deadline
      unsigned long current_time = millis();
      if (tasks[task_index].arrival_time + tasks[task_index].deadline < current_time) {
        tasks[task_index].dmiss++;
      }

      sleeping = true;
      // Reset the current task ID to -1
      current_task_id = -1;
    } else {

      if (sleeping) {  // avoid multiple sleep
        int task_index = -1;
        long min_arrival_time = 9999999;
        unsigned long current_time = millis();
        for (int i = 0; i < numTasks; i++) {
          if (tasks[i].arrival_time < min_arrival_time && tasks[i].arrival_time > current_time && tasks[i].id != -1) {
            task_index = i;
            min_arrival_time = tasks[i].arrival_time;
          }
        }

        // Calculate the sleep time based on the arrival time of the closest task
        unsigned long sleep_time = 0;
        if (task_index >= 0) {
          sleep_time = min_arrival_time - current_time;


          inactivity_time += sleep_time;
          // Use the sleep time to pause the scheduler
          delay(min_arrival_time - millis());
        }
        sleeping = false;
      }
    }
  }
