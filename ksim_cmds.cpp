/*
  Seth Cram
  11/12/2021
  hw3: Kernel Simulator
  File: ksim_cmds.cpp

  File Description:
   -Determines function to run based on command.
   -Contains: add, exit, io-event, release, and wait functions
    Also contains some helper functions for these
*/

#include "ksim.h" //header file w/ prototypes, macros, globals

using namespace std; //for queues and strings

//run funct based on what shell built-in we found:
int KsimBuiltins( int i, char **argList )
{
  int FunctState; //to determine completion or failure

  //find what built in cmd it is and run its funct

  //check:  printf("Found builtin cmd.\n" );

  switch( i )
    {
    case 0: //add
      FunctState = Add_Funct( argList );

      break;

    case 1: //exit
      FunctState = Exit_Funct();

      break;

    case 2: //io-event
      FunctState = IOevent_Funct( argList );

      break;

    case 3: //query
      FunctState = Query_Funct( argList );

      break;

    case 4: //release
      FunctState = Release_Funct( );

      break;

    case 5: //step
      FunctState = Step_Funct( );

      break;

    case 6: //wait
      FunctState = Wait_Funct( argList );

      break;

    default: //fail-catch
      printf("Builtin command not found.\n");

      FunctState = -1; //failure

      break;
    }

  return FunctState;
}

/*
The <process-name> operand is required and can be composed of any 
 sequence of characters, but cannot include whitespace characters. If
 successful, this command will create a simulation process that will be
 placed into the New state and 32 ticks will pass, indicating the time the
 kernel consumed to create the new process. If a process named
 <process-name> already exists, your simulator must indicate
 that the process exists and not add any time to the tick counter. 
*/
int Add_Funct( char **argList )
{
  if( ArgumentPassed( argList[1], "add" ) == false)
    {
      return -2; //non-termination level error
    }

  //if proc name is 'all':
  if( strcmp( argList[1], "all" ) == 0 )
    {
      printf( "Opcode \"add\" can't have this proc name \"all\".\n" );

      return -1; //non-termination level error
    }

  //add new named proc to the 'New State' queue
  // but only if proc with same name doesn't already exist
  
  //create new proc:
  struct PCB newProc; 
  newProc.id = argList[ 1 ];
  newProc.state = "New";
  newProc.lastRunTick = 0; //could set to curr ticks for dif behavior

  //if process id found in kernel::
  if( SearchQueues( newProc.id ) == true )
    {
      cout << "process named \"" << newProc.id;
      cout << "\" is already being hosted.\n";

      return -2; //non-termination lvl error
    }
 
  //put new proc into aprop queue:
  NewProc_State.push( newProc );

  //cant use printf() with c++ strings

  tickCount += 32;

  return 0;
}

//search queues for if process name exists:
bool SearchQueues( string procName )
{
  //loops thru array of queues:
  for( int i = 0; i < QUEUE_ARRAY_LEN; i++ )
    {

      queue<struct PCB> currQueue = *queueArray[i]; //copy over queue

      //while queue not empty:
      while( !( currQueue.empty() ) ) //doesnt catch running proc
        {
          //testing: cout << "looking at: " << currQueue.front().id << endl;

	  //if front of queue's name is same as process name:
          if( currQueue.front().id == procName )
            {
	      return true;//name already exists
            }

          currQueue.pop(); //remove head entry

        }

    }
  return false; //no matching id
}

//checks if argument passed or not:
bool ArgumentPassed( char *arg, string Op  )
{
  //if no name for proc:
  if( arg == NULL )
    {
      //error msg:
      cout << "Opcode \"" << Op << "\" requires a process name.\n"; 

      return false;
    }

  return true;
}

//built-in exit funct:
int Exit_Funct()
{
  return 1; //ret termination lvl failure
}

/*
  this command is entered by the user to simulate that the i/o device
  specified by the operand <iodev-num> is ready. the operand <io-dev-num>
  is a number in the range [0..3]. since a process in this simulation can
  only be waiting on at most one i/o device, this will cause all processes
  that are waiting on this device to immediately become available and enter
  the ready state. if no processes are waiting on the specified device,
  ksim will emit a message of the form “no processes waiting on device
  <io-dev-num>” (where <io-dev-num> will be replaced with the device number
  specified by the user). this kernel consumes 1 tick when servicing this
  command.
*/
int IOevent_Funct( char **argList )
{
  //if passed in argument isn't valid:
  if( !DeviceValid( argList ) ) //error msg within
    {
      return -8; //fail
    }
  
  int devNum = atoi( argList[1] ); //convert dev # to int

  //if dev number out of range:
  if( devNum < 0 || 3 < devNum )
    {
      printf("Pass a device number (0-3) in.\n");

      return -8; //error
    }

  //use devNum as index of array of blocked queues

  //if blocked queue empty:
  if( blockedArray[devNum]->empty() )
    {
      printf("No process waiting on device %d.\n", devNum );

      return -1; //error
    }

  //pop all process in corresponding blocked queue off to Ready state:
  AllBlockedtoReady( devNum );
  
  tickCount += 1;

  return 0; //success
}

//moves all blocked proc's in devnNum's blocked array to ready state:
void AllBlockedtoReady( int devNum )
{
  //while blocked queue not empty:
  while( ! blockedArray[ devNum ]->empty() )
    {
      //copy pcb over:
      struct PCB currPCB = blockedArray[devNum]->front();

      //pop it off blocked queue:
      blockedArray[devNum]->pop();

      //change state to ready state:
      currPCB.state = "Ready";

      //dont have to clear blocked tick or dev waiting on

      //push it onto ready queue:
      Ready_State.push( currPCB );

      cout << "process \"" << currPCB.id
           << "\" moved from Blocked (iodev="
           << devNum << ") to Ready.\n";
    }
}

//checks if passed in device is valid:
bool DeviceValid( char **argList )
{
  //if only 1 arg:
  if( argList[1] == NULL)
    {
      printf("Pass a device number (0-3) in.\n");

      return false; //error
    }

  int i; //cnting var

  //check if passed in argument is a number:
  for(i = 0; isdigit( argList[1][i] ) == 1; i++ );

  //if last char isn't null, a char was entered:
  if( '\0' != argList[1][i] )
    {
      printf("You can't use a string or char as an argument. Pass (0-3).\n"
	     );

      return false;//error
    }

  return true;
}

/*
  this command is entered by the user to cause the currently running
  process to move from state running to state exit. this command takes no
  operands. if there is a process in the running state when this command
  is entered, the kernel consumes 32 ticks servicing this command. if no
  process is in the running state when this command is entered, the kernel
  consumes no ticks servicing this command.
*/
int Release_Funct( )
{

  //if no running proc: 
  if( Running_State.empty() )
    {
      printf("No process is currently running.\n");

      return -1; //error
    }

  //move running proc to exit state

  struct PCB runProc = Running_State.front(); 

  //change running proc's state and last run tick:
  runProc.state = "Exit";
  runProc.lastRunTick = tickCount;

  //push it onto exit queue:
  Exit_State.push( runProc );

  cout << "process \"" << runProc.id 
	<< "\" moved from Running to exit.\n"; 

  //pop running proc:
  Running_State.pop();

  tickCount += 32; 

  return 0; //success
}

/*
  This command is entered by the user to cause the currently running
  process to block waiting on the I/O device specified by the operand
  <io-dev-num>. The operand <io-dev-num> is a number in the range [0..3].
  When a process is blocked waiting for a device, it will wait for 1024
  ticks or until the user enters an io-event command signaling that the
  device is ready. Multiple processes can be waiting on the same I/O 
  device.If a process is currently in the Running state and operand
  <io-dev-num> is valid, the kernel consumes 1 tick for this command.
*/
int Wait_Funct( char **argList )
{
  //if passed in argument isn't valid:
  if( !DeviceValid( argList ) )
    {
      return -8; //fail
    }
  
  int devNum = atoi( argList[1] ); //convert dev # to int

  //if dev number out of range:
  if( devNum < 0 || 3 < devNum )
    {
      printf("Pass a device number (0-3) in.\n");

      return -8; //error
    }

  //if no running proc:
  if( Running_State.empty() )
    {
      printf("No process in the running state.\n");

      return -4; //error
    }

  //store running proc:
  struct PCB runningProc = Running_State.front();

  //remove running proc:
  Running_State.pop();

  //update device waiting on, blocked tick, last run tick, and state:
  runningProc.deviceWaitingOn = devNum;
  runningProc.blockedTick = tickCount;
  runningProc.lastRunTick = tickCount;
  runningProc.state = "Blocked";
  
  //add running proc to blocked queue:
  blockedArray[ devNum ]->push( runningProc );

  //msg:
  cout << "Process \"" << runningProc.id
       << "\" moved from Running (iodev="
       << devNum << ") to Blocked.\n";

  //add 1 tick if successful:
  tickCount+=1;

  return 0; //success
}
