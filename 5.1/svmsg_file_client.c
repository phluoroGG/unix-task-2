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

    req.mtype = 1;
    req.clientId = clientId;
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';

    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1) {
    	printf("msgsnd\n");
    	exit(EXIT_FAILURE);
    }

    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1) {
    	printf("msgrcv\n");
    	exit(EXIT_FAILURE);
    }

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);
        exit(EXIT_FAILURE);
    }

    totBytes = msgLen;
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
        	printf("msgrcv\n");
    		exit(EXIT_FAILURE);
    	}

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long) totBytes, numMsgs);

    exit(EXIT_SUCCESS);
}
