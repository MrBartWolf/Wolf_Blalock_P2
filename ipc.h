#include <semaphore.h>
#include <sys/msg.h>

typedef struct {
    int partsMade, partsRemaining;
} shmData;

typedef struct {
    long msgType;
    struct {
        int factory_id;
        int capacity;
        int parts_made;
        int duration;
    } body;
} msgBuf;

#define SHMEM_SIZE sizeof(shmData)

#define SHMFLG (IPC_CREAT | S_IRUSR | S_IWUSR)

#define SEMFLG (S_IRUSR | S_IWUSR)

#define MSG_INFO_SIZE (sizeof(msgBuf) - sizeof(long))

#define MSGFLG (IPC_CREAT | S_IRUSR | S_IWUSR)
