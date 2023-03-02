/*
  Seth Cram
  11/12/2021
  hw3: Kernel Simulator
  File: ksim_step.cpp

  File Description:
   -Contains: step function and a variety of helper functions

  Notes:
   -long and unsigned keywords can be in either order
*/

#include "ksim.h" //header file w/ prototypes, macros

using namespace std; //for queues and strings

/*
  this command is entered by the user to cause ksim to step to perform
  housekeeping duties and advance the simulation. this command has no 
  operands. the housekeeping duties performed during this command are:
  ...
 */
int Step_Funct( )
{
  //if exit queue not empty:
  if( !Exit_State.empty() )
    {
      //del all proc's in exit queue:
      DeleteExitProcs();
    }

  //move proc from new to ready:
  NewtoReady();

  //check if IO-waiting proc ready to come off:
  TimeoutCheck();

  //move from running to ready curr run proc:
  RunningtoReady();
  
  //if proc moved from ready to running:
  if( ReadytoRunning() == 0 )
    {
      tickCount+=256;
    }
  // proc not moved from ready to running:
  else
    {
      tickCount+=1;
    }
  
  return 0; //success
}

/*
  remove from the system all processes that are in the exit state. once a 
  process is removed from the system it can no longer be queried in the
  simulator. for each process that is removed from the system, ksim will
  emit a message of the form “process <process-name> is banished to the 
  void.
*/
void DeleteExitProcs()
{
  //while exit queue not empty:
  while( !Exit_State.empty() )
    {
      //print out delete msg:
      cout << "Process \"" << Exit_State.front().id 
	   << "\" is banished to the void.\n";

      //delete head proc:
      Exit_State.pop();
    }
}

/*
  advance at most 1 process in the new state into the ready state. if more
  than 1 process exists in the new pool, the process with the oldest
  creation time is advanced from new to ready.
 */
void NewtoReady()
{
  //dont do anything if new proc queue empty:
  if( NewProc_State.empty() )
    {
      return;
    }

  //copy new proc's head proc:
  struct PCB currProc = NewProc_State.front();

  //remove head of new proc state:
  NewProc_State.pop();

  //change proc's state:
  currProc.state = "Ready";

  //add removed proc into ready queue:
  Ready_State.push( currProc );

  cout << "process \"" << currProc.id
       << "\" moved from New to Ready.\n";
}

/*
  advance at most 1 process from each i/o device in the blocked state into
  the ready state. this can only occur if the process that is blocked has
  waited for the specified number of ticks associated with an i/o device
  wait (1024 ticks).
 */
void TimeoutCheck( )
{
  //queue<struct pcb> blockedarrayph[] = *blockedarray;

  //loop thru all blocked arrays:
  for( int i = 0; i < BLOCKED_QUEUES; i++)
    {
      queue<struct PCB> currQueue = *blockedArray[i];

      //if blocked queue isnt empty:
      if( !currQueue.empty() )
	{
	  struct PCB currProc = currQueue.front();

	  //check if time to unblock head proc:
	  if( tickCount - currProc.blockedTick >= BLOCKED_TICK_LIMIT )
	    {
	      //change state:
	      currProc.state = "Ready";
	      
	      //dont needa reset blocked tick

	      //remove from blocked queue:
	      blockedArray[i]->pop();

	      //add to ready queue:
	      Ready_State.push( currProc );

	      cout << "process \"" << currProc.id
		   << "\" moved from Blocked (iodev="
		   << currProc.deviceWaitingOn << ") to Ready.\n";
	    }
	}
    }
}
/*
  if a process is currently in the running state, remove the process from
  the processor and place it in the ready state. 
*/
void RunningtoReady( )
{
  //testing: printf("made it into runningtoready funct\n");

  //do nothing if no running proc:
  if( Running_State.empty() )
    {
      return;
    }

  struct PCB currProc = Running_State.front();

  //update state and last run time:
  currProc.state = "Ready";
  currProc.lastRunTick = tickCount;

  //push running proc onto ready queue:
  Ready_State.push( currProc );

  cout << "Process \"" << currProc.id
       << "\" moved from Running to Ready.\n";

  //clear running proc:
  Running_State.pop();
}

/*
  If any processes are in the Ready state, dispatch a process to the
  processor by advancing it from the Ready state to the Running state.
  If more than 1 process is in the Ready state, the process that has waited
  the longest amount of time since last being run is advanced from the
  Ready state to the Running state.
*/
int ReadytoRunning()
{
  //returns success or failure (0 or nonzero)

  //if ready queue empty:
  if( Ready_State.empty() )
    {
      return -1; //fail
    }

  //testing: printf( "Curr Ready queue size: %d", Ready_State.size() );

  //if ready state is only 1 proc:
  if( Ready_State.size() == 1 ) 
    {
      struct PCB currProc = Ready_State.front();
  
      //remove from ready queue:
      Ready_State.pop();

      //set proc to running:
      currProc.state = "Running";
  
      //add running proc:
      Running_State.push( currProc );

      cout << "Process \"" << currProc.id
	   << "\" moved from Ready to Running.\n";
    }
  //ready state has multiple proc's:
  else
    {
      //add proc with longest time since last run
      
      //loop thru ready queue and push top onto bot
      // unless top has lower tick since last run

      //move oldestr proc to last run into ready:
      MoveOldesttoRun();

    }
  return 0; //success
}

//move oldestr proc to last run into ready:
void MoveOldesttoRun( )
{
  //store queue head:
  struct PCB longestProcSR = Ready_State.front();

  //remove queue head:
  Ready_State.pop();

  //loop thru all elements in ready queue:
  for( int i = 0; i < Ready_State.size(); i++ )
    {
      //store highest element:
      struct PCB currProc = Ready_State.front();

      //if curr head of list been waiting longer:
      if( currProc.lastRunTick < longestProcSR.lastRunTick )
	{
	  //push prev highest element onto queue's back:
	  Ready_State.push( longestProcSR );

	  //make curr head of queue longest proc waiting:
	  longestProcSR = currProc;
	}
      //curr head hasnt been waiting longer:
      else
	{
	  //rotate curr head to back of queue:
	  Ready_State.push( currProc );
	}

      //remove head of the queue:
      Ready_State.pop();
    }

  //set proc to running:
  longestProcSR.state = "Running";

  //push longest proc onto run queue:
  Running_State.push( longestProcSR );

  cout << "Process \"" << longestProcSR.id
       << "\" moved from Ready to Running.\n";
}
