typedef struct {
    long msgType;
    struct {
        int factory_id;
        int capacity;
        int parts_made;
        int duration;
    } body;
} msgBuf;

#define MSG_SIZE (sizeof(msgBuf))

#define MSG_INFO_SIZE (sizeof(msgBuf) - sizeof(long))

#define MSGFLG (IPC_CREAT | S_IRUSR | S_IWUSR)

#define MSGPATH "./msgq.h"
