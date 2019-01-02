Benjamin Mouer
unisex bathroom
11/28/18

The purpose of this lab was to show how shared memory and semaphores can be used to solve problems of scheduling and mutual exclusion with real world examples. 

My program uses mainly shared memory to solve this problem. It creates instances of variables that a process will individually use (id, time, etc) and uses shared memory variables for variables that change throughout the program for different processes (num of stall available, if men/women are using the bathroom). It asks for the number of stalls and once it figures out if the arrival is a man or woman and the time of the arrival (with a corresponding sleep function), starts a new process. Once all of the processes are exited, the program outputs the totals data and terminates. Comments in the code provide further elaborations and explanations.


