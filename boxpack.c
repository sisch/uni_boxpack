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
typedef struct packet Packet;
typedef struct container Container;
#define MAXINT 2147483647

// ---DATA STRUCTURE---
/** packet:
  * Each packet is assigned to a container upon construction.
  * Size is the number of slots used in the container.
  * Index is the sequential number of the packet, starting at 0
  * Packets are created as a linked list and therefore have a pointer to the nextPacket
 **/
struct packet {
  int size;
  int index;
  Packet *nextPacket;
};

/** container:
  * Size is the number of slots available in the container.
  * Index is the number of the container (i.e. in order of creation), starting at 0.
  * firstPacket is a pointer to the first packet inside the container.
  * lastPacket is a pointer to the most recently added packet to simplify adding new packets.
  * Containers are created as a linked list and therefore have a pointer to the nextContainer.
 **/
struct container {
  int size;
  int index;
  int remainingSize;
  Packet *firstPacket;
  Packet *lastPacket;
  Container *nextContainer;
};

// definitions enabling use of booleans and string length
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
// ~TODO:~ Error handling depending on flags
// Apparently not necessary to solve the task
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
// Creating a container of size conSize allocates memory for a container struct
// and appends it to the existing list or creates one unless it exists.
void createContainer(int conSize)
{
  Container *curContainer;
  curContainer = malloc(sizeof(Container));
  curContainer->size = conSize;
  curContainer->remainingSize = conSize;
  // Upon creation a container is empty, so firstPacket is initialized with NULL
  curContainer->firstPacket = NULL;
  // First container created
  if (containerList == NULL)
  {
    containerList = curContainer;
    containerList->index = 0;
  }
  // Other containers append
  else
  {
    mostRecentlyAddedContainer->nextContainer = curContainer;
    curContainer->index = inlineAddition(mostRecentlyAddedContainer->index, 1);
  }
  // mostRecentlyAddedContainer is tracked for performance reasons.
  mostRecentlyAddedContainer = curContainer;
  // The linked list of containers is cyclic, because in nextFit the loop does not
  // necessary start from the first container.
  mostRecentlyAddedContainer->nextContainer = containerList;
}

// createPacket allocates memory for a packet struct and adds a packet
// of size currentPacketSize to the curContainer.
// Check for remainingSize happens before the call to this function.
void createPacket(int currentPacketSize, Container *curContainer)
{
  int newIndex = 0;
  Packet *newPacket = malloc(sizeof(Packet));
  newPacket->size = currentPacketSize;
  newPacket->nextPacket = NULL;
  // If there already is a firstPacket newPacket will receive index != 0
  if(curContainer->firstPacket != NULL)
  {
    // If there is only one Packet, lastPacket is still uninitialized, so it is set
    // to firstPacket.
    if(curContainer->lastPacket == NULL)
    {
      curContainer->lastPacket = curContainer->firstPacket;
    }
    newIndex = inlineAddition(curContainer->lastPacket->index, 1);
    newPacket->index = newIndex;
  }
  // If this is the firstPacket set index to 0 and add the packet to the curContainer
  else
  {
    newPacket->index = 0;
    curContainer->firstPacket = newPacket;
    curContainer->lastPacket = newPacket;
    curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
    return;
  }
  curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
  curContainer->lastPacket->nextPacket = newPacket;
  curContainer->lastPacket = newPacket;
}

// To free memory loop over all packets of a container.
void destroyPackets(Container* this)
{
  Packet* curPacket = this->firstPacket;
  if(curPacket != NULL)
  {
    Packet* toDestroyNext = curPacket->nextPacket;
    free(curPacket);
    while(toDestroyNext != NULL)
    {
      curPacket = toDestroyNext->nextPacket;
      free(toDestroyNext);
      toDestroyNext = curPacket;
    }  
  }
}

// To free memory loop over all containers, first destroying packets then freeing memory
void destroyContainer(Container* this)
{
  if(this == NULL)
  {
    return;
  }
  destroyPackets(this);
  Container* toDestroyNext = this->nextContainer;
  free(this);
  while(toDestroyNext != NULL && toDestroyNext != containerList)
  {
    this = toDestroyNext->nextContainer;
    destroyPackets(toDestroyNext);
    free(toDestroyNext);
    toDestroyNext = this;
  }
}

// ---FITTING ALGORITHMS---
// First-Fit
bool firstFit(int currentPacketSize)
{
  // Start from the first container and find one, that fits the currentPacketSize
  Container *curContainer = containerList;
  while(curContainer->remainingSize < currentPacketSize)
  {
    // If loop returns to first container, none have enough space so return false
    // which will result in "validation failed"
    if (curContainer->nextContainer == containerList)
    {
      return false;
    }
    curContainer = curContainer->nextContainer;
  }
  // If container has enough remainingSize, createPacket and mark container for next fit.
  createPacket(currentPacketSize, curContainer);
  currentContainerForNextFit = curContainer;
  return true;
}

// Best-Fit
bool bestFit(int currentPacketSize)
{
  // loop over all containers starting from the first.
  // remember the container with the least remainingSize as bestFitContainer
  Container *bestfitContainer;
  Container *curContainer;
  bestfitContainer = containerList;
  curContainer = containerList;
  int curRemainingSize = curContainer->remainingSize;
  int curBestsize = MAXINT;
  do
  {
    // condition 1 and 3 seem redundant, but had a reason at time of writing.
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
  // Since the first container is set to bestFit initially better re-check, whether packet fits in.
  if(bestfitContainer->remainingSize < currentPacketSize)
  {
    return false; 
  }
  // add packet and mark container for nextFit
  createPacket(currentPacketSize, bestfitContainer);
  currentContainerForNextFit = bestfitContainer;
  return true;
}

// Next-Fit
bool nextFit(int currentPacketSize)
{
  // loop over all containers starting from the one marked as currentContainerForNextFit
  Container *curContainer = currentContainerForNextFit;
  // if none have been marked yet, start from the beginning
  if(curContainer == NULL)
  {
    curContainer = containerList;
  }
  while(curContainer->remainingSize < currentPacketSize)
  {
    curContainer = curContainer->nextContainer;
    // if loop reaches currentContainerForNextFit again, return false -> "validation failed"
    if(curContainer == currentContainerForNextFit)
    {
      return false;
    }
  }
  // add packet and mark curContainer for nextFit
  createPacket(currentPacketSize, curContainer);
  currentContainerForNextFit = curContainer;
  return true;
}

// Almost-Worst-Fit
bool almostWorstFit(int currentPacketSize)
{
  // Loop over all containers starting from the first.
  // Find Container with second most remainingSize
  Container *maxSizeContainer;
  Container *max2SizeContainer;
  Container *curContainer;
  Container *awfContainer;
  curContainer = containerList;
  maxSizeContainer = NULL;
  max2SizeContainer = NULL;
  int maxSize = -1;
  int max2Size = -1;
  do
  {
    #ifdef DEBUG
    printf("Current Container Size: %d, Packet %d\n", curContainer->remainingSize, currentPacketSize);
    #endif
    // if container can hold packet of size currentPacketSize
    // Check also if remainingSize is larger than one of maxSize, max2Size.
    // Replace if necessary and adjust pointers to the containers accordingly.
    if(curContainer->remainingSize >= currentPacketSize)
    {
      if(curContainer->remainingSize > maxSize)
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

  // decide which container is the awfContainer, if none return false
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
  // Better re-check if packet will REALLY fit
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
  // add packet and mark awfContainer for next Fit
  createPacket(currentPacketSize, awfContainer);
  currentContainerForNextFit = awfContainer;
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
  //loop over all containers and output container index and all packets
  Container* c = containerList;
  do
  {
    snprintf(line, 1024, "%d:", c->index);
    fwrite(line, sizeof(char), strlen(line), file);
    Packet* p = c->firstPacket;
    //loop over the packets
    while(p != NULL)
    {
      snprintf(line, 1024, " %d", p->size);
      fwrite(line, sizeof(char), strlen(line), file);
      p = p->nextPacket;
    }
    // if there are no packets output 0, even though the task did not specify this
    if(c->firstPacket == NULL)
    {
      snprintf(line, 1024, " 0"); 
      fwrite(line, sizeof(char), strlen(line), file);
    }
    fwrite("\n", sizeof(char), 1, file);
    c = c->nextContainer;
  } while ( c != containerList ) ;
  fclose(file);
  return true;
}

// readInput reads two lines
// will call createContainer n times during the first line
// will call fitFunction m times during second line
bool readInput(char* filename)
{
  FILE *file;
  // Create linebuffer sufficiently large (because of tests)
  // and initialize with w
  char line[1048576] = "\0";
  bool (*curFunc)(int);
  // start with firstFit if no algo specified
  curFunc = firstFit;
  file = fopen(filename, "r");
  if( file != NULL )
  {
    // -- Read first line
    fgets(line, sizeof(line)-1, file);
    char *val = strtok(line, " "); // Load first value
    if(val == NULL)
    {
      fprintf(stderr, "Error: Wrong input format on line 1 (line did not contain spaces)\n");
      fclose(file);
      return false;
    }
    while (val != NULL)
    {
      int i;
      for(i = 0; i < len(val);i++)
      {
        // Assuming only numbers on first line. Check whether there are other characters.
        if(isalpha(val[i]))
        {
          fprintf(stderr, "Error: numbers contain illegal characters\n");
          fclose(file);
          return false;
        }
      }
      int containerSize = atoi(val);
      if(containerSize <= 0)
      {
        fprintf(stderr, "Error: Wrong input format on line 1 (value was not a positive integer)\n");
        fclose(file);
        return false;
      }
      createContainer(containerSize);
      val = strtok(NULL, " "); // Load next value
    }
    #ifdef DEBUG
      puts("Second line\n");
    #endif
    
    // -- Read second line
    fgets(line, sizeof(line)-1, file);
    val = strtok(line, " ");
    while (val != NULL)
    {
      //if value begins with character (i.e. not number) assume algo token
      if(isalpha(val[0]))
      {
        if(val[0] == 'f')
        {
          if(val[1] != 'f')
          {
            fprintf(stderr, "Error: Not a valid algo token. Expected ff got %s.\n", val);
            fclose(file);
            return false;
          }
          curFunc = &firstFit;
        }
        else if(val[0] == 'b')
        {
          if(val[1] != 'f')
          {
            fprintf(stderr, "Error: Not a valid algo token. Expected bf got %s.\n", val);
            fclose(file);
            return false;
          }
          curFunc = &bestFit;
        }
        else if(val[0] == 'n')
        {
          if(val[1] != 'f')
          {
            fprintf(stderr, "Error: Not a valid algo token. Expected nf got %s.\n", val);
            fclose(file);
            return false;
          }
          curFunc = &nextFit;
        }
        else if(val[0] == 'a')
        {
          if(val[1] != 'w' || val[2] != 'f')
          {
            fprintf(stderr, "Error: Not a valid algo token. Expected awf got %s.\n", val);
            fclose(file);
            return false;
          }
          curFunc = &almostWorstFit;
        }
        else
        {
          fprintf(stderr, "Error: Not a valid algo token.\n");
          fclose(file);
          return false;
        }
      }
      // if value does not begin with character, assume integer (i.e. packet size)
      else
      {
        int packetSize = atoi(val);
        if(packetSize <= 0)
        {
          fprintf(stderr, "Error: packet size is not a positive integer\n");
          fclose(file);
          return false;
        }
        // Try to add packet with previously specified fitting function.
        // if false, packet does not fit -> "validation failed"
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
    // Next block checks for line-endings. Proper formatting requires exactly 1 \n
    int ns = 0;
    fclose(file);
    file = fopen(filename, "r");
    char c;
    while ((c = fgetc(file) )!= EOF)
    {
      if (c == '\n')
      {
        ns++;
      }
    }
    fclose(file);
    if(ns != 1)
    {
      fprintf(stderr, "Error: Too many lines in input file.\n");
      return false;  
    }
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

// clean up before exit.
int close()
{
  if(containerList != NULL)
    destroyContainer(containerList);
  return 0;
}

// Program entry
int main(int argc, char *argv[])
{
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
