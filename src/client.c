#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#define BLOCK 512

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("USAGE: ./client <message>\n");
		_exit(1);
	}
	int sz;
	for (int i=1; i<argc; i++)
		sz = write(STDOUT_FILENO, argv[i], BLOCK);
	_exit(0);
}