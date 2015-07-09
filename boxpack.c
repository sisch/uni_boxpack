//Simon,Schliesky,Vx2xQu15AGe4
// ---INCLUDES---
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
#include "stddef.h"

// ---FORWARD DECLARATIONS
typedef int bool; // Taken from StackOverflow
int close(void);
bool printContainers(void);
typedef struct packet Packet;
typedef struct container Container;
#define MAXINT 2147483647
//#define DEBUG



// ---DATA STRUCTURE---
struct packet {
  int size;
  int index;
  Packet *nextPacket;
};

struct container {
  int size;
  int index;
  int remainingSize;
  Packet *firstPacket;
  Container *nextContainer;
};


#define true 1
#define false 0
#define len(x)  (sizeof(x)/sizeof((x)[0]))
// Declare global variables
Container *container_list;
Container *currentContainerForNextFit;

// ---INLINE ASSEMBLER PART---
// (Part of the task!!!)
// inlineAddition returns the sum of two integers a and b
// TODO: Error handling depending on flags
int inlineAddition(int a, int b)
{
  int result = a;
  __asm__(
    "add %1, %0" : "+r"(result) : "r"(b)
    );
  return result;
}
// inlineSubtraction returns the result of a - b
int inlineSubtraction(int a, int b)
{
int result = a;
  __asm__(
    "sub %1, %0" : "+r"(result) : "r"(b)
    );
  return result;
}

// ---STRUCTURE IMPLEMENTATION---
void createContainer(int conSize)
{
//  printf("cC: %d\n", conSize);
  Container *curContainer;
  if (container_list == NULL)
  {
    container_list = (Container*) malloc(sizeof(Container));
    container_list->index = 0;
    container_list->size = conSize;
    container_list->remainingSize = conSize;
    container_list->nextContainer = container_list;
    container_list->firstPacket = NULL;
  }
  else
  {
    curContainer = container_list;
    while(curContainer->nextContainer != container_list)
    {
      curContainer = curContainer->nextContainer;
    }
    Container *newContainer = (Container*) malloc(sizeof(Container));
    newContainer->index = inlineAddition(curContainer->index, 1);
    newContainer->size = conSize;
    newContainer->remainingSize = conSize;
    newContainer->nextContainer = container_list;
    newContainer->firstPacket = NULL;
    curContainer->nextContainer = newContainer;
  }
}

void createPacket(int currentPacketSize, Container *curContainer)
{
  int newIndex = 0;
  Packet *newPacket = (Packet*) malloc(sizeof(Packet));
  newPacket->size = currentPacketSize;
  newPacket->nextPacket = NULL;
  if(curContainer->firstPacket != NULL)
  {
    Packet *lastPacket = curContainer->firstPacket;
    while (lastPacket->nextPacket != NULL)
      {
        newIndex = inlineAddition(newIndex, lastPacket->size);
        lastPacket = lastPacket->nextPacket;
      }
      newPacket->index =  newIndex;
      lastPacket->nextPacket = newPacket;
  }
  else
  {
    newPacket->index = newIndex;
    curContainer->firstPacket = newPacket;
  }
  curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
}
void destroyPacket(Packet* this)
{
  if(this != NULL)
  {
    destroyPacket(this->nextPacket);
  }
  free(this);
}

void destroyContainer(Container* this)
{
  destroyPacket(this->firstPacket);
  if(this->nextContainer != container_list){
    //printf("Next Exists: %p\n", this);
    destroyContainer(this->nextContainer);
  }
  //printf("No Next Exists: %p\n", this);
  free(this);
}

// ---FITTING ALGORITHMS---
// First-Fit
bool firstFit(int currentPacketSize)
{
  //printf("First-Fit %d\n", currentPacketSize);
  Container *curContainer = container_list;
  while(curContainer->remainingSize < currentPacketSize)
  {
    if (curContainer->nextContainer == container_list)
    {
      return false;
    }
    curContainer = curContainer->nextContainer;
  }
  createPacket(currentPacketSize, curContainer);
  return true;
}

// Best-Fit
bool bestFit(int currentPacketSize)
{
  // TODO: FIX EVERYTHING
  Container *bestfitContainer;
  Container *curContainer;
  bestfitContainer = container_list;
  curContainer = container_list;
  int curRemainingSize = curContainer->remainingSize;
  int curBestsize = MAXINT;
  do
  {
    if( curRemainingSize >= currentPacketSize
      && curBestsize > inlineSubtraction(curRemainingSize, currentPacketSize)
      && inlineSubtraction(curRemainingSize, currentPacketSize) >= 0)
    {
      bestfitContainer = curContainer;
      curBestsize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
    }
    curContainer = curContainer->nextContainer;
    curRemainingSize = curContainer->remainingSize;
  } while (curContainer != container_list);
  if(bestfitContainer->remainingSize < currentPacketSize)
  {
    return false; 
  }
  createPacket(currentPacketSize, bestfitContainer);
  return true;
}

// Next-Fit
bool nextFit(int currentPacketSize)
{
  Container *curContainer = currentContainerForNextFit;
  while(curContainer->remainingSize < currentPacketSize)
  {
    curContainer = curContainer->nextContainer;
    if(curContainer == currentContainerForNextFit)
    {
      return false;
    }
  }
  currentContainerForNextFit = curContainer;
  createPacket(currentPacketSize, curContainer);
  return true;
}

// Almost-Worst-Fit
bool almostWorstFit(int currentPacketSize)
{
  // Find Container with second most remainingSize
  Container *maxSizeContainer;
  Container *max2SizeContainer;
  Container *curContainer;
  Container *awfContainer;
  curContainer = container_list;
  maxSizeContainer = NULL;
  max2SizeContainer = NULL;
  // maxSizeContainer > max2SizeContainer >= containerSize
  int maxSize = -1;
  int max2Size = -1;
  do
  {
    #ifdef DEBUG
    printf("Current Container Size: %d, Packet %d\n", curContainer->remainingSize, currentPacketSize);
    #endif
    if(curContainer->remainingSize >= currentPacketSize)
    {
      if( curContainer->remainingSize > maxSize )
      {
        if(maxSizeContainer != NULL)
        {
          max2Size = maxSize;
          max2SizeContainer = maxSizeContainer;
        }
        maxSize = curContainer->remainingSize;
        maxSizeContainer = curContainer;
      }
      else if(curContainer->remainingSize > max2Size)
      {
        max2Size = curContainer->remainingSize;
        max2SizeContainer = curContainer;
      }
    }
    curContainer = curContainer->nextContainer;  
  } while(curContainer != container_list);
  if(max2SizeContainer != NULL)
  {
    awfContainer = max2SizeContainer;
  }
  else if(maxSizeContainer != NULL)
  {
    awfContainer = maxSizeContainer;
  }
  else
  {
    return false;
  }
  // tie-breaker when max and max2 are equal
  if(maxSize == max2Size)
  {
    if(awfContainer->index > maxSizeContainer->index)
    {
      awfContainer = maxSizeContainer;
    }
  }
  if(currentPacketSize > awfContainer->remainingSize)
  {
    return false;
  }
  #ifdef DEBUG
  printf("cur %d: conSize %d, packSize %d\n", curContainer->index, curContainer->remainingSize,currentPacketSize);
  printf("maxS %d: conSize %d, packSize %d\n", maxSizeContainer->index, maxSizeContainer->remainingSize,currentPacketSize);
  printf("maxS2 %d: conSize %d, packSize %d\n", max2SizeContainer->index, max2SizeContainer->remainingSize,currentPacketSize);
  printf("awf %d: conSize %d, packSize %d\n", awfContainer->index, awfContainer->remainingSize,currentPacketSize);
  printf("%d: %d\n",currentPacketSize, awfContainer->remainingSize);
  #endif
  createPacket(currentPacketSize, awfContainer);
  return true;
}

// ---IO FUNCTIONS---
// writeOutput prints a correctly formatted output line
// per container to output file
bool writeOutput(char *filename)
{
  FILE *file;
  file = fopen(filename, "w");
  char line[4096];
   
  Container* c = container_list;
  do
  {
    snprintf(line, 1024, "%d:", c->index);
    fwrite(line, sizeof(char), strlen(line), file);
    Packet* p = c->firstPacket;
    while(p != NULL)
    {
      snprintf(line, 1024, " %d", p->size);
      fwrite(line, sizeof(char), strlen(line), file);
      p = p->nextPacket;
    }
    fwrite("\n", sizeof(char), 1, file);
    c = c->nextContainer;
  } while ( c != container_list ) ;
  fclose(file);
  return true;
  //printContainers(); // TODO: replace by writeToFile
}

// readInput reads two lines
// will call createContainer n times during the first line
// will call fitFunction m times during second line
bool readInput(char* filename)
{
  FILE *file;
  char line[1048576];
  bool (*curFunc)(int);

  curFunc = firstFit;
  file = fopen(filename, "r");
  if( file != NULL )
  {
    // -- Read first line
    fgets(line, sizeof(line), file); // TODO: Replace with getline
    char *val = strtok(line, " "); // Load first value
    if(val == NULL)
    {
      printf("Error: Wrong input format on line 1 (line did not contain spaces)\n");
    }
    while (val != NULL)
    {
      int containerSize = atoi(val);
      if(containerSize <= 0)
      {
        printf("Error: Wrong input format on line 1 (value was not positive integer)\n");
      }
      createContainer(containerSize);
      val = strtok(NULL, " "); // Load next value
    }
    
    // -- Read second line
    fgets(line, sizeof(line), file); // TODO: Replace with getline
    val = strtok(line, " ");
    while (val != NULL)
    {
      /*if(strcmp(val, " "))
      {
        printf("Missing value / possible double space");
        fclose(file);
        return false;
      }*/
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
        //printContainers();
        if (!curFunc(packetSize))
        {
          #ifdef DEBUG
          writeOutput("error.log");
          #endif
          fprintf(stdout, "validation failed\n");
          fclose(file);
          return false;
        }
      }
      val = strtok(NULL, " "); // Load next value
    }
    //get trailing newline
    fgets(line, sizeof(line), file);
    if(!feof(file))
    {
      fprintf(stderr, "Error: Too many lines in input file.\n");
    }
    fclose(file);
    return false;
  }
  else {
    fprintf(stderr, "Error: File Not Found or Permission Denied.\n");
  }
  return true;
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

bool printContainers()
{
  Container* c = container_list;
  do
  {
    printf("%d:", c->index);
    Packet* p = c->firstPacket;
    while(p != NULL)
    {
      printf(" %d", p->size);
      p = p->nextPacket;
    }
    printf("\n");
    c = c->nextContainer;
  } while ( c != container_list ) ;
  return true;
}
// Program entry
int main(int argc, char *argv[])
{
  init();
  if (!readInput(argv[1]))
  {
    close();
    return 2;
  }
  if(!writeOutput(argv[2]))
  {
    return 4;
  }
  return close();
}
