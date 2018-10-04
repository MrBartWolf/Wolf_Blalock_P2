
void main(int argc, char *argv[]) {
    // Set up shared memory and initialize its objects.

    // Set up the message queue

    // Fork/Execute supervisor process to run in a separate terminal
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

    // Wait for supervisor to collect aggregates from all factory lines

    // Print "supervisor says all lines have been completed"

    // Give permission to supervisor to print final report

    // Wait for supervisor to finish printing

    // Clean up after zombie processes (supervisor + all factory lines

    // Destroy any objects in shared memory

    // Destroy shared memory

    // Destroy message queue
}
