#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include "supervisor.h"
#include "ipc.h"
#include "wrappers.h"

void main(int argc, char *argv[]) {
    // Local variables
    int linesActive;
    // Semaphore variables
    sem_t *factoryLinesDone_sem;
    sem_t *printFinalReport_sem;
    sem_t *finalReportPrinted_sem;
    // Message Queue variables
    key_t msgkey;
    int msgid;
    msgBuf *msg;
    // Shared memory variables
    int shmid;
    key_t shmkey;
    int shmflg;
    shmData *shmP;

    // Handle command line argument *COMPLETE*
    if (argc != 2) {
        printf("Format: ./supervisor lines_active\n");
        exit(-1);
    }
    linesActive = atoi(argv[1]);

    // Connect to message queue
    msgkey = ftok("message queue", 0);
    msgid = Msgget(msgkey, MSGFLG);

    // Connect to shared memory
    shmkey = ftok(SHMPATH, 0);
    shmid = Shmget(shmkey, SHMEM_SIZE, SHMFLG);
    shmP = Shmat(shmid, NULL, 0);

    // Connect to semaphores *COMPLETE* 
    factoryLinesDone_sem = Sem_open("/factoryLinesDone", O_CREAT, SEMFLG, 0);
    printFinalReport_sem = Sem_open("/printFinalReport", O_CREAT, SEMFLG, 0);
    finalReportPrinted_sem = Sem_open("/finalReportPrinted", O_CREAT, SEMFLG, 0);

    // Create per-factory-line production aggregates (num-items-built, num-iteratons)
    flineAggr aggrs[linesActive]; // Defined in supervisor.h
    for (int i = 0; i < linesActive; i++) {
        aggrs[i].itemsBuilt = 0;
        aggrs[i].iterations = 0;
    }

    printf("SUPER: Starting\n");
    while (linesActive > 0) {

        // Receive a message from the message queue
        Msgrcv(msgid, msg, MSG_INFO_SIZE, 0, MSGFLG); 

        // If (Production message)
        if(msg->msgType == 1)
        {
            // Print("Factory Line %d produced %d parts in %d milliSecs", data...)
            printf("SUPER: Factory Line %d produced %d parts in %d milliSecs\n", msg->body.factory_id, msg->body.parts_made, msg->body.duration);
            // update per-factory-line production aggregates
            aggrs[msg->body.factory_id].itemsBuilt += msg->body.parts_made;
            aggrs[msg->body.factory_id].iterations++;
        }
        // else if (Termination message)
        else if(msg->msgType == 1)
        {
            // LinesActive--
            linesActive--;
            // Print("Factory Line %d Terminated", data...)
            printf("SUPER: Factory Line %d Completed its task\n", msg->body.factory_id);
        }
        // else

            // Discard unsupported message
    }

    // Inform parent that all factory lines have completed *COMPLETE*
    Sem_post(factoryLinesDone_sem);

    // Wait for permission from parent to start printing production aggregates *COMPLETE*
    Sem_wait(printFinalReport_sem);

    // Print per-factory-line production aggregates *COMPLETE*
    printf("****** SUPER: Final Report ******\n");
    for (int i = 0; i < linesActive; i++)
        printf("Line   %3d made total of %4d parts in %5d iterations\n", i, aggrs[i].itemsBuilt,
            aggrs[i].iterations);
    printf("==============================\n");

    // Need to print the parts made vs order total

    // Inform parent that the final report has been printed *COMPLETE*
    Sem_post(finalReportPrinted_sem);

    // IPC cleanup *Only semaphores taken care of*
    Sem_close(factoryLinesDone_sem);
    Sem_close(printFinalReport_sem);
    Sem_close(finalReportPrinted_sem);
}
