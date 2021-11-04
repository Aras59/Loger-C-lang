#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdatomic.h>
#include "library.h"


int main(){
    
    init();
    

    while(1)
    sleep(1);
    
    
    
   

    return EXIT_SUCCESS;
}