/*
 ************************************
 * Wrappers for system call funcitons
 ************************************
 */
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "wrappers.h"

pid_t Fork(void) {
    int r = fork();
    if (r == -1)
        unix_error("Error calling fork()\n");
    return r;
}

void unix_error(char *msg) {
    fprintf(stderr, msg, NULL);
    /*
     ***************************************
     * Um, shouldn't this be -1 for failure?
     ***************************************
     */
    exit(-1);
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

void *Shmat(int shmid, const void *shmaddr, int shmflg) {
    void* r = shmat(shmid, shmaddr, shmflg);
    if (r == (void *)-1)
        unix_error("Error calling shmget()\n");
    return r;
}

// Detaches the shared memory segment located at the address specified by shmaddr from the address
// space of the calling process.
int Shmdt(const void *shmaddr) {
    int r = shmdt(shmaddr);
    if (r == -1)
        unix_error("Error calling shmdt()\n");
    return r;
}

// For named semaphores
sem_t *Sem_open(const char *name, int oflag, mode_t mode, unsigned int value) {
    sem_t* r = sem_open(name, oflag, mode, value);
    if (r == SEM_FAILED)
        unix_error("Error calling sem_open()\n");
    return r;
}

/****
 * 2?
 ****/
sem_t *Sem_open2(const char *name, int oflag) {

}

int Sem_close(sem_t *sem) {
    int r = sem_close(sem);
    if (r == -1)
        unix_error("Error calling sem_close()\n");
    return r;
}

int Sem_unlink(const char *name) {
    int r = sem_unlink(name);
    if (r == -1)
        unix_error("Error calling sem_unlink()\n");
    return r;
}

int Sem_wait(sem_t *sem) {
    int r = sem_wait(sem);
    if (r == -1)
        unix_error("Error calling sem_wait()\n");
    return r;
}

int Sem_post(sem_t *sem) {
    int r = sem_post(sem);
    if (r == -1)
        unix_error("Error calling sem_post()\n");
    return r;
}

// For unnamed semaphores
int Sem_init(sem_t *sem, int pshared, unsigned int value) {
    int r = sem_init(sem, pshared, value);
    if (r == -1)
        unix_error("Error calling sem_init()\n");
    return r;
}

int Sem_destroy(sem_t *sem) {
    int r = sem_destroy(sem);
    if (r == -1)
        unix_error("Error calling sem_destroy()\n");
    return r;
}

// Returns the message queue identifier associated with the value of the key argument. Also used to
// create with IPC_CREAT flag.
int Msgget(key_t key, int msgflg) {
    int r = msgget(key, msgflg);
    if (r == -1)
        unix_error("Error calling msgget()\n");
    return r;
}

int Msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
    int r = msgsnd(msqid, msgp, msgsz, msgflg);
    if (r == -1)
        unix_error("Error calling msgsnd()\n");
    return r;
}

int Msgrcv(int msqid, void *msgp, size_t maxmsgsz, long msgtyp, int msgflg) {
    int r = msgrcv(msqid, msgp, maxmsgsz, msgtyp, msgflg);
    if (r == -1) {
        switch (errno) {
            case E2BIG:
                printf("E2BIG\n");
                break;
            case EACCES:
                printf("EACCES\n");
                break;
            case EAGAIN:
                printf("EAGAIN\n");
                break;
            case EFAULT:
                printf("EFAULT\n");
                break;
            case EIDRM:
                printf("EIDRM\n");
                break; 
            case EINTR:
                printf("EINTR\n");
                break;
            case EINVAL:
                printf("EINVAL\n");
                break;
            case ENOMSG:
                printf("ENOMSG\n");
                break;
            default:
                printf("Not an identifed error\n");
                break;
            }
        unix_error("Error calling msgrcv()\n");
    }
    return r;
}

int Msgctl(int msqid, int cmd, struct msqid_ds *buf) {
    int r = msgctl(msqid, cmd, buf);
    if (r == -1)
        unix_error("Error calling msgctl()\n");
    return r;
}

