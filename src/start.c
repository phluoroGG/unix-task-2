#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define BLOCK 512

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("USAGE: ./start <client_count>\n");
		_exit(1);
	}
	int count = strtol(argv[1], NULL, 10);
	if (count  < 1) {
		return 0;
	}
	int pipefd[2];
	pipe(pipefd);
	for (int i = 0; i < count; i++) {
		pid_t child_pid = fork();
		switch (child_pid) {
		case -1: perror("fork");
			_exit(1);
		case 0: close(pipefd[0]);
			close(STDOUT_FILENO);
			dup(pipefd[1]);
			execl("./client","./client", "Hello", NULL);
		default: break;
		}
	}
	close(pipefd[1]);
	close(STDIN_FILENO);
	dup(pipefd[0]);
	execl("./server", "./server", NULL);
	return 0;
}
