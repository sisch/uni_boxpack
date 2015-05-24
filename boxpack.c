// Simon,Schliesky
// ---INCLUDES---
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

// ---DATA STRUCTURE---
typedef struct packet Packet;
typedef struct container Container;

struct packet {
  int size;
  int index;
  Packet *nextPacket;
};

struct container {
  int size;
  Packet *firstPacket;
  Container *nextContainer;
};


typedef int bool; // Taken from StackOverflow
#define true 1
#define false 0

#define len(x)  (sizeof(x)/sizeof((x)[0]))
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

// ---STRUCTURE IMPLEMENTATION---
void createContainer(int conSize)
{
  Container *curContainer;
  if (container_list == NULL)
  {
    container_list = (Container*) malloc(sizeof(Container));
    container_list->size = conSize;
    container_list->nextContainer = NULL;
  }
  else
  {
    curContainer = container_list;
    while(curContainer->nextContainer != NULL)
      curContainer = curContainer->nextContainer;
    Container *newContainer = (Container*) malloc(sizeof(Container));
    newContainer->size = conSize;
    newContainer->nextContainer = NULL;
    curContainer->nextContainer = newContainer;
  }
}

void destroyContainer(Container* this)
{
  if(this->nextContainer != NULL){
    printf("Next Exists: %p\n", this);
    destroyContainer(this->nextContainer);
  }
  printf("No Next Exists: %p\n", this);
  free(this);
}

// ---FITTING ALGORITHMS---
// First-Fit
bool firstFit(int currentPacketSize)
{
  printf("First-Fit %d\n", currentPacketSize);
  //Container *curContainer;
  //curContainer = container_list;
  return false;
}
// Best-Fit
bool bestFit(int currentPacketSize)
{
  printf("Best-Fit %d\n", currentPacketSize);
  return false;
}
// Next-Fit
bool nextFit(int currentPacketSize)
{
  printf("Next-Fit %d\n", currentPacketSize);
  return false;
}
// Almost-Worst-Fit
bool almostWorstFit(int currentPacketSize)
{
  printf("Almost-Worst-Fit %d\n", currentPacketSize);
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
    // -- Read first line
    fgets(line, sizeof(line), file);
    char *val = strtok(line, " "); // Load first value
    if(val == NULL)
    {
      // Error: Wrong input format (line does not start with integer)
    }
    while (val != NULL)
    {
      int containerSize = atoi(val);
      createContainer(containerSize);
      val = strtok(NULL, " "); // Load next value
    }
    
    // -- Read second line
    fgets(line, sizeof(line), file);
    val = strtok(line, " ");
    while (val != NULL)
    {
      if(isalpha(val[0]))
      {
        if(val[0] == 'f')
        {
          curFunc = &firstFit;
        }
        else if(val[0] == 'b')
        {
          curFunc = &bestFit;
        }
        else if(val[0] == 'n')
        {
          curFunc = &nextFit;
        }
        else if(val[0] == 'a')
        {
          curFunc = &almostWorstFit;
        }
      }
      else
      {
        int packetSize = atoi(val);
        curFunc(packetSize);
      }
      printf("\n");
      val = strtok(NULL, " "); // Load next value
    }
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
  container_list = NULL;
}

int close()
{
  if(container_list != NULL)
    destroyContainer(container_list);
  return 0;
}

// Program entry
int main(int argc, char *argv[])
{
  init();
  readInput(argv[1]);
  return close();
}