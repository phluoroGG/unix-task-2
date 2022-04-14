#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int avail = 0;

static void *
producer(void *arg)
{
    int cnt = atoi((char *) arg);

    for (int j = 0; j < cnt; j++) {
        sleep(1);

        int s = pthread_mutex_lock(&mtx);
        if (s != 0) {
    		printf("Error in function pthread_mutex_lock\n");
    		exit(EXIT_FAILURE);
    	}

        avail++;

        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
    		printf("Error in function pthread_mutex_unlock\n");
    		exit(EXIT_FAILURE);
    	}

        s = pthread_cond_signal(&cond);
        if (s != 0) {
    		printf("Error in function pthread_cond_signal\n");
    		exit(EXIT_FAILURE);
    	}
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    time_t t = time(NULL);

    int totRequired = 0;
    for (int j = 1; j < argc; j++) {
        totRequired += atoi(argv[j]);

        pthread_t tid;
        int s = pthread_create(&tid, NULL, producer, argv[j]);
        if (s != 0) {
    		printf("Error in function pthread_create\n");
    		exit(EXIT_FAILURE);
    	}
    }
    
    int numConsumed = 0;
    bool done = false;

    for (;;) {
        int s = pthread_mutex_lock(&mtx);
        if (s != 0) {
    		printf("Error in function pthread_mutex_lock\n");
    		exit(EXIT_FAILURE);
    	}

        while (avail == 0) {
            s = pthread_cond_wait(&cond, &mtx);
            if (s != 0) {
    		printf("Error in function pthread_cond_wait\n");
    		exit(EXIT_FAILURE);
    	    }
        }
        
        while (avail > 0) {

            numConsumed ++;
            avail--;
            printf("T=%ld: numConsumed=%d\n", (long) (time(NULL) - t),
                    numConsumed);

            done = numConsumed >= totRequired;
        }

        s = pthread_mutex_unlock(&mtx);
        if (s != 0) {
    		printf("Error in function pthread_mutex_unlock\n");
    		exit(EXIT_FAILURE);
    	}

        if (done)
            break;
    }

    exit(EXIT_SUCCESS);
}
