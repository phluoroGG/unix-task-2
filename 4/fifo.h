#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_FIFO "/tmp/fifo_sv"
#define CLIENT_FIFO_TEMPLATE "/tmp/fifo_cl.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
#define BLOCK 512

struct request {
	pid_t pid;
	char message[BLOCK];
};

struct response {
	char message[BLOCK];
};
