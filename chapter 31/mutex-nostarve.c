#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "common_threads.h"

typedef struct __ns_mutex_t {
    sem_t lock;    
    sem_t next;       
    int next_count;  
    sem_t roomEmpty;  
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    sem_init(&m->lock, 0, 1);
    sem_init(&m->next, 0, 0);
    m->next_count = 0;
    sem_init(&m->roomEmpty, 0, 1);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_wait(&m->lock);
    sem_wait(&m->roomEmpty);
    if (m->next_count > 0)
        sem_post(&m->next);
    else
        sem_post(&m->lock);
}

void ns_mutex_release(ns_mutex_t *m) {
    sem_wait(&m->lock);
    m->next_count--;
    if (m->next_count > 0)
        sem_post(&m->next);
    else
        sem_post(&m->roomEmpty);
    sem_post(&m->lock);
}

void *worker(void *arg) {
    ns_mutex_t *m = (ns_mutex_t *)arg;

    ns_mutex_acquire(m);
    // Critical section
    printf("Thread in critical section\n");
    sleep(1); 
    ns_mutex_release(m);

    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");

    ns_mutex_t m;
    ns_mutex_init(&m);

    pthread_t p1, p2, p3;
    Pthread_create(&p1, NULL, worker, &m);
    Pthread_create(&p2, NULL, worker, &m);
    Pthread_create(&p3, NULL, worker, &m);

    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    Pthread_join(p3, NULL);

    printf("parent: end\n");
    return 0;
}
