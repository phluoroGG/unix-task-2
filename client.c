#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc < 3) {
		puts("Usage: ./client <server_w_pid> <message>");
		return 1;
	}
	FILE* f = fopen("data", "w+");
	int fd = fileno(f);
	pid_t pid = getpid();
	pid_t wpid = (pid_t)strtol(argv[1], NULL, 10);
	if(flock(fd, LOCK_EX) == 0) {
		fflush(NULL);
		for (int i = 2; i < argc; i++) {
			fprintf(f, "%s\n", argv[i]);
			printf("Client writes : %s\n", argv[i]);
		}
		rewind(f);
		flock(fd, LOCK_UN);
		kill(wpid, SIGCONT);
	}
	fclose(f);
	return 0;
} 
