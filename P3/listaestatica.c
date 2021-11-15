#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "listaestatica.h"


struct SEN {
      char *nombre;
      int senal;
};

static struct SEN sigstrnum[]={
       {"HUP", SIGHUP},
       {"INT", SIGINT},
       {"QUIT", SIGQUIT},
       {"ILL", SIGILL},
       {"TRAP", SIGTRAP},
       {"ABRT", SIGABRT},
       {"IOT", SIGIOT},
       {"BUS", SIGBUS},
       {"FPE", SIGFPE},
       {"KILL", SIGKILL},
       {"USR1", SIGUSR1},
       {"SEGV", SIGSEGV},
       {"USR2", SIGUSR2},
       {"PIPE", SIGPIPE},
       {"ALRM", SIGALRM},
       {"TERM", SIGTERM},
       {"CHLD", SIGCHLD},
       {"CONT", SIGCONT},
       {"STOP", SIGSTOP},
       {"TSTP", SIGTSTP},
       {"TTIN", SIGTTIN},
       {"TTOU", SIGTTOU},
       {"URG", SIGURG},
       {"XCPU", SIGXCPU},
       {"XFSZ", SIGXFSZ},
       {"VTALRM", SIGVTALRM},
       {"PROF", SIGPROF},
       {"WINCH", SIGWINCH},
       {"IO", SIGIO},
       {"SYS", SIGSYS},
     
#ifdef SIGPOLL 
       {"POLL", SIGPOLL},
#endif

#ifdef SIGPWR
       {"PWR", SIGPWR},
#endif

#ifdef SIGEMT
        {"EMT", SIGEMT},
#endif

#ifdef SIGINFO
        {"INFO", SIGINFO},
#endif

#ifdef SIGSTKFLT
        {"STKFLT", SIGSTKFLT},
#endif

#ifdef SIGCLD
        {"CLD", SIGCLD},
#endif

#ifdef SIGLOST
        {"LOST", SIGLOST},
#endif

#ifdef SIGCANCEL
        {"CANCEL", SIGCANCEL},
#endif

#ifdef SIGTHAW
        {"THAW", SIGTHAW},
#endif

#ifdef SIGFREEZE
        {"FREEZE", SIGFREEZE},
#endif

#ifdef SIGLWP
        {"LWP", SIGLWP},
#endif

#ifdef SIGWAITING
        {"WAITING", SIGWAITING},
#endif
     {NULL,-1},
     };  

int Senal(char * sen){
      int i;

      for (i=0; sigstrnum[i].nombre!=NULL; i++)
      	 if (!strcmp(sen, sigstrnum[i].nombre))
      	 	return sigstrnum[i].senal;
      return -1;
}

char *NombreSenal(int sen){ 
       int i;
       for (i=0; sigstrnum[i].nombre!=NULL; i++)
       	 if (sen==sigstrnum[i].senal)
            return sigstrnum[i].nombre;
       return ("SIGUNKNOWN");
}


 LISTAE crearLista (){
    LISTAE aux = (LISTAE) malloc (sizeof(struct nodoest));
    aux->fin=-1;
    return aux;
 }


int esListaVacia (LISTAE l){
   return (l->fin == -1);
}


int insertarElemento (LISTAE l, DatoProcesos d){
  

    if (l->fin < MAX){
        l->fin += 1;
    	l->arr[l->fin].pid = d.pid;
    	l->arr[l->fin].prio = d.prio;
    	l->arr[l->fin].prog = d.prog;
    
    	l->arr[l->fin].time = (char *) (malloc (strlen(d.time)+1));
    	strcpy(l->arr[l->fin].time, d.time);
    	strcpy(l->arr[l->fin].estado, d.estado);
    	l->arr[l->fin].sig = d.sig;
  
    	return 1;
    }
     return 0;
}

DatoProcesos obtenerElemento (LISTAE l, int pos){
       return l->arr[pos];

}


int buscarElemento (LISTAE l, pid_t pid){
    int i;

    for (i=0; i<l->fin+1; i++){
        if (l->arr[i].pid == pid)
        	return i;
    }
  
    return -1;
}

void eliminarElemento (LISTAE l, int pos){
     int i =pos;

    free (l->arr[i].prog);
    free (l->arr[i].time);

     for (i=pos; i<l->fin; i++)																														
       l->arr[i] = l->arr[i+1];
     
     l->fin += -1;

       	  
}



void opcionesEstado (DatoProcesos *dato, int estado){

	 if (WIFEXITED(estado)){
           strcpy(dato->estado, "TERMINADO");
           dato->sig = WEXITSTATUS(estado);
       }
       else if (WIFSIGNALED(estado)){
              strcpy(dato->estado, "SENALADO");
              strcpy(dato->sen, NombreSenal(WTERMSIG(estado)));          
            }
            else if (WIFSTOPPED(estado)){
                strcpy(dato->estado, "PARADO");
                strcpy(dato->sen, NombreSenal(WSTOPSIG(estado)));  
            }
}



void actualizaElemento (DatoProcesos * dato){
	int estado;
	pid_t pid_hijo = 0;
  


    if ((pid_hijo = waitpid (dato->pid, &estado, WNOHANG | WUNTRACED | WCONTINUED)) == -1){
   	       
   	       return;
    }

     dato->prio = getpriority(PRIO_PROCESS, dato->pid);


    if (dato->pid == pid_hijo){

  	  

       opcionesEstado(dato, estado);
    }
    

}


void mostrarElemento (DatoProcesos dato){
   
    if (!strncmp(dato.estado, "P", 1) || !strncmp(dato.estado, "S", 1) )
       printf("%d p=%d %s %s (%s) %s\n",dato.pid, dato.prio, dato.time, dato.estado, dato.sen, dato.prog );
    else printf("%d p=%d %s %s (%03d) %s\n", dato.pid, dato.prio, dato.time, dato.estado, dato.sig, dato.prog );

}


void mostrarElementos (LISTAE l){
   int i=0;
    while (i< l->fin+1){
       actualizaElemento(&l->arr[i]);
       mostrarElemento(l->arr[i]);
       i++;
    }
}



int eliminarElementos (LISTAE l, char condicion){
    int i, res=0;

    for (i=0; i<l->fin+1; i++){
    	actualizaElemento(&l->arr[i]);
        if (condicion == 'T') {
          if (!strncmp(l->arr[i].estado, "T", 1)){
          	 eliminarElemento(l, i);
          	 res =1;
          	 i += -1;
          }
        }
        else if (condicion == 'S')
        	    if (!strncmp(l->arr[i].estado, "S", 1)){
          	             eliminarElemento(l, i);
          	             res =1;
          	             i += -1;
                 }
      
 
    }
    return res;

}

void liberarlista(LISTAE l){
   
 
    while (!esListaVacia(l)){
    	eliminarElemento(l, l->fin);
    }

    free(l);


}


