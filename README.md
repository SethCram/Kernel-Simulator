# ksim by Seth Cram

  ksim is a Kernel Simulator that models the movement of processes inside
   the operating system. It operates off of the five-state process model.   

  Usage Instructions:
  - Compile: make
  - Run: ./ksim

  - add <process-name>
    - Adds a process to the New state within the Kernel.
    - All added processes have the same priority.

  - exit
    - Terminates the simulation.

  - io-event <io-device-number>
    - Simulate the io event specified occures.
    - Only four options: 0-3
    
  - query <process-name>
    - Can be done with no argument or 'all' to get information on every process currently within the kernel. 
    - Otherwise, displays information about specified process.

  - release
    - Moves process in running state to exit state.

  - step
    - Causes the kernal to advance the simulation.
    - Removes all processes in Exit state.
    - Moves 1 process from New to Ready state.
    - Moves process from Blocked to Ready if 1024 ticks have passed.
    - Moves process in Running to Ready state.
    - Longest since last run process in Ready state moved to Running.

  - wait <io-device-number>
    - Moves Running process to Blocked state.
    - Only four options: 0-3

  Look within a file's header to learn its specifics.  
