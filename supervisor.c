#include <fcntl.h>
#include <stdlib.h>
#include "supervisor.h"
#include "ipc.h"

void main(int argc, char *argv[]) {
    // Local variables
    int linesActive;
    // Semaphore variables
    sem_t *factoryLinesDone_sem;
    sem_t *printFinalReport_sem;
    sem_t *finalReportPrinted_sem;

    // Handle command line argument *COMPLETE*
    if (argc != 2) {
        printf("Format: ./supervisor lines_active\n");
        exit(-1);
    }
    linesActive = atoi(argv[1]);

    // Connect to message queue

    // Connect to shared memory?

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

    while (linesActive > 0) {

        // Receive a message from the message queue

        // If (Production message)

            // Print("Factory Line %d produced %d parts in %d milliSecs", data...)

            // update per-factory-line production aggregates

        // else if (Termination message)

            // LinesActive--

            // Print("Factory Line %d Terminated", data...)

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
        printf("Line %3d made total of %4d parts in %5d iterations\n", i, aggrs[i].itemsBuilt,
            aggrs[i].iterations);

    // Inform parent that the final report has been printed *COMPLETE*
    Sem_post(finalReportPrinted_sem);

    // IPC cleanup *Only semaphores taken care of*
    Sem_close(factoryLinesDone_sem);
    Sem_close(printFinalReport_sem);
    Sem_close(finalReportPrinted_sem);
}
