#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include "ipc.h"
#include "wrappers.h"
#include <sys/msg.h>

void main(int argc, char *argv[]) {
    // Local variables
    int myId;
    int capacity;
    int duration;
    int iterations;
    int partsThisIteration;
    int partsMadeByMe;
    // Shared memory variables
    int shmid;
    key_t shmkey;
    shmData *shmP;
    // Semaphore variables
    sem_t *shmAccess_sem;
    sem_t *flinePrint_sem;
    // Message Queue variables
    key_t msgkey;
    int msgid;
    msgBuf *msg;

    // Handle command line arguments *Complete, I think*
    if (argc != 4) {
        printf("Format: ./fline myId, capacity, duration\n");
        exit(-1);
    }
    myId = atoi(argv[1]);
    capacity = atoi(argv[2]);
    duration = atoi(argv[3]);

    // Connect to shared memory *Incomplete*
    /****************************************************************
     * Shmkey? How do we get that if the parent process generated it?
     ****************************************************************/
    shmkey = ftok(SHMPATH, 0); 
    shmid = Shmget(shmkey, SHMEM_SIZE, SHMFLG);
    shmP = Shmat(shmid, NULL, 0);

    //Connect to Message queue
    msgkey = ftok(MSGPATH, 0);
    msgid = Msgget(msgkey, MSGFLG);

    // Connect to / open semaphores. *Complete*
    shmAccess_sem = Sem_open("/shmAccess", O_CREAT, SEMFLG, 1);
    flinePrint_sem = Sem_open("/flinePrint", O_CREAT, SEMFLG, 1);

    // Check number of remaining parts and adjust
    Sem_wait(shmAccess_sem);
    while (shmP -> partsRemaining > 0) {
        // Determine how many to make and update remain *COMPLETE*
        if (shmP -> partsRemaining >= capacity)
            partsThisIteration = capacity;
        else
            partsThisIteration = shmP -> partsRemaining;
        /****************************************************************************************
         * Are we supposed to update partsMade here or is the supervisor supposed to do that? Or
         * does there even need to be a partsMade variable at all? I think he mentioned something
         * about it but I am not sure the presence of one is necessary for the execution of this
         * program.
         ****************************************************************************************/
        // shmP -> partsMade += partsThisIteration;
        shmP -> partsRemaining -= partsThisIteration;
        Sem_post(shmAccess_sem);

        // Print factory line production message *COMPLETE*
        Sem_wait(flinePrint_sem);
        printf("Factory Line %d: Going to make %d parts in %d milliSecs", myId, partsThisIteration,
            duration);
        Sem_post(flinePrint_sem);

        // sleep to simulate production time *COMPLETE*
        usleep(duration);

        // Create and send production message *COMPLETE*
        msg->msgType = 1; //This is a production message
        msg->body.factory_id = myId;
        msg->body.capacity = capacity;
        msg->body.parts_made = partsThisIteration;
        msg->body.duration = duration;
        Msgsnd(msgid, &msg, MSG_INFO_SIZE, 0);

        // Increment # iterations *COMPLETE*
        iterations++;

        // Update total # parts made by me *COMPLETE*
        partsMadeByMe += partsThisIteration;

        // Lock the semaphore to read shared memory for the conditional. *COMPLETE*
        Sem_wait(shmAccess_sem);
    }
    // Unlock shared memory semaphore, for it is no longer needed. *COMPLETE*
    Sem_post(shmAccess_sem);

    // Create and send completion message *COMPLETE*
    msg->msgType = 2; //This is a termination message
    msg->body.factory_id = myId;
    msg->body.capacity = capacity;
    msg->body.parts_made = partsMadeByMe;
    msg->body.duration = duration;
    Msgsnd(msgid, &msg, MSG_INFO_SIZE, 0);

    // Detach from shared memory *Complete I think*
    Shmdt(shmP);

    // Close semaphores *COMPLETE*
    Sem_close(shmAccess_sem);
    Sem_close(flinePrint_sem);
}
