#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdatomic.h>
#include "library.h"

atomic_int save;
atomic_int loglevel;
volatile sig_atomic_t state = 0;
pthread_mutex_t lock;


void handler_save_file(int signo,siginfo_t* info,void* other){
    state = 1;
}


void handler_chagne_signal(int signo,siginfo_t* info,void* other){
    if(signo == SIGRTMIN+1){
        if(atomic_load(&save)==1){
            atomic_store(&save,0);
        }else{
            atomic_store(&save,1);
        }
    }
    if(signo == SIGRTMIN+2){
        if(atomic_load(&loglevel)==1){
            atomic_store(&loglevel,2);
        }
        else if(atomic_load(&loglevel)==2){
            atomic_store(&loglevel,3);
        }
        else if(atomic_load(&loglevel)==3){
            atomic_store(&loglevel,1);
        }
    }
}


void * func (void * arg){
    FILE *fp;
    pid_t pid;
    pid = syscall(SYS_gettid);
    
    while(1){
        if(state == 1){
            pthread_mutex_lock(&lock);
            time_t timer;
            time(&timer);
            fp = fopen(ctime(&timer),"a");
			if(fp == NULL){
				saveLogToFile("Nie udało się odczytać pliku! Plik nie istanieje!",1);
			}else{
				fprintf(fp,"Thread: %lu\n",pthread_self());
				fprintf(fp,"LogLevel: %d\n",atomic_load(&loglevel));
				fprintf(fp,"SaveLevel: %d\n",atomic_load(&save));
				fprintf(fp,"Pid: %d\n",pid);
				fprintf(fp,"-----------------------------------------------------------------------------------\n");
				state = 0;
				fclose(fp);
			}
            
            pthread_mutex_unlock(&lock);
        }
        sleep(1);
    }
    return NULL;
}

void saveLogToFile(char * log,int level){
    if(atomic_load(&save) == 1 ){
        pthread_mutex_lock(&lock);
		if(level <= atomic_load(&loglevel)){
			FILE *fp = fopen("Logs.txt","a");
			if(fp == NULL){
			   return ;
			}
			fprintf(fp,"%s\n",log);
			fclose(fp);
        }
        pthread_mutex_unlock(&lock);
    }
    
    return ;
    
}

void init(){

    pthread_mutex_init(&lock,NULL);
    pid_t pid;
    struct sigaction act;
    sigset_t set;
    atomic_store(&save,0);
    atomic_store(&loglevel,1);

    pid = syscall(SYS_gettid);


    act.sa_sigaction = handler_save_file;
    act.sa_mask = set;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGRTMIN,&act,NULL);

    act.sa_sigaction = handler_chagne_signal;
    act.sa_mask = set;
    act.sa_flags = SA_SIGINFO;


    for(int i=1;i<3;i++){
        sigaction(SIGRTMIN+i,&act,NULL);
    }

    pthread_t tid;
    pthread_create(&tid,NULL,func,NULL);

    
    
}





