/*
 ************************************
 * Wrappers for system call funcitons
 ************************************
 */
#include "wrappers.h"

pid_t Fork(void) {
    pid_t r = fork();
    if (r == -1)
        unix_error("Error calling fork()\n");
    return pid;
}

void unix_error(char *msg) {
    printf(stderr, msg);
    /*
     ***************************************
     * Um, shouldn't this be -1 for failure?
     ***************************************
     */
    exit(1);
}

// Returns the identifier of the shared memory segment associated with the value of argument "key"
// A new shared memory segment, with equal value to the value of size rounded up to a multiple of
// PAGE_SIZE, is created if key has the value IPC_PRIVATE or key isn't IPC_PRIVATE, no shared
// memory segment corresponsing to key exists, and IPC_CREAT is specified in shmflg.
int Shmget(key_t key, size_t size, int shmflag) {
    int r = shmget(key, size, shmflag);
    if (r == -1)
        unix_error("Error calling shmget()\n");
    return r;
}

void *Shmat(int shmid, const void *shmaddr, int shmflg); {
    int r = shmat(shmid, shmaddr, shmflg);
    if (r == -1)
        unix_error("Error calling shmget()\n");
    return r;
}

// Detaches the shared memory segment located at the address specified by shmaddr from the address
// space of the calling process.
int Shmdt(const void *shmaddr) {
    int r = shmdt(shmaddr);
    if (r == -1)
        unix_error("Error calling shmdt()\n");
    return r
}

// For named semaphores
sem_t *Sem_open(const char *name, int oflag, mode_t mode, unsigned int value) {
    int r = sem_open(name, oflag, mode, value);
    if (r == -1)
        unix_error("Error calling sem_open()\n");
    return r;
}

/****
 * 2?
 ****/
sem_t *Sem_open2(const char *name, int oflag) {

}

int Sem_close(sem_t *sem) {
    r = sem_close(sem);
    if (r == -1)
        unix_error("Error calling sem_close()\n");
    return r;
}

int Sem_unlink(const char *name) {
    r = sem_unlink(name);
    if (r == -1)
        unix_error("Error calling sem_unlink()\n");
    return r;
}

int Sem_wait(sem_t *sem) {
    r = sem_wait(sem);
    if (r == -1)
        unix_error("Error calling sem_wait()\n");
    return r;
}

int Sem_post(sem_t *sem) {
    r = sem_post(sem);
    if (r == -1)
        unix_error("Error calling sem_post()\n");
    return r;
}

// For unnamed semaphores
int Sem_init(sem_t *sem, int pshared, unsigned int value) {
    r = sem_init(sem, pshared, value);
    if (r == -1)
        unix_error("Error calling sem_init()\n");
    return r;
}

int Sem_destroy(sem_t *sem) {
    r = sem_destroy(sem);
    if (r == -1)
        unix_error("Error calling sem_destroy()\n");
    return r;
}

// Returns the message queue identifier associated with the value of the key argument. Also used to
// create with IPC_CREAT flag.
int Msgget(key_t key, int msgflg) {
    r = msgget(key, msgflg);
    if (r == -1)
        unix_error("Error calling msgget()\n");
    return r;
}

