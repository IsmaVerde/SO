#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
 char *argv[]={"xclock","-background", "green", NULL};
  /* printf("Antes de execvp\n");
  if (execvp("xclock",argv)==-1) perror("execvp");
  printf("Despues\n");
*/
/* 
exec pri hace lo mismo que el anterior exec pero cambiando la prioridad

fork crea una replica de un proceso dado, si fork devuelve 0 es el proceso
hijo, si no, es el padre. En el shell el fork crea una copia del shell y tirar
palante, si lo cierras, vuelve al shell padre

el priority tiene que fijar la prioridad con getpriority

ejecutar procesos en segundo plano con "&"

comando kill -9 (%(numero de proceso) o pid del proceso) -> mata proceso
kill -STOP pid de proceso -> detiene el proceso
kill -CONT %nproceso -> continua proceso parado


*/ 
pid_t pid;
int i;
printf("Antes de empezar\n");
getc(stdin);
if((pid=fork())==0) {
   //codigo del proceso hijo
//segundo plano
   if (execvp("xclock",argv)==-1) perror("execvp");
    exit(0);
 /*  for (i=0; i<100; i++) 
        printf("%d\n",i);
   exit(0); */
}
if(pid==-1) perror("fork");
else
//codigo del proceso padre
printf("Proceso creado por el hijo pid= %d\n", pid);
waitpid(pid,NULL,0);
printf("El padre va a terminar\n");
//codigo comun a ambos, hijo y padre
}
