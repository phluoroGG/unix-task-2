#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <signal.h>

void sig_handler(int sig)
{
	if (sig == SIGCONT) {}
}

int main(int argc, char** argv)
{
	if (signal(SIGCONT, sig_handler) == SIG_ERR)
		printf("can't catch SIGCONT\n");
	FILE* f = fopen("data", "r");
	int fd = fileno(f);
	pid_t pid = getpid();
	while(flock(fd, LOCK_EX) == 0) {
		char buffer[255];
                while (1) {
			fgets(buffer, 255, f);
			if (!feof(f)) printf("Server reads : %s", buffer);
			else break;
                }
		fflush(NULL);
		rewind(f);
		flock(fd, LOCK_UN);
		pause();
	}
	fclose(f);
	return 0;
} 
