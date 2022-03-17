#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "fifo.h"
#include "become_daemon.h"

int becomeDaemon(int flags)
{
	int maxfd, fd;

	switch (fork()) {
		case -1: return -1;
		case 0:  break;
		default: _exit(EXIT_SUCCESS);
	}

	if (setsid() == -1)
		return -1;

	switch (fork()) {
		case -1: return -1;
		case 0:  break;
		default: _exit(EXIT_SUCCESS);
	}

	if (!(flags & BD_NO_UMASK0))
		umask(0);

	if (!(flags & BD_NO_CHDIR))
		chdir("/");

	if (!(flags & BD_NO_CLOSE_FILES)) {
		maxfd = sysconf(_SC_OPEN_MAX);
	if (maxfd == -1)
		maxfd = BD_MAX_CLOSE;

	for (fd = 0; fd < maxfd; fd++)
		close(fd);
	}

	if (!(flags & BD_NO_REOPEN_STD_FDS)) {
		close(STDIN_FILENO);

	fd = open("/dev/null", O_RDWR);

	if (fd != STDIN_FILENO)
		return -1;
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		return -1;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	becomeDaemon(0);
	int serverFd, dummyFd, clientFd;
	char clientFifo[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response resp;

	umask(0);
	if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
		_exit(1);
	}
	serverFd = open(SERVER_FIFO, O_RDONLY);
	if (serverFd == -1) {
        	_exit(1);
	}

	dummyFd = open(SERVER_FIFO, O_WRONLY);
	if (dummyFd == -1) {
		_exit(1);
	}

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		_exit(1);
	}

	for (;;) {
		if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
			fprintf(stderr, "Error reading request; discarding\n");
			continue;
		}

	snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
	clientFd = open(clientFifo, O_WRONLY);
	if (clientFd == -1) {
		continue;
	}

	strcpy(resp.message, req.message);
	if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
		fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
	close(clientFd);
	}
	exit(EXIT_SUCCESS);
}