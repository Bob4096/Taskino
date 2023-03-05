# Taskino
Taskino is a C++ library for Arduino that implements a scheduler for managing multiple tasks.

The scheduler is implemented using a struct called "Task", which stores information about each task (e.g., arrival time, period, deadline, priority, callback function, etc.). The tasks are stored in an array of Task structs, with each task identified by a unique id.

The Scheduler class has several member functions for managing the tasks. The addTask function adds a new task to the scheduler, either by passing a Task struct directly or by passing the individual parameters (arrival time, period, deadline, priority, id, callback function). The pauseTask function pauses a specific task, while the terminateTask function terminates a task (i.e., removes it from the scheduler). The runTasks function is the main function that runs the tasks, and it uses a scheduling algorithm (either rate monotonic or priority scheduling, depending on the value of the use_priority_scheduling flag) to determine which task to run next.

Other member functions include functions for getting the current task id, start time, and inactivity time, as well as functions for getting the index or Task struct for a specific task id. There are also functions for modifying an existing task (by id) and for getting information about the number of deadline misses (i.e., when a task misses its deadline) for a specific task or for all tasks.

partial TODO list
[] check if array of task is full
[] avoid that tasks have same id ?
[] check if deadline is higher than period
[] task to execute once
[] manage arrival time to avoid that all tasks start at the same time (spacially if their periods are multiples)
[] make an array only with active tasks
