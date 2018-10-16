#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/msg.h>

pid_t   Fork(void);
void    unix_error(char *msg) ;
int     Shmget(key_t key, size_t size, int shmflg);
void   *Shmat(int shmid, const void *shmaddr, int shmflg);
int     Shmdt( const void *shmaddr ) ;

sem_t  *Sem_open( const char *name, int oflag, mode_t mode, unsigned int value );
sem_t  *Sem_open2( const char *name, int oflag ) ;
int     Sem_close( sem_t *sem );
int     Sem_unlink( const char *name );

int     Sem_wait( sem_t *sem ) ;
int     Sem_post( sem_t *sem ) ;
int     Sem_init( sem_t *sem, int pshared, unsigned int value ) ;
int     Sem_destroy( sem_t *sem ) ;

int     Msgget( key_t key, int msgflg );
int     Msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
int     Msgrcv(int msqid, void *msgp, size_t maxmsgsz, long msgtyp, int msgflg);
int     Msgctl(int msqid, int cmd, struct msqid_ds *buf);
