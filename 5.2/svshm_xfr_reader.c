#include "svshm_xfr.h"

int
main(int argc, char *argv[])
{
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;

    semid = semget(ftok(argv[1], *((int*) argv[2])), 0, 0);
    if (semid == -1) {
    	printf("semget\n");
    	exit(EXIT_FAILURE);
    }

    shmid  = shmget(ftok(argv[1], *((int*) argv[2])), 0, 0);
    if (shmid == -1) {
    	printf("shmget\n");
    	exit(EXIT_FAILURE);
    }

    shmp = shmat(shmid, NULL, SHM_RDONLY);
    if (shmp == (void *) -1) {
    	printf("shmat\n");
    	exit(EXIT_FAILURE);
    }

    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (reserveSem(semid, READ_SEM) == -1) {
        	printf("reserveSem\n");
    		exit(EXIT_FAILURE);
    	}

        if (shmp->cnt == 0)
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
        	printf("partial/failed write\n");
    		exit(EXIT_FAILURE);
    	}
    	
        if (releaseSem(semid, WRITE_SEM) == -1) {
        	printf("releaseSem\n");
    		exit(EXIT_FAILURE);
    	}
    }

    if (shmdt(shmp) == -1) {
    	printf("shmdt\n");
    	exit(EXIT_FAILURE);
    }

    if (releaseSem(semid, WRITE_SEM) == -1) {
    	printf("releaseSem\n");
    	exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
