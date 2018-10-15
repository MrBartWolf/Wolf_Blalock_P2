/*************************************************************************************************
 * I like to write comments to myself by encasing them in asterisks like this, not just comments
 * but also concerns, bugs that need to be fixed, and tasks that need to be completed. I think they
 * will make for a good way to rendevous our thoughts as we punch away at this thing. What do you
 * think?
 *************************************************************************************************/

#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "ipc.h"
#include "wrappers.h"
#include <sys/msg.h>

void main(int argc, char *argv[]) {
    // Local variables
    int numLines;
    int orderSize;
    // Shared memory variables
    int shmid;
    key_t shmkey;
    int shmflg;
    shmData *shmP;
    // Message Queue variables
    key_t msgkey;
    int msgid;
    msgBuf *msg;
    // Semaphore variables
    sem_t *shmAccess_sem;
    sem_t *flinePrint_sem;
    sem_t *factoryLinesDone_sem;
    sem_t *printFinalReport_sem;
    sem_t *finalReportPrinted_sem;

    // Error check and parse command line arguments *COMPLETE*
    if (argc != 3) {
        printf("Format: ./parent num_lines order_size\n");
        exit(-1);
    }
    numLines = atoi(argv[1]);
    orderSize = atoi(argv[2]);

    // Set up shared memory and initialize its objects. *Complete, I believe*
    shmkey = ftok(SHMPATH, 0);
    shmid = Shmget(shmkey, SHMEM_SIZE, SHMFLG);
    shmP = Shmat(shmid, NULL, 0);

    shmP -> orderSize = orderSize;
    shmP -> partsMade = 0;
    shmP -> partsRemaining = orderSize;

    // Set up the message queue
    msgkey = ftok(MSGPATH, 0); //I don't think the parent actually needs the mailbox
    msgid = Msgget(msgkey, MSGFLG);
    
    // Set up semaphores *complete, I believe. Order could be better*
    shmAccess_sem = Sem_open("/shmAccess", O_CREAT, SEMFLG, 1);
    flinePrint_sem = Sem_open("/flinePrint", O_CREAT, SEMFLG, 1);
    factoryLinesDone_sem = Sem_open("/factoryLinesDone", O_CREAT, SEMFLG, 0);
    printFinalReport_sem = Sem_open("/printFinalReport", O_CREAT, SEMFLG, 0);
    finalReportPrinted_sem = Sem_open("/finalReportPrinted", O_CREAT, SEMFLG, 0);

    // Fork/Execute supervisor process to run in a separate terminal *Complete I think but be sure*
    int superID = Fork();
    if (superID == 0) {
        char *cmnd;
        snprintf(cmnd, STRSIZE, "./supervisor %d ; exec bash", numLines);
        if (execlp("/usr/bin/gnome-terminal", "SuperVterm", "--", "/bin/bash", "-c", cmnd,
                NULL) < 0) {
            perror("PARENT: execlp Supervisor Failed");
            exit(-1);
        }
    } 

    // Create/execute all factory line processes in same terminal as parent *Needs work*
    srandom(time(NULL));
    long capacity;
    long duration;
    pid_t factoryID[numLines];
    for (int i = 0; i < numLines; i++) {
        // This should return a number in the range of 10 - 50
        capacity = (random() % 41) + 10;
        // This should return a number in the range of 500 - 1200
        duration = (random() % 701) + 500;
        factoryID[i] = Fork();
        if (factoryID[i] == 0) {
            char *idArg;
            char *capArg;
            char *durArg;
            snprintf(idArg, STRSIZE, "%d", i + 1);
            snprintf(capArg, STRSIZE, "%d", capacity);
            snprintf(durArg, STRSIZE, "%d", duration); 
            if (execl("./fline", "fline", idArg, capArg, durArg, NULL) < 0) {
                perror("Parent: execl Factory line failed");
                exit(-1);
            }
        }
        /************************************************************************************
         * Formatting of flags might need to be updated to be more like the example images. I
         * don't know, do you think it's a big deal?
         ************************************************************************************/
        printf("PARENT: Factory Line %d Created with Capacity %ld Duration %ld\n", i, capacity,
                duration);
    }

    // Wait for supervisor to collect aggregates from all factory lines *COMPLETE*
    Sem_wait(factoryLinesDone_sem);
    printf("PARENT: Supervisor says all lines have been completed\n");

    // Give permission to supervisor to print final report *COMPLETE*
    Sem_post(printFinalReport_sem);

    // Wait for supervisor to finish printing *COMPLETE*
    Sem_wait(finalReportPrinted_sem);

    // Clean up after zombie processes )(supervisor + all factory lines) *COMPLETE*
    waitpid(superID, NULL);
    printf("PARENT: Shutting Down Factory Lines\n");
    for (int i = 0; i < numLines; i++) {
        waitpid(factoryID[i], NULL);
    }

    // Destroy any objects in shared memory

    // Destroy shared memory
    Shmdt(shmP);
    shmctl(shmid, IPC_RMID, NULL);

    // Destroy message queue *COMPLETE*
    Msgctl(msgid, IPC_RMID, NULL);

    // Clean up semaphores *COMPLETE*
    Sem_close(shmAccess_sem);
    Sem_close(factoryLinesDone_sem);
    Sem_close(printFinalReport_sem);
    Sem_close(finalReportPrinted_sem);
    Sem_close(flinePrint_sem);

    Sem_unlink("/shmAccess");
    Sem_unlink("/factoryLinesDone");
    Sem_unlink("/printFinalReport");
    Sem_unlink("/finalReportPrinted");
    Sem_unlink("/flinePrint");

    // Anything else?
}
