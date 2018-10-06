#include <semaphore.h>

typedef struct {
    int partsMade, partsRemaining;
} shmData;

#define SHMEM_SIZE sizeof(shmData)
