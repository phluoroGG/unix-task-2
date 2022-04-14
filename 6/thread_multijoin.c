#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

static int totThreads = 0;
static int numLive = 0;
static int numUnjoined = 0;  

enum tstate {
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED
};

static struct {
    pthread_t tid;
    enum tstate state;
    char *arr;
} *thread;

static void *
threadFunc(void *arg)
{
    int idx = (int) arg;
    int s;

    s = pthread_mutex_lock(&threadMutex);
    if (s != 0) {
    	printf("Error in function pthread_mutex_lock\n");
    	exit(EXIT_FAILURE);
    }

    numUnjoined++;
    thread[idx].state = TS_TERMINATED;

    s = pthread_mutex_unlock(&threadMutex);
    if (s != 0) {
    	printf("Error in function pthread_mutex_unlock\n");
    	exit(EXIT_FAILURE);
    }
    s = pthread_cond_signal(&threadDied);
    if (s != 0) {
    	printf("Error in function pthread_cond_signal\n");
    	exit(EXIT_FAILURE);
    }

    return (void *) thread[idx].arr;
}

int
main(int argc, char *argv[])
{
    int s, idx;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    	printf("Usage: %s message blocks\n", argv[0]);
    	exit(EXIT_FAILURE);
    }

    thread = calloc(argc - 1, sizeof(*thread));
    if (thread == NULL) {
    	printf("Error in function calloc\n");
    	exit(EXIT_FAILURE);
    }

    for (idx = 0; idx < argc - 1; idx++) {
        thread[idx].arr = argv[idx + 1];
        thread[idx].state = TS_ALIVE;
        s = pthread_create(&thread[idx].tid, NULL, threadFunc, (void *) idx);
        if (s != 0) {
    		printf("Error in function pthread_create\n");
    		exit(EXIT_FAILURE);
    	}
    }

    totThreads = argc - 1;
    numLive = totThreads;

    while (numLive > 0) {
        s = pthread_mutex_lock(&threadMutex);
        if (s != 0) {
    		printf("Error in function pthread_mutex_lock\n");
    		exit(EXIT_FAILURE);
    	}

        while (numUnjoined == 0) {
            s = pthread_cond_wait(&threadDied, &threadMutex);
            if (s != 0) {
    		printf("Error in function pthread_cond_wait\n");
    		exit(EXIT_FAILURE);
    	    }
        }

        for (idx = 0; idx < totThreads; idx++) {
            if (thread[idx].state == TS_TERMINATED) {
            	void *msg;
                s = pthread_join(thread[idx].tid, &msg);
                if (s != 0) {
    			printf("Error in function pthread_join\n");
    			exit(EXIT_FAILURE);
    		}

                thread[idx].state = TS_JOINED;
                numLive--;
                numUnjoined--;
                printf("Thread %d: %s\n", idx, (char *) msg);
            }
        }

        s = pthread_mutex_unlock(&threadMutex);
        if (s != 0) {
    		printf("Error in function pthread_mutex_unlock\n");
    		exit(EXIT_FAILURE);
    	}
    }

    exit(EXIT_SUCCESS);
}
