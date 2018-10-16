typedef struct {
    int orderSize, partsMade, partsRemaining;
} shmData;

#define SHMEM_SIZE sizeof(shmData)

#define SHMFLG (IPC_CREAT | S_IRUSR | S_IWUSR)

#define SHMPATH "./shm.h"
