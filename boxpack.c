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
  Packet *lastPacket;
  Container *nextContainer;
};


#define true 1
#define false 0
#define len(x)  (sizeof(x)/sizeof((x)[0]))
// Declare global variables
Container *containerList;
Container *currentContainerForNextFit;
Container *mostRecentlyAddedContainer;

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
  curContainer = malloc(sizeof(Container));
  curContainer->size = conSize;
  curContainer->remainingSize = conSize;
  curContainer->firstPacket = NULL;
  if (containerList == NULL)
  {
    containerList = curContainer;
    containerList->index = 0;
  }
  else
  {
    mostRecentlyAddedContainer->nextContainer = curContainer;
    curContainer->index = inlineAddition(mostRecentlyAddedContainer->index, 1);
  }
  mostRecentlyAddedContainer = curContainer;
  mostRecentlyAddedContainer->nextContainer = containerList;
}

void createPacket(int currentPacketSize, Container *curContainer)
{
  int newIndex = 0;
  Packet *newPacket = malloc(sizeof(Packet));
  newPacket->size = currentPacketSize;
  newPacket->nextPacket = NULL;
  if(curContainer->firstPacket != NULL)
  {
    if(curContainer->lastPacket == NULL)
    {
      curContainer->lastPacket = curContainer->firstPacket;
    }
    newIndex = inlineAddition(curContainer->lastPacket->index, 1);
    newPacket->index =  newIndex;
  }
  else
  {
    newPacket->index = 0;
    curContainer->firstPacket = newPacket;
    curContainer->lastPacket = curContainer->firstPacket;
    curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
    return;
  }
  curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
  curContainer->lastPacket->nextPacket = newPacket;
  curContainer->lastPacket = newPacket;
}
void destroyPacket(Packet* this)
{
  if(this == NULL)
  {
    return;
  }
  Packet* toDestroyNext = this->nextPacket;
  free(this);
  while(toDestroyNext != NULL)
  {
    this = toDestroyNext->nextPacket;
    free(toDestroyNext);
    toDestroyNext = this;
  }
}

void destroyContainer(Container* this)
{
  if(this == NULL)
  {
    return;
  }
  destroyPacket(this->firstPacket);
  Container* toDestroyNext = this->nextContainer;
  free(this);
  while(toDestroyNext != NULL && toDestroyNext != containerList)
  {
    this = toDestroyNext->nextContainer;
    free(toDestroyNext);
    toDestroyNext = this;
  }
}

// ---FITTING ALGORITHMS---
// First-Fit
bool firstFit(int currentPacketSize)
{
  //printf("First-Fit %d\n", currentPacketSize);
  Container *curContainer = containerList;
  while(curContainer->remainingSize < currentPacketSize)
  {
    if (curContainer->nextContainer == containerList)
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
  bestfitContainer = containerList;
  curContainer = containerList;
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
  } while (curContainer != containerList);
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
  curContainer = containerList;
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
  } while(curContainer != containerList);
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
   
  Container* c = containerList;
  do
  {
    snprintf(line, 1024, "%d:", c->index);
    fwrite(line, sizeof(char), strlen(line), file);
    Packet* p = c->firstPacket;
    if(c->firstPacket == NULL)
    {
      snprintf(line, 1024, " %d", 0); 
    }
    while(p != NULL)
    {
      snprintf(line, 1024, " %d", p->size);
      fwrite(line, sizeof(char), strlen(line), file);
      p = p->nextPacket;
    }
    fwrite("\n", sizeof(char), 1, file);
    c = c->nextContainer;
  } while ( c != containerList ) ;
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
      fprintf(stderr, "Error: Wrong input format on line 1 (line did not contain spaces)\n");
    }
    while (val != NULL)
    {
      int containerSize = atoi(val);
      if(containerSize <= 0)
      {
        fprintf(stderr, "Error: Wrong input format on line 1 (value was not positive integer)\n");
      }
      createContainer(containerSize);
      val = strtok(NULL, " "); // Load next value
    }
    #ifdef DEBUG
      puts("Second line\n");
    #endif
    
    // -- Read second line
    fgets(line, sizeof(line), file); // TODO: Replace with getline
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
    #ifdef DEBUG
      puts("After Second line\n");
    #endif
    //get trailing newline
    fgets(line, sizeof(line), file);
    if(!feof(file))
    {
      fprintf(stderr, "Error: Too many lines in input file.\n");
      fclose(file);
      return false;
    }
    fclose(file);
  }
  else {
    fprintf(stderr, "Error: File Not Found or Permission Denied.\n");
    return false;
  }
  return true;
}

void init()
{
  containerList = NULL;
}

int close()
{
  if(containerList != NULL)
    destroyContainer(containerList);
  return 0;
}

bool printContainers()
{
  Container* c = containerList;
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
  } while ( c != containerList ) ;
  return true;
}
// Program entry
int main(int argc, char *argv[])
{
  free(NULL);
  init();
  if (!readInput(argv[1]))
  {
    #ifdef DEBUG
      puts("Read\n");
    #endif
    close();
    return 2;
  }
    #ifdef DEBUG
      puts("Before Write\n");
    #endif

  if(!writeOutput(argv[2]))
  {
    #ifdef DEBUG
      puts("Write\n");
    #endif
    return 4;
  }
  return close();
}
