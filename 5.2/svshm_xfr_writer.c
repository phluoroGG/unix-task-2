#include "semun.h"
#include "svshm_xfr.h"

int
main(int argc, char *argv[])
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    semid = semget(ftok(argv[1], *((int*) argv[2])), 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
    	printf("semget\n");
    	exit(EXIT_FAILURE);
    }

    if (initSemAvailable(semid, WRITE_SEM) == -1) {
    	printf("initSemAvailable\n");
    	exit(EXIT_FAILURE);
    }
    if (initSemInUse(semid, READ_SEM) == -1) {
    	printf("initSemInUse\n");
    	exit(EXIT_FAILURE);
    }

    shmid = shmget(ftok(argv[1], *((int*) argv[2])), sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    if (shmid == -1) {
    	printf("shmget\n");
    	exit(EXIT_FAILURE);
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) {
    	printf("shmat\n");
    	exit(EXIT_FAILURE);
    }

    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        if (reserveSem(semid, WRITE_SEM) == -1) {
        	printf("reserveSem\n");
    		exit(EXIT_FAILURE);
    	}

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
        	printf("read\n");
    		exit(EXIT_FAILURE);
    	}

        if (releaseSem(semid, READ_SEM) == -1) {
        	printf("releaseSem\n");
    		exit(EXIT_FAILURE);
    	}

        if (shmp->cnt == 0)
            break;
    }

    if (reserveSem(semid, WRITE_SEM) == -1) {
    	printf("reserveSem\n");
    	exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
    	printf("semctl\n");
    	exit(EXIT_FAILURE);
    }
    if (shmdt(shmp) == -1) {
    	printf("shmdt\n");
    	exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
    	printf("shmctl\n");
    	exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
