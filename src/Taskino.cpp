#include "Taskino.h"

Scheduler::Scheduler(int16_t numTasks, bool use_priority_scheduling) {
  this->tasks = new Task[numTasks];
  this->numTasks = numTasks;
  this->use_priority_scheduling = use_priority_scheduling;
  this->current_task_id = -1;
  this->start_time = -1;
  this->sleeping = true;
  this->start = true;

  for (int16_t i = 0; i < numTasks; i++) {
    init(i);
  }
}

void Scheduler::init(int16_t i) {

  tasks[i].arrival_time = 0;
  tasks[i].period = 0;
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
  for (int16_t i = 0; i < numTasks; i++) {
    if (tasks[i].id < 0) {
      tasks[i] = task;
      tasks[i].internal_index = i;
      break;
    }
  }
}

void Scheduler::addTask(uint32_t arrival_time, int16_t priority, uint32_t period, int16_t deadline, int16_t id, void (*callback)(Task*)) {
  for (int16_t i = 0; i < numTasks; i++) {
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


void Scheduler::pauseTask(int16_t internal_index) {
  tasks[internal_index].paused = true;
}

void Scheduler::terminateTask(int16_t internal_index) {
  init(internal_index);
}

int16_t Scheduler::getCurrentTaskId() {
  return current_task_id;
}

int16_t Scheduler::getStartTime() {
  return start_time;
}

int16_t Scheduler::getInactivityTime() {
  return inactivity_time;
}

  int16_t Scheduler::getTaskIndexById(int16_t id) {
    for (int16_t i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return i;  // return the index of the task
      }
    }
    return -1;  // return -1 if the task is not found
  }

  Task Scheduler::getTaskById(int16_t id) {
    for (int16_t i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return tasks[i];  // return a copy of the Task object
      }
    }
    return Task();  // return an empty Task object if the task is not found
  }

  void Scheduler::modifyTask(int16_t id, uint32_t arrival_time, int16_t priority, uint32_t period, int16_t deadline, void (*callback)(Task*)) {
    for (int16_t i = 0; i < numTasks; i++) {
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
  int16_t Scheduler::getTotalNumDeadlineMisses() {
    int16_t total = 0;
    for (int16_t i = 0; i < numTasks; i++) {
      if (tasks[i].id != -1) {  // only count deadline misses for tasks that exist
        total += tasks[i].dmiss;
      }
    }
    return total;
  }

  // Returns the number of times a task with the given id has missed its deadline
  int16_t Scheduler::getNumDeadlineMissesById(int16_t id) {
    for (int16_t i = 0; i < numTasks; i++) {
      if (tasks[i].id == id) {
        return tasks[i].dmiss;
      }
    }
    return -1;  // return -1 if the task is not found
  }

  int16_t Scheduler::findNextTask() {
    // Initialize variables to track the index, priority, and time to deadline of the next task to execute
    int16_t task_index = -1;
    int16_t task_priority = -1;
    uint32_t task_minTimeToNextDeadline = 999999;

    // Get the current time in milliseconds
    uint32_t current_time = millis();

    // Iterate through all tasks to find the next task to execute
    for (int16_t i = 0; i < numTasks; i++) {
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
          int16_t time_to_deadline = tasks[i].deadline + tasks[i].arrival_time - current_time;
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
    int16_t task_index = findNextTask();

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
      uint32_t current_time = millis();
      if (tasks[task_index].arrival_time + tasks[task_index].deadline < current_time) {
        tasks[task_index].dmiss++;
      }

      sleeping = true;
      // Reset the current task ID to -1
      current_task_id = -1;
    } else {

      if (sleeping) {  // avoid multiple sleep
        int16_t task_index = -1;
        uint32_t min_arrival_time = 9999999;
        uint32_t current_time = millis();
        for (int16_t i = 0; i < numTasks; i++) {
          if (tasks[i].arrival_time < min_arrival_time && tasks[i].arrival_time > current_time && tasks[i].id != -1) {
            task_index = i;
            min_arrival_time = tasks[i].arrival_time;
          }
        }

        // Calculate the sleep time based on the arrival time of the closest task
        uint32_t sleep_time = 0;
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
