#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "msgq.h"
#include "sem.h"
#include "shm.h"
#include "wrappers.h"

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
    msgBuf msg;

    // Handle command line arguments *Complete, I think*
    if (argc != 4) {
        printf("Format: ./fline myId, capacity, duration\n");
        exit(-1);
    }
    myId = atoi(argv[1]);
    capacity = atoi(argv[2]);
    duration = atoi(argv[3]);

    // Connect to / open semaphores.
    shmAccess_sem = Sem_open("/shmAccess", O_CREAT, SEMFLG, 1);
    flinePrint_sem = Sem_open("/flinePrint", O_CREAT, SEMFLG, 1);

    // Connect to shared memory
    shmkey = ftok(SHMPATH, 0); 
    shmid = Shmget(shmkey, SHMEM_SIZE, SHMFLG);
    shmP = Shmat(shmid, NULL, 0);

    //Connect to Message queue
    msgkey = ftok(MSGPATH, 0);
    msgid = Msgget(msgkey, MSGFLG);

    // Check number of remaining parts and adjust
    Sem_wait(shmAccess_sem);
    printf("Parts remaining: %d\n", shmP -> partsRemaining);
    while (shmP -> partsRemaining > 0) {
        // Determine how many to make and update remain
        if (shmP -> partsRemaining >= capacity)
            partsThisIteration = capacity;
        else
            partsThisIteration = shmP -> partsRemaining;
        // shmP -> partsMade += partsThisIteration;
        shmP -> partsRemaining -= partsThisIteration;
        Sem_post(shmAccess_sem);

        // Print factory line production message
        Sem_wait(flinePrint_sem);
        printf("Factory Line %d: Going to make %d parts in %d milliSecs\n", myId, partsThisIteration,
            duration);
        Sem_post(flinePrint_sem);

        // sleep to simulate production time
        usleep(duration);

        // Create and send production message
        msg.msgType = 1; //This is a production message
        msg.body.factory_id = myId;
        msg.body.capacity = capacity;
        msg.body.parts_made = partsThisIteration;
        msg.body.duration = duration;
        Msgsnd(msgid, &msg, MSG_SIZE, 0);

        // Increment # iterations
        iterations++;

        // Update total # parts made by me
        partsMadeByMe += partsThisIteration;

        // Lock the semaphore to read shared memory for the conditional.
        Sem_wait(shmAccess_sem);
    }
    // Unlock shared memory semaphore, for it is no longer needed.
    Sem_post(shmAccess_sem);

    Sem_wait(flinePrint_sem);
    printf(">>> Factory Line %3d: Terminating after making total of %6d parts in %4d iterations\n",
        myId, partsMadeByMe, iterations);
    Sem_post(flinePrint_sem);

    // Create and send completion message
    msg.msgType = 2; //This is a termination message
    msg.body.factory_id = myId;
    msg.body.capacity = capacity;
    msg.body.parts_made = partsMadeByMe;
    msg.body.duration = duration;
    Msgsnd(msgid, &msg, MSG_INFO_SIZE, 0);

    // Detach from shared memory
    Shmdt(shmP);

    // Close semaphores
    Sem_close(shmAccess_sem);
    Sem_close(flinePrint_sem);
}
