// Simon,Schliesky
// ---INCLUDES---
#include "stdio.h"
#include "stdlib.h"

// ---DATA STRUCTURE---
typedef struct packet Packet;
struct packet {
  int size;
  int index;
  Packet *nextPacket;
};

typedef struct container Container;
struct container {
  int size;
  Packet *firstPacket;
  Container *nextContainer;
};


typedef int bool; // Taken from StackOverflow
#define true 1
#define false 0

// Declare global variables
Container *container_list;

// ---INLINE ASSEMBLER PART---
// (Part of the task!!!)
// inlineAddition returns the sum of two integers a and b
// TODO: Error handling depending on flags
int inlineAddition(int a, int b)
{
  int result = a;
  asm(
    "add %1, %0" : "+r"(result) : "r"(b)
    );
  return result;
}
// inlineSubtraction returns the result of a - b
int inlineSubtraction(int a, int b)
{
int result = a;
  asm(
    "sub %1, %0" : "+r"(result) : "r"(b)
    );
  return result;
}

// ---FITTING ALGORITHMS---
// First-Fit
bool firstFit(int currentPacketSize)
{
  Container *curContainer;
  curContainer = container_list;
  return false;
}
// Best-Fit
bool bestFit()
{
  
  return false;
}
// Next-Fit
bool nextFit()
{
  
  return false;
}
// Almost-Worst-Fit
bool almostWorstFit()
{
  
  return false;
}

// ---IO FUNCTIONS---
// writeOutput prints a correctly formatted output line
// per container to stdout
void writeOutput()
{

}

// readInput reads two lines
// will call createContainer n times during the first line
// will call fillContainer m times during second line
// TODO: Handle input format errors
void readInput(char* filename)
{
  FILE *file;
  size_t length;
  char line[1024], *pLine;
  bool (*curFunc)(int);

  curFunc = firstFit;
  file = fopen(filename, "r");
  if( file != NULL )
  {
    fgets(line, sizeof(line), file);
    /* while input data (space-separated) Process Input
    if(!curFunc())
    {
      printf("validation Failed\n");
      exit(1);
    }
    */
    printf("#1 %d\n", inlineAddition(26,72));
    printf("#2 %d\n", inlineSubtraction(26,72));

  }
  else {
    printf("Error: File Not Found or Permission Denied");
  }
  if(!feof(file))
  {
    //Error: Too many lines in input file
  }
  fclose(file);
}

void init()
{

}

int close()
{
  return 0;
}

// Program entry
int main(int argc, char *argv[])
{
  init();
  readInput(argv[1]);
  return close();
}