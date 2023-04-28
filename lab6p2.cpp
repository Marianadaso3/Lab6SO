#include <unistd.h>
#include <fstream>
#include <sys/syscall.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;

// play with the amounts
int iterations_num = 4;
int THREADPOOL = 4;

ofstream file;

class Monitor{
    public:
        int available_resources = 64;
        int MAX_RESOURCES = 16;
        pthread_cond_t MonitorCondition;
        pthread_mutex_t  MonitorLock;
        
        void mutex_init() { pthread_mutex_init(&MonitorLock, NULL); }
        void mutex_end() { pthread_mutex_destroy(&MonitorLock); }
        
        void* increase_count();
        void* decrease_count();
        void* resource_manager();
};

void* Monitor::decrease_count() {
    
    file<<"Iniciando decrease_count"<<"\n";
    
    while (available_resources < MAX_RESOURCES) {
        pthread_cond_wait(&MonitorCondition, &MonitorLock);
    }

    file<<"Recursos suficientes disponibles, consumiendo..."<<"\n";
    available_resources -= MAX_RESOURCES;

    sleep(1);

    file<<"Terminando decrease_count"<<"\n";

    file<<"Mutex liberado"<<"\n";
    pthread_mutex_unlock(&MonitorLock);
}

void* Monitor::increase_count() {
    
    file<<"Iniciando increase_count"<<"\n";
    pthread_mutex_lock(&MonitorLock);

    if (available_resources <= MAX_RESOURCES) {
        file<<"Recursos disponibles! Recursos actuales: "<<available_resources<<"\n";
    }

    pthread_cond_signal(&MonitorCondition);
    available_resources+=MAX_RESOURCES;

    file<<"Terminando increase_count"<<"\n";
    pthread_mutex_unlock(&MonitorLock);
}

void* Monitor::resource_manager() {
    file<<"Iniciando thread "<<syscall(SYS_gettid)<<"\n";

    int i;
    for (i = 0; i < iterations_num; i++) {
        file<<"Iniciando iteracion "<< i + 1<<"\n"; 
        file<<"Se comsumiran "<<MAX_RESOURCES<<" recursos"<<"\n"; 

        pthread_mutex_lock(&MonitorLock);

        file<<syscall(SYS_gettid)<<" - (!) Recurso tomado"<<"\n";
        file<<"Buenos dias! Recurso usado"<<syscall(SYS_gettid)<<"\n";

        file<<"Mutex adquirido, entrando al monitor"<<"\n";
        decrease_count();
        increase_count();

        file<<"Mutex liberado"<<"\n";
        pthread_mutex_unlock(&MonitorLock);
    }
   
