/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 48-2 */

/*  svshm_xfr_writer.c

   Read buffers of data data from standard input into a System V shared memory
   segment from which it is copied by svshm_xfr_reader.c

   We use a pair of binary semaphores to ensure that the writer and reader have
   exclusive, alternating access to the shared memory. (I.e., the writer writes
   a block of text, then the reader reads, then the writer writes etc). This
   ensures that each block of data is processed in turn by the writer and
   reader.

   This program needs to be started before the reader process as it creates the
   shared memory and semaphores used by both processes.

   Together, these two programs can be used to transfer a stream of data through
   shared memory as follows:

        $ svshm_xfr_writer < infile &
        $ svshm_xfr_reader > out_file
*/
#include "semun.h"              /* Definition of semun union */
#include "svshm_xfr.h"

int
main(int argc, char *argv[])
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    /* Create set containing two semaphores; initialize so that
       writer has first access to shared memory. */

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

    /* Create shared memory; attach at address chosen by system */

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

    /* Transfer blocks of data from stdin to shared memory */

    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        if (reserveSem(semid, WRITE_SEM) == -1) {
        	printf("reserveSem\n");
    		exit(EXIT_FAILURE);
    	}        /* Wait for our turn */

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
        	printf("read\n");
    		exit(EXIT_FAILURE);
    	}

        if (releaseSem(semid, READ_SEM) == -1) {
        	printf("releaseSem\n");
    		exit(EXIT_FAILURE);
    	}         /* Give reader a turn */

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */

        if (shmp->cnt == 0)
            break;
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

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
