#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "msgq.h"
#include "sem.h"
#include "shm.h"
#include "wrappers.h"

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

    // Error check and parse command line arguments
    if (argc != 3) {
        printf("Format: ./parent num_lines order_size\n");
        exit(-1);
    }
    numLines = atoi(argv[1]);
    orderSize = atoi(argv[2]);
    printf("PARENT: Will Manufacture an Order of Size = %d parts\n", orderSize);

    // Set up shared memory and initialize its objects.
    shmkey = ftok(SHMPATH, 0);
    shmid = Shmget(shmkey, SHMEM_SIZE, SHMFLG);
    shmP = Shmat(shmid, NULL, 0);

    shmP -> orderSize = orderSize;
    shmP -> partsMade = 0;
    shmP -> partsRemaining = orderSize;

    // Set up the message queue
    msgkey = ftok(MSGPATH, 0); //I don't think the parent actually needs the mailbox
    msgid = Msgget(msgkey, MSGFLG);
    
    // Set up semaphores
    shmAccess_sem = Sem_open("/shmAccess", O_CREAT, SEMFLG, 1);
    flinePrint_sem = Sem_open("/flinePrint", O_CREAT, SEMFLG, 1);
    factoryLinesDone_sem = Sem_open("/factoryLinesDone", O_CREAT, SEMFLG, 0);
    printFinalReport_sem = Sem_open("/printFinalReport", O_CREAT, SEMFLG, 0);
    finalReportPrinted_sem = Sem_open("/finalReportPrinted", O_CREAT, SEMFLG, 0);

    printf("Creating %d Factory Lines\n", numLines);

    // Fork/Execute supervisor process to run in a separate terminal
    int superID = Fork();
    if (superID == 0) {
        char cmnd[1024];
        sprintf(cmnd, "./supervisor %d ; exec bash", numLines);
        if (execlp("/usr/bin/gnome-terminal", "SuperVterm", "--", "/bin/bash", "-c", cmnd,
                NULL) < 0) {
            perror("PARENT: execlp Supervisor Failed");
            exit(-1);
        }
    } 

    // Create/execute all factory line processes in same terminal as parent
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
            char idArg[1024];
            char capacityArg[1024];
            char durationArg[1024];
            sprintf(idArg, "%d", i + 1);
            sprintf(capacityArg, "%ld", capacity);
            sprintf(durationArg, "%ld", duration);
            if (execl("./fline", "fline", idArg, capacityArg, durationArg, NULL) < 0) {
                perror("Parent: execl Factory line failed");
                exit(-1);
            }
        }
        printf("PARENT: Factory Line %4d Created with Capacity %4ld Duration %4ld\n", i, capacity,
                duration);
    }

    // Wait for supervisor to collect aggregates from all factory lines
    Sem_wait(factoryLinesDone_sem);
    printf("PARENT: Supervisor says all lines have been completed\n");

    // Give permission to supervisor to print final report
    Sem_post(printFinalReport_sem);

    // Wait for supervisor to finish printing
    Sem_wait(finalReportPrinted_sem);

    // Clean up after zombie processes )(supervisor + all factory lines)
    waitpid(superID, NULL, 0);
    printf("PARENT: Shutting Down Factory Lines\n");
    for (int i = 0; i < numLines; i++) {
        waitpid(factoryID[i], NULL, 0);
    }

    // Destroy any objects in shared memory

    // Destroy shared memory
    Shmdt(shmP);
    shmctl(shmid, IPC_RMID, NULL);

    // Destroy message queue
    Msgctl(msgid, IPC_RMID, NULL);

    // Clean up semaphores
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
