/*************************************************************************************************
 * I haven't added import statements yet, so go ahead if you want to. I like to write comments to 
 * myself by encasing them in asterisks like this, not just comments but also concerns, bugs that
 * need to be fixed, and tasks that need to be completed. I think they will make for a good way to
 * rendevous our thoughts as we punch away at this thing. What do you think?
 *************************************************************************************************/

#include <sstdlib>

void main(int argc, char *argv[]) {
    int n;
    int order_size;
    int shmid;
    key_t shmkey;
    int shmflg;
    shmData shmP;
    sem_t *factorySMaccess_sem;
    sem_t *factoryLinesDone_sem;
    sem_t *printFinalReport_sem;
    sem_t *finalReportPrinted_sem;

    // Error check and parse command line arguments
    if (argc != 3) {
        printf("Format: ./parent n order_size\n");
        exit(-1);
    }
    n = atoi(argv[1]);
    order_size = atoi(argv[2]);

    // Set up shared memory and initialize its objects. *Partway complete*
        /**********************************************
         * Is this a legit call? Am I doing this right?
         **********************************************/
        shmkey = ftok("../shmemsegment.h", 0);
        /**********************************************
         * Consider defining flag outside of this file.
         **********************************************/
        shmflg = IPC_CREAT | S_IRUSR | S_IWUSR;
        shmid = Shmget(shmkey, SHMEM_SIZE, shmflg);
        shmP = Shmat(shmid, NULL, 0);

        shmP -> partsMade = 0;
        shmP -> partsRemaining = order_size;

    // Set up the message queue
    
    // Set up semaphores *Partway complete*
    factoryLinesDone_sem = Sem_open("/factoryLinesDone", O_CREAT, SEMFLG, 0);
    printFinalReport_sem = Sem_open("/printFinalReport", O_CREAT, SEMFLG, 0);
    finalReportPrinted_sem = Sem_open("/finalReportPrinted", O_CREAT, SEMFLG, 0);

    /*********************************************************************************************
     * I'm thinking for efficiency's sake (and to impress the professor, we should have the shared
     * memory read/write semaphore be unnamed. What do you think?
     *********************************************************************************************/

    // Fork/Execute supervisor process to run in a separate terminal *Complete I think but be sure*
    superID = Fork();
    if (superID == 0) {
        snprintf(cmnd, STRSIZE, "./supervisor %d ; exec bash", numLines);
        if (execlp("/usr/bin/gnome-terminal", "SuperVterm", "--", "/bin/bash", "-c", cmnd,
                NULL) < 0) {
            perror("PARENT: execlp Supervisor Failed");
            exit(-1);
        }
    } 

    // Create/execute all factory line processes in same terminal as parent

    // Wait for supervisor to collect aggregates from all factory lines *Complete, I think*
    sem_wait(factoryLinesDone_sem);
    printf("Supervisor says all lines have been completed");

    // Give permission to supervisor to print final report *Complete*
    sem_post(printFinalReport_sem);

    // Wait for supervisor to finish printing *Complete*
    sem_wait(finalReportPrinted_sem);

    // Clean up after zombie processes (supervisor + all factory lines)

    // Destroy any objects in shared memory

    // Destroy shared memory

    // Destroy message queue

    // Clean up semaphores
}
