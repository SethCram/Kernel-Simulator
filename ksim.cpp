/*
  Seth Cram
  11/12/2021
  hw3: Kernel Simulator
  File: ksim.cpp

  File's Description:
    -Global queues, arrays, and timer variables defined
    -main file
    -Reads and parses command line
    -Begins execution of entered command

  Testing Notes:
    -The highest value that can be represented in the tick counter is 
     999,999,999. This also means that the tick counter will “roll over”
     when 1,000,000,000 ticks have passed.
    -Rollover testing passed
    -Queues unneccessary for exit and running state,
     but simplifies manipulation

  Possible Expansion: 
    -Could change all global queues, arrays, and timer variables into local
     and pass in and out of various function. This would make the executable
     safe for multi-threading.
    -Could change runnning and exit queue into a single process pointer,
     one for each. Would decrease the overhead provided by two extra queues.
*/

#include "ksim.h" //header file w/ prototypes, macros, globals

using namespace std; //for queues and strings

//global queues for each state:
queue<struct PCB> NewProc_State;
queue<struct PCB> Blocked0_State;
queue<struct PCB> Blocked1_State;
queue<struct PCB> Blocked2_State;
queue<struct PCB> Blocked3_State;
queue<struct PCB> Ready_State;
queue<struct PCB> Exit_State; //only 1 proc here at a time
queue<struct PCB> Running_State; //only 1 proc here at a time

//global array of all queues:
queue<struct PCB> *queueArray[] = { &NewProc_State, &Blocked0_State, 
				     &Blocked1_State, &Blocked2_State, 
				     &Blocked3_State, &Ready_State, 
				    &Exit_State, &Running_State };

//global array of all blocked queues:
queue<struct PCB> *blockedArray[] = { &Blocked0_State,
				      &Blocked1_State, &Blocked2_State,
				      &Blocked3_State };
//need to be passed by ref to change curr contents of queues

//global timer variables:
unsigned long long int tickCount = 0; //max data type
unsigned long int rollOvers = 0; //keeps track of curr rollover cycle

int main( int argc, char **argv )
{
  char *line;
  char **argList;
  int runStatus;
  
  //var to display:
  unsigned int displayTickCount = 0; //never negative

  printf("********************************************************\n");
  printf("Welcome to ksim!\n");
  printf("Current limitations include, but are not limited to: \n");
  printf("1. Unsafe for multi-threaded environments. \n");
  //testing: 
  //printf("2. The simulator's time to rollover is set to %d.\n",MAX_TICKS);
  //Not realistic:
  //printf("3. Undefined behavior will occur if the simulator rolls over an incredibly large number of times.");
  printf("********************************************************\n");

  do 
    {
      //set display tick count from global tick: (after each cmd)
      displayTickCount = tickCount  - rollOvers * MAX_TICKS;

      //if tick count too high:
      if( displayTickCount >= MAX_TICKS )
	{
	  //add to roll over cntr
          rollOvers++;

	  //set display tick count from global tick: 
	  displayTickCount = tickCount  - rollOvers * MAX_TICKS; 

	}

      //check: printf("Actual tick count: %09d\n", tickCount);

      //The current tick must be displayed in 9 decimal digits:
      printf("ksim-%09d>", displayTickCount); 

      //READ cmd line into 'line':      
      line = ReadCmdLine();

      //check: printf("Line: %s\n", line);

      //PARSE line into args:
      argList = ParseLine( line );

      //EXECUTE cmd:
      runStatus = ExecuteCmd( argList );

      //check: printf("Current run status: %d\n", runStatus);

    }while( runStatus != 1 ); //run while not a termination lvl error

  return runStatus; //return termination lvl error
}

//find number of build-in cmds:
int NumBuiltinCmds( const char **builtins )
{
  int i; //for-loop cnting var
  int numOfEntries = 0; //cnts builtins

  for( i = 0; builtins[i] != NULL ; i++)
    {
      numOfEntries++;
    }

  //check:  printf("Number of Builtin cmds: %d\n", numOfEntries );

  return numOfEntries;
}

//determines whether passed in cmd is built-in, present, or neither:
int ExecuteCmd( char **argList )
{
  //hardcoded, built-in ksim cmds:
  static const char *builtins[] = { "add", "exit", "io-event", "query",
				     "release", "step", "wait", NULL }; 
  //NULL to find num of entries

  //bool builtinCmd = false; //to signal if curr run cmd is a builtin one
  int retState, i;
  int totalBuiltins; //holds number of builtin cmds 

  totalBuiltins = NumBuiltinCmds( builtins ); //calc numer of builtins

  if( argList[0] == NULL ) //if no cmd entered
    {
      return -1;//error
    } 

  for( i = 0; i < totalBuiltins; i++ ) //loop thru builtin cmds
    {
      if( strcmp( builtins[i], argList[0] ) == 0 )
	//if found builtin cmd
	{
	  retState = KsimBuiltins( i, argList );

	  //builtinCmd = true;
	}
    }

  return retState;
}

//store cmd line in ret'd char *:
char *ReadCmdLine()
{
  //int numOfChars = 100; //assumes cmd lines is less than 100 chars
  int getCharInt;
  char *line = (char *)malloc( CMD_LINE_MAX_CHARS * sizeof(char) ); //dont free bc ret pntr
  int i; //for-loop cnting var

  while( 1 ) //loop forever unless ret'd from
    {
      getCharInt = getchar(); //get nxt char

      for( i = 0; getCharInt != EOF && getCharInt != '\n'; i++ )
	{
	  //check: putchar( getCharInt ); //put read in char out to std out
	  
	  line[i] = getCharInt; //put read in char into line
	                        // implicit typecast done
	  
	  //check: printf( "%c", line[i] ); //check copy in
	  
	  getCharInt = getchar(); //get nxt char
	}
      
      line[i] = '\0'; //add null term char to end of line
      
      return line;
    }
}

//parse line into args and ret pntr to array of pntr to chars w/ args:
char **ParseLine( char *line)
{
  char **argList = (char **)malloc( CMD_LINE_MAX_CHARS * sizeof(char*) );
  int i; //for-loop cnting var

  argList[0] = strtok( line, " " ); //read 1st arg, for for-loop conditon

  for( i = 0; argList[i] != NULL; i++ )
    {
      //check:  printf("ArgList[%d]: %s\n", i, argList[i]); //print curr arg

      argList[i+1] = strtok(NULL, " " ); //i+1 so this is checked in
                                         // for-loop condition
    }

  return argList;
}
