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
  int numberOfPackets;
  Packet *firstPacket;
  Packet *lastPacket;
  Container *nextContainer;
};

// definitions enabling use of booleans
#define true 1
#define false 0

// Declare global variables
Container *containerList;
Container *currentContainerForNextFit;
Container *mostRecentlyAddedContainer;
char* inputStart;
char* firstLineStart;
char* secondLineStart;

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
  #ifdef DEBUG2
  printf("Container created, ");
  #endif
  Container *curContainer;
  curContainer = malloc(sizeof(Container));
  curContainer->size = conSize;
  curContainer->remainingSize = conSize;
  curContainer->numberOfPackets = 1; // initialized with one, because output requires 0 printed
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
    curContainer->numberOfPackets = 1;
    curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
    return;
  }
  curContainer->remainingSize = inlineSubtraction(curContainer->remainingSize, currentPacketSize);
  curContainer->lastPacket->nextPacket = newPacket;
  curContainer->lastPacket = newPacket;
  curContainer->numberOfPackets = inlineAddition(curContainer->numberOfPackets,1);
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
    //printf("Current Container Size: %d, Packet %d\n", curContainer->remainingSize, currentPacketSize);
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
  // How much memory will be needed for output?
  // 10 digits (MAXINT) for container index
  // 1 for :
  // 1 space per packet
  // x digits (max containersize digits) for packet sizes
  // 1 for '\n'
  // per container 12+ (1+x)*packetNumber
  
  // calculate memory needs
  Container* c = containerList;
  int charactersNeededInOutput = 0;
  do
  {
    charactersNeededInOutput = charactersNeededInOutput + 12 + (1+c->size)*c->numberOfPackets;
    c = c->nextContainer;
  }while(c != containerList);
  char *line = malloc((1+charactersNeededInOutput)*sizeof(char));
  char *currentChar = line;
  //loop over all containers and output container index and all packets
  c = containerList;
  int shift =0;
  do
  {
    shift = snprintf(currentChar, 12, "%d:", c->index);
    currentChar += shift;
    #ifdef DEBUG2
    printf("currentString: %s\n", line);
    #endif
    Packet* p = c->firstPacket;
    //loop over the packets
    while(p != NULL)
    {
      shift = snprintf(currentChar, 12, " %d", p->size);
      currentChar += shift;
      p = p->nextPacket;
    }
    // if there are no packets output 0, even though the task did not specify this
    if(c->firstPacket == NULL)
    {
      shift = snprintf(currentChar, 3, " 0");
      currentChar += shift;
    }
    shift = snprintf(currentChar, 2, "\n");
    currentChar += shift;
    c = c->nextContainer;
  } while ( c != containerList ) ;
  FILE *file;
  file = fopen(filename, "w");
  
  fwrite(line, sizeof(char), strlen(line), file);
  #ifdef DEBUG2
  printf("Final String: %s\n", line);
  #endif
  free(line);
  fclose(file);
  return true;
}

// readInput reads two lines
// will call createContainer n times during the first line
// will call fitFunction m times during second line
bool readInput(char* filename)
{
  #ifdef UnklareAufgabenstellung
  printf("(╯°□°)╯︵ ┻━┻");
  #endif
  FILE *file;
  // determine filesize
  file = fopen(filename, "r");
  if(file == NULL)
  {
    fprintf(stderr, "Error: File not found or permission denied.\n");
    return false;
  }
  fseek(file, 0, SEEK_END);
  size_t fsize = ftell(file);
  fseek(file, 0, SEEK_SET);
  // Create linebuffer sufficiently large (because of tests)
  // and initialize with w
  char *input = malloc(sizeof(char)*(fsize + 1));
  inputStart = input;
  //size_t readSize = 
  fread(input, fsize, 1, file);
  input[fsize] = 0;
  fclose(file);
  
  bool (*curFunc)(int);
  // start with firstFit if no algo specified
  curFunc = &firstFit;
  
  int i = 0;
  int positionOfN;
  int ns = 0;
  char *c = input;
  while (c != 0 && i < fsize)
  { 
    if (strncmp(c, "\n", 1) == 0)
    {
      positionOfN = i;
      ns++;
    }
    c++;
    i++;
  }
  if(ns != 1)
  {
    fprintf(stderr, "Error: Too many lines in input file.\n");
    return false;  
  }
  
  char *firstLine = malloc(positionOfN+1);
  char *secondLine = malloc(fsize - positionOfN+1);
  firstLineStart = firstLine;
  secondLineStart = secondLine;
  firstLine = strtok(input, "\n");
  secondLine = strtok(NULL, "\n");
  firstLine[positionOfN] = '\0';
  char *val = strtok(firstLine, " "); // Load first value

  int count = 0;
  if(val == NULL)
  {
    fprintf(stderr, "Error: Wrong input format on line 1 (line did not contain spaces)\n");
    return false;
  }
  while (val != NULL)
  {
    count++;
    #ifdef DEBUG2
    printf("val: %s\n",val);
    #endif
    for(i = 0; i < strlen(val);i++)
    {
      // Assuming only numbers on first line. Check whether there are other characters.
      if(isalpha(val[i]))
      {
        fprintf(stderr, "Error: numbers contain illegal characters\n");
        return false;
      }
    }
    int containerSize = atoi(val);
    if(containerSize <= 0)
    {
      fprintf(stderr, "Error: Wrong input format on line 1 (value was not a positive integer)\n");
      return false;
    }
    createContainer(containerSize);
    val = strtok(NULL, " "); // Load next value
  }
  #ifdef DEBUG
    puts("\nSecond line\n");
  #endif
  
  // -- Read second line
  val = strtok(secondLine, " ");
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
          return false;
        }
        curFunc = &firstFit;
      }
      else if(val[0] == 'b')
      {
        if(val[1] != 'f')
        {
          fprintf(stderr, "Error: Not a valid algo token. Expected bf got %s.\n", val);
          return false;
        }
        curFunc = &bestFit;
      }
      else if(val[0] == 'n')
      {
        if(val[1] != 'f')
        {
          fprintf(stderr, "Error: Not a valid algo token. Expected nf got %s.\n", val);
          return false;
        }
        curFunc = &nextFit;
      }
      else if(val[0] == 'a')
      {
        if(val[1] != 'w' || val[2] != 'f')
        {
          fprintf(stderr, "Error: Not a valid algo token. Expected awf got %s.\n", val);
          return false;
        }
        curFunc = &almostWorstFit;
      }
      else
      {
        fprintf(stderr, "Error: Not a valid algo token.\n");
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
        return false;
      }
    }
    val = strtok(NULL, " "); // Load next value
  }
  // Next block checks for line-endings. Proper formatting requires exactly 1 \n
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
  free(inputStart);
  free(firstLineStart);
  free(secondLineStart);
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
