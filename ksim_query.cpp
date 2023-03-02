/*
  Seth Cram
  11/12/2021
  hw3: Kernel Simulator
  File: ksim_query.cpp

  File Description:
   -Contains: query function and its helper functions

  Notes:
   -dont pass pointers by memory location (&)
*/

#include "ksim.h" //header file w/ prototypes, macros, globals

using namespace std; //for queues and strings

/*
   this command is entered by the user to cause ksim to print information
   regarding the state of process <process-name>. if the operand 
   <process-name> is “all” (without quotes) or is not present on the
   command line, ksim will print detailed information about all processes
   that are currently being hosted. if the name specified by <process-name>
   is not associated with a current process, ksim will emit a message
   indicating such. the kernel consumes no ticks when servicing this command
 */
int Query_Funct( char **argList )
{
  //if no arg or 'all':
  if( argList[1] == NULL || strcmp( argList[1], "all" ) == 0)
    {
      QueryAll();
    }
  //a proc name hopefully:
  else
    {
      //search thru all proc's and compare names

      char *procNamePntr = argList[1];

      //convert char * to string: (to compare later on)
      string procName( procNamePntr ); 

      //loops thru array of queues:
      for( int i = 0; i < QUEUE_ARRAY_LEN; i++ )
	{

	  queue<struct PCB> currQueue = *queueArray[i]; //copy over queue

	  //while queue not empty:
	  while( !( currQueue.empty() ) )
	    {
	      
	      string currID = currQueue.front().id; //id
	      struct PCB currPCB = currQueue.front(); //curr pcb

	      //testing: cout << "looking at: " << currID << endl;

	      //w/ find proc:
	      if( currID == procName )
		{
		  cout << "***\n"; 
		  cout << "id: \"" << currID  << "\"\n";
		  cout << "state: \"" << currPCB.state
		       << "\"\n";
		 
		  //show proc device specifics if blocked:
		  DeviceSpecifics( currPCB );
		  
		  cout << "***\n";

		  //found proc so stop looking:
		  return -2; //non-termination lvl error
		}

	      currQueue.pop(); //remove head entry

	    }

	}

      //looking for proc not found:
      cout << "Process \"" << procName << "\" not found.\n"; 
      return -16; //error

    }



  return 0; //success
}

//shows device specifics if proc blocked:
void DeviceSpecifics( struct PCB currPCB )
{
  //calc shown blocked tick:
  unsigned long long shownBlockedTick;

  //if waiting proc in blocked queue:
  if( currPCB.state == "Blocked" )
    {
      //if haven't rolled over yet w/ blocked:
      if( MAX_TICKS > currPCB.blockedTick )
        {
          //shown tick = blocked tick:
          shownBlockedTick = currPCB.blockedTick;
        }
      //rollover happened before block:
      else
        {
          //calc blocked tick to show:
                      shownBlockedTick = currPCB.blockedTick -
                        rollOvers * MAX_TICKS;
        }

      //print out what waiting on:
      cout << "\twaiting on device "
           << currPCB.deviceWaitingOn << " since tick ";
      printf("%09d\n", shownBlockedTick );
    }
}

//query all proc's within the kernel:
void QueryAll()
{
  for( int i = 0; i < QUEUE_ARRAY_LEN; i++ )
    {

      queue<struct PCB> currQueue = *queueArray[i]; //copy over queue

      //while queue not empty:
      while( !( currQueue.empty() ) )
	{
	  //display device specifics:
          DisplayDevice( currQueue );

	  //pop outside so no pass by ref:
	  currQueue.pop(); //remove head entry
	}
      
    }
  
}

//if blocked, displays proc's blocked tick and device:
void DisplayDevice( queue<struct PCB> currQueue )
{
  string currID = currQueue.front().id; //ID
  struct PCB currPCB = currQueue.front(); //curr PCB

  //testing: cout << "looking at: " << currID << endl;

  cout << "***\n";
  cout << "id: \"" << currID  << "\"\n";
  cout << "state: \"" << currPCB.state
       << "\"\n";

  //display device specifics if blocked:
  DeviceSpecifics( currPCB );

  cout << "***\n";

}



