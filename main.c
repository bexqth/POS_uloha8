#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct buffer_t {
    int *array;
    int capacity;
    int index;
    int pocetParnych;
    int pocetNeparnych;
} buffer_t;

void buffer_init(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int)*capacity);
    buff->index = 0;
    buff->capacity = capacity;
    buff->pocetParnych = 0;
    buff->pocetNeparnych = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buff, int data) {
    if(buff->index < buff->capacity) {
        buff->array[buff->index++] = data;
        return true;
    }
    return false;
}

int buffer_pull(buffer_t *buff) {
    if(buff->index > 0) {
        return buff->array[--buff->index];
    }
    return -1;
}

typedef struct threat_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t lala;
    pthread_cond_t tinkyWinky;
    buffer_t buff;
} threat_data_t;

void thread_data_init(threat_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->lala, NULL);
    pthread_cond_init(&data->tinkyWinky, NULL);
    buffer_init(&data->buff, capacity);
}

void thread_data_destroy(threat_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->lala);
    pthread_cond_destroy(&data->tinkyWinky);
    buffer_destroy(&data->buff);
}

int vygenerujCislo() {
    return rand();
}

void* tinkyWinky_fun(void* data) {
    threat_data_t* data_t = (threat_data_t*) data;

    for(int i = 0; i < 1000; i++) {
    //while(true) {
        pthread_mutex_lock(&data_t->mutex);
        int cislo = vygenerujCislo();
        while(!buffer_push(&data_t->buff, cislo)) {
            pthread_cond_wait(&data_t->tinkyWinky, &data_t->mutex);
        }

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->lala);
    }
}

void vyhodnotCislo(buffer_t *buff, int cislo) {
    if(cislo % 2 == 0 ) {
        buff->pocetParnych++;
        printf("cislo je parne -> %d \n", cislo);
    } else {
        buff->pocetNeparnych++;
        printf("cislo je neparne -> %d \n", cislo);
    }
}

void* lala_fun(void* data) {
    threat_data_t* data_t = (threat_data_t*) data;

    for(int i = 0; i < 1000; i++) {
    //while(true) {
        pthread_mutex_lock(&data_t->mutex);

        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->lala, &data_t->mutex);
        }

        int cislo = buffer_pull(&data_t->buff);
        vyhodnotCislo(&data_t->buff, cislo);
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->tinkyWinky);
    }
}


int main() {

    threat_data_t data;
    thread_data_init(&data,8);

    pthread_t lala, tinkyWinky;
    pthread_create(&tinkyWinky, NULL, tinkyWinky_fun, &data);
    pthread_create(&lala, NULL, lala_fun, &data);

    pthread_join(tinkyWinky, NULL);
    pthread_join(lala, NULL);


    thread_data_destroy(&data);

    if (data.buff.pocetParnych >= 600) {
        printf("60 percent nahodne generovanych cisiel je parnych \n");
    } else {
        printf("60 percent nahodne generovanych cisiel nie je parnych \n");
    }

    return 0;
}
