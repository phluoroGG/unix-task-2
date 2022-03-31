/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2022.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 46-9 */

/* svmsg_file_client.c

   Send a message to the server svmsg_file_server.c requesting the
   contents of the file named on the command line, and then receive the
   file contents via a series of messages sent back by the server. Display
   the total number of bytes and messages received. The server and client
   communicate using System V message queues.
*/
#include "svmsg_file.h"

static int clientId;

static void
removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1) {
    	printf("msgctl\n");
    	exit(EXIT_FAILURE);
    }
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int serverId, numMsgs;
    ssize_t msgLen, totBytes;

    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: ./svmsg_file_client <pathname>\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
        printf("pathname too long (max: %ld bytes)\n",
                (long) sizeof(req.pathname) - 1);
        exit(EXIT_FAILURE);
    }
    /*FIXME: above: should use %zu here, and remove (long) cast */

    /* Get server's queue identifier; create queue for response */

    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1) {
    	printf("msgget - server message queue\n");
    	exit(EXIT_FAILURE);
    }

    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1) {
    	printf("msgget - client message queue\n");
    	exit(EXIT_FAILURE);
    }

    if (atexit(removeQueue) != 0) {
    	printf("atexit\n");
    	exit(EXIT_FAILURE);
    }

    /* Send message asking for file named in argv[1] */

    req.mtype = 1;                      /* Any type will do */
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
                                        /* Ensure string is terminated */

    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) {
    	printf("msgsnd\n");
    	exit(EXIT_FAILURE);
    }

    /* Get first response, which may be failure notification */

    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1) {
    	printf("msgrcv\n");
    	exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

    totBytes = msgLen;                  /* Count first message */
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
        	printf("msgrcv\n");
    		exit(EXIT_FAILURE);
    	}

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long) totBytes, numMsgs);
    /*FIXME: above: should use %zd here, and remove (long) cast (or perhaps
       better, make totBytes size_t, and use %zu)*/

    exit(EXIT_SUCCESS);
}
