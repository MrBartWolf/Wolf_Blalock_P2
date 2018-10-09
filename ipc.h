#include <semaphore.h>

typedef struct {
    int partsMade, partsRemaining;
} shmData;

#define SHMEM_SIZE sizeof(shmData)

#define SHMFLG (IPC_CREAT | S_IRUSR | S_IWUSR)

#define SEMFLG (S_IRUSR | S_IWUSR)
