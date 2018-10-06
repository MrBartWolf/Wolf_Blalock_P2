/*************************************************************************************************
 * I haven't added import statements yet, so go ahead if you want to. I like to write comments to 
 * myself by encasing them in asterisks like this, not just comments but also concerns, bugs that
 * need to be fixed, and tasks that need to be completed. I think they will make for a good way to
 * rendevous our thoughts as we punch away at this thing. What do you think?
 *************************************************************************************************/

#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "shmSegment.h"
#include "wrappers.h"

void main(int argc, char *argv[]) {
    int numLines;
    int orderSize;
    // Shared memory variables
    int shmid;
    key_t shmkey;
    int shmflg;
    shmData *shmP;
    // Semaphore variables
    sem_t *shmAccess_sem;
    sem_t *factoryLinesDone_sem;
    sem_t *printFinalReport_sem;
    sem_t *finalReportPrinted_sem;
    sem_t *flinePrint_sem;

    // Error check and parse command line arguments
    if (argc != 3) {
        printf("Format: ./parent n order_size\n");
        exit(-1);
    }
    numLines = atoi(argv[1]);
    orderSize = atoi(argv[2]);

    // Set up shared memory and initialize its objects. *Complete, I believe*
        shmkey = ftok("../shmemsegment.h", 0);
        shmid = Shmget(shmkey, SHMEM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
        shmP = Shmat(shmid, NULL, 0);

        shmP -> partsMade = 0;
        shmP -> partsRemaining = orderSize;

    // Set up the message queue
    
    // Set up semaphores *complete, I believe. Order could be better*
    shmAccess_sem = Sem_open("/shmAccess", O_CREAT, SEMFLG, 1);
    factoryLinesDone_sem = Sem_open("/factoryLinesDone", O_CREAT, SEMFLG, 0);
    printFinalReport_sem = Sem_open("/printFinalReport", O_CREAT, SEMFLG, 0);
    finalReportPrinted_sem = Sem_open("/finalReportPrinted", O_CREAT, SEMFLG, 0);
    flinePrint_sem = Sem_open("/flinePrint", O_CREAT, SEMFLG, 1);

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
    for (int i = 1; i < numLines + 1; i++) {
        // This should return a number in the range of 10 - 50
        capacity = (random() % 41) + 10;
        // This should return a number in the range of 500 - 1200
        duration = (random() % 701) + 500;
        if (Fork() == 0) {
            char *idArg;
            char *capArg;
            char *durArg;
            snprintf(idArg, STRSIZE, "%d", i);
            snprintf(capArg, STRSIZE, "%d", capacity);
            snprintf(durArg, STRSIZE, "%d", duration); 
            /**************************************
             * Execute factory line. Is this right?
             **************************************/
            if (execl("./fline", idArg, capArg, durArg, NULL) < 0) {
                perror("Parent: execl Factory line failed");
                exit(-1);
            }
            /**********************************************************************************
             * Formatting of flags might need to be updated to be more like the example images.
             **********************************************************************************/
            printf("PARENT: Factory Line %d Created with Capacity %ld Duration %ld\n", i, capacity,
                duration);
        }
    }
    

    // Wait for supervisor to collect aggregates from all factory lines *Complete*
    Sem_wait(factoryLinesDone_sem);
    printf("Supervisor says all lines have been completed");

    // Give permission to supervisor to print final report *Complete*
    Sem_post(printFinalReport_sem);

    // Wait for supervisor to finish printing *Complete*
    Sem_wait(finalReportPrinted_sem);

    // Clean up after zombie processes (supervisor + all factory lines)

    // Destroy any objects in shared memory

    // Destroy shared memory

    // Destroy message queue

    // Clean up semaphores *Complete, I think*
    
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
}
