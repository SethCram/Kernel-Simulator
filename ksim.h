/* Seth Cram
   11/12/2021
   hw3: Kernel Simulator
   File: ksim.h

    Description:
    -header file w/ prototypes, macros, and globals 
    -needed by ksim.cpp, ksim_cmds.cpp, ksim_step.cpp, and ksim_query.cpp
*/

//LIBS:

//converting C code to be C++ compatable:
#include <cstdio>
#include <cstdlib>
#include <cstring> 

#include <queue> //for STL queues
#include <string> //for strings
#include <iostream> //for cout

//struct for every proc:
struct PCB{ 
  std::string id;
  long long unsigned int blockedTick;
  std::string state;
  int deviceWaitingOn;
  long long unsigned int lastRunTick; //equiv to last run or creation tick
};

//PROTOTYPES:

//command line parsing:
char *ReadCmdLine();
char **ParseLine( char *line );

//choosing which function to run:
int RunProc( char **argList );
int KsimBuiltins( int i, char **argList );
int ExecuteCmd( char **argList );
int NumBuiltinCmds( const char **builtins );

//essential internal functions:
int Add_Funct( char **argList );
int Exit_Funct();
int IOevent_Funct( char **argList );
int Release_Funct( );
int Wait_Funct( char **argList );
bool DeviceValid( char **argList ); //ioevent and wait helper funct
bool ArgumentPassed( char *arg, std::string Op ); //helper funct for add
bool SearchQueues( std::string procName ); //helper funct for add
void AllBlockedtoReady( int devNum ); //helper funct for io-event

//query function and its helper functions:
int Query_Funct( char **argList );
void QueryAll();
void DisplayDevice( std::queue<struct PCB> currQueue );
void DeviceSpecifics( struct PCB currPCB );

//step function and its related helper functions:
int Step_Funct( );
void NewtoReady();
void DeleteExitProcs();
void TimeoutCheck( );
void RunningtoReady( );
int ReadytoRunning();
void MoveOldesttoRun();

//MACROS: (for readability)
#define CMD_LINE_MAX_CHARS 2093 //found w/ 'env | wc -c' on cs server

#define QUEUE_ARRAY_LEN 8 //hard coded for easy accessibility
#define BLOCKED_QUEUES 4 

#define BLOCKED_TICK_LIMIT 1024
#define MAX_TICKS 1000000000
//testing: #define MAX_TICKS 2000


//GLOBALS: (defined in ksim.cpp)
 
//reference to global queues:
extern std::queue<struct PCB> NewProc_State;
extern std::queue<struct PCB> Blocked0_State;
extern std::queue<struct PCB> Blocked1_State;
extern std::queue<struct PCB> Blocked2_State;
extern std::queue<struct PCB> Blocked3_State;
extern std::queue<struct PCB> Ready_State;
extern std::queue<struct PCB> Exit_State;
extern std::queue<struct PCB> Running_State;

//reference to global queue arrays:
extern std::queue<struct PCB> *queueArray[];
extern std::queue<struct PCB> *blockedArray[];

//reference to global timer vars:
extern unsigned long long int tickCount;
extern unsigned long int rollOvers;




