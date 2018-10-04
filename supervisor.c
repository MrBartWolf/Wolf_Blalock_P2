void main(int argc, char *argv[]) {
    // Establish IPC semaphores and shared memory.

    // LinesActive = argv[1]

    // While (LinesActive > 0) {

        // Receive a message from the message queue

        // If (Production message)

            // Print("Factory Line %d produced %d parts in %d milliSecs", data...)

            // update per-factory-line production aggregates (num-items-built, num-iterations)

        // else if (Termination message

            // LinesActive--

            // Print("Factory Line %d Terminated", data...)

        // else

            // Discard unsupported message
    // }

    // Inform parent that all factory lines have completed

    // Wait for permission from parent to start printing production aggregates

    // Print per-factory-line production aggregates
}
