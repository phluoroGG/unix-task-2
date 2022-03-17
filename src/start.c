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
		printf("USAGE: ./start <message_blocks>\n");
		_exit(1);
	}
	int pipefd[2];
	int status;
	pipe(pipefd);
	for (int i = 1; i < argc; i++) {
		pid_t child_pid = fork();
		switch (child_pid) {
		case -1: perror("fork");
			_exit(1);
		case 0: close(pipefd[0]);
			close(STDOUT_FILENO);
			dup(pipefd[1]);
			execl("./client","./client", argv[i], NULL);
		default: wait(&status);
		}
	}
	close(pipefd[1]);
	close(STDIN_FILENO);
	dup(pipefd[0]);
	execl("./server", "./server", NULL);
	return 0;
}
