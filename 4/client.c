#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "fifo.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void removeFifo(void)
{
	unlink(clientFifo);
}

int
main(int argc, char *argv[])
{
	int serverFd, clientFd;
	struct request req;
	struct response resp;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		printf("USAGE: ./client <message_blocks>\n");
		_exit(1);
	}

	umask(0);
	snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
	if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
		printf("mkfifo %s\n", clientFifo);
		_exit(1);
	}

	if (atexit(removeFifo) != 0) {
		printf("atexit\n");
		_exit(1);
	}

	for(int i = 1; i < argc; i++) {
		req.pid = getpid();
		strcpy(req.message, argv[i]);

		serverFd = open(SERVER_FIFO, O_WRONLY);
		if (serverFd == -1) {
			printf("open %s\n", SERVER_FIFO);
			_exit(1);
		}
       
		if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
			printf("Can't write to server\n");
			_exit(1);
		}
	
		clientFd = open(clientFifo, O_RDONLY);
		if (clientFd == -1) {
			printf("open %s\n", clientFifo);
			_exit(1);
		}

		if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
			printf("Can't read response from server\n");
			_exit(1);
		}

		printf("%s\n", resp.message);
	}
	exit(EXIT_SUCCESS);
}
