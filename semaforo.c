/*
Universidad del Valle de Guatemala
LABORATORIO 6
Autor: Mariana David 

Referencia:
https://stackoverflow.com/questions/11160763/create-a-dynamic-number-of-threads
https://www.delftstack.com/es/howto/c/pthread-get-thread-id-in-c/
https://www.geeksforgeeks.org/use-posix-semaphores-c/
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#define NUM_THREADS 5
#define NUM_ITERATION 2
int RESOURCES = 3;

sem_t semaphore;

struct resource {
    FILE* data;
};

void *worker(void *arg){
    int tid = gettid();
    int j;

    // OBTENEMOS LOS DATOS DEL FILE ENVIADO POR LOS PARAMETROS
    FILE *data= (FILE*)arg;

    printf("Iniciando hilo %d...\n", tid);
    printf("Semaforo abierto correctamente en hilo %d.\n", tid);

    // IMPRIMOS EL RESULTADO EN EL FILE
    fprintf(data, "Iniciando hilo %d...\n", tid);
    fprintf(data, "Esperando hilo...\n");
    fprintf(data, "Semaforo abierto correctamente en hilo %d.\n", tid);

    for(j=0; j<NUM_ITERATION; j++){
        printf("Iniciando iteracion %d en hilo %d...\n", j, tid);
        fprintf(data, "Iniciando iteracion %d en hilo %d...\n", j, tid);
        sem_wait(&semaphore);

        RESOURCES--;
        printf("Recurso tomado por hilo %d.\n", tid);
        fprintf(data, "(!) Recurso tomado por hilo %d.\n", tid);
        sleep(2);
        printf("Recurso usado por hilo %d.\n", tid);
        fprintf(data, "Buenos dias! Recurso usado por hilo %d.\n", tid);
        RESOURCES++;
        sem_post(&semaphore);
        printf("Recurso devuelto por hilo %d.\n", tid);
        fprintf(data, "Recurso devuelto por hilo %d :)\n", tid);
    }

    fprintf(data, "Semaforo cerrado para hilo %d.\n", tid);
    fprintf(data, "Terminando hilo %d...\n", tid);
    pthread_exit(NULL);
}

int main(){
    // CREATE THE FILE
    FILE* file = fopen("semaphore_log.txt", "wt");
    if(file == NULL){
        printf("Error al crear archivo.\n");
        return 1;
    }
    // struct resource *semaphore_log = (struct resource *)malloc(sizeof(struct resourcer));

    sem_init(&semaphore, 0 , 1);
    int i;
    printf("Iniciando programa...\n");
    printf("Creando pool de hilos...\n");

    // ESCRIBIMOS EN EL LOG
    fprintf(file, "Iniciando programa...\n");
    fprintf(file, "Creando hilos...\n");

    pthread_t pool_threads[NUM_THREADS];

    for(i=0; i<NUM_THREADS; i++){
        pthread_create(&pool_threads[i], NULL, &worker, (void *)file);
    }

    sleep(0.5);
    printf("Esperando hilos...\n");

    for(i=0; i<NUM_THREADS; i++){
        pthread_join(pool_threads[i], NULL);
    }

    sleep(2);
    printf("Valor final de RESOURCES: %d\n", RESOURCES);
    sem_destroy(&semaphore);

    fclose(file);
    return 0;
}
