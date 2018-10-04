#include <semaphore.h>

typedef struct {
    int partsMade, partsRemaining;
    sem_t SHMaccess_sem;
} shm
