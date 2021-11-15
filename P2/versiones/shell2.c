#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <limits.h>
#include <libgen.h>
#include "lista.h"
#define MAXLINEA 2048
#define MAXTROZOS 50

/* 	Practica 2. Sistemas Operativos.
	Ismael Verde: ismael.verdec
	Sergio Pinheiro: sergio.pineiro*/


char linea[MAXLINEA];
char *trozos[MAXTROZOS];
int ntrozos;
int i;
extern int errno;
LISTA L;

void opciones(int argc, char *argv[], char *letras, char *op[]){

   int i, j;
   for (i=1; i<argc; i++)
     if(argv[i][0]=='-' && strlen(argv[i])==2)
       for(j=0;letras[j]!='\0'; j++)
         if(letras[j]==argv[i][1])
           op[j][0]=1;
}


int TrocearCadena(char * cadena, char * trozos[]){
	 int i=1;
	 if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
		return 0;
	 while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	 return i;
}

char TipoFichero (mode_t m){
switch (m&S_IFMT){
   case S_IFSOCK: return 's'; /*socket */
   case S_IFLNK: return 'l'; /*symbolic link*/
   case S_IFREG: return '-'; /* fichero normal*/
   case S_IFBLK: return 'b'; /*block device*/
   case S_IFDIR: return 'd'; /*directorio */
   case S_IFCHR: return 'c'; /*char device*/
   case S_IFIFO: return 'p'; /*pipe*/
   default: return '?'; /*desconocido, no deberia aparecer*/
}

}



	 
char * ConvierteModo2 (mode_t m){
static char permisos[12];
strcpy (permisos,"---------- ");
permisos[0]=TipoFichero(m);
if (m&S_IRUSR) permisos[1]='r';
if (m&S_IWUSR) permisos[2]='w';
if (m&S_IXUSR) permisos[3]='x';
if (m&S_IRGRP) permisos[4]='r';
if (m&S_IWGRP) permisos[5]='w';
if (m&S_IXGRP) permisos[6]='x';
if (m&S_IROTH) permisos[7]='r';
if (m&S_IWOTH) permisos[8]='w';
if (m&S_IXOTH) permisos[9]='x';
if (m&S_ISUID) permisos[3]='s';
if (m&S_ISGID) permisos[6]='s';
if (m&S_ISVTX) permisos[9]='t';
return (permisos);

}


struct tipocmd{
    char *nombre;
    void (*funcion)(int argc, char *argv[]);
};



void cmdAutores(int argc, char *argv[]) {
     char op_l=0; 
     char op_n=0;
     char *op[2]={&op_l,&op_n};
     opciones(argc, argv, "ln", op);  


    if (!(op_l || op_n)) {printf("Ismael Verde: (ismael.verdec)\n"); printf ("Sergio Pineiro: (sergio.pineiro)\n");}
       
    if (op_l) {printf("ismael.verdec\n"); printf("sergio.pineiro\n");}
               
    if (op_n) {printf("Ismael Verde\n"); printf("Sergio Pineiro\n");}
                        
}

void cmdExit (int argc, char *argv[]) {
    liberarLista(L);
     exit(0);
}

void cmdPid (int argc, char *argv[]){
   char op_p=0;
   char *op[1]={&op_p};
   opciones(argc, argv, "p", op);
   if (op_p) printf("Pid de padre del shell: %d\n", getppid());
   else printf("Pid de shell: %d\n",getpid());	
}

void cmdCdir (int argc, char *argv[]){
        char s[50];
	char *direct = argv [1];
	if (argc == 1) {
		printf("%s\n", getcwd(s,50)); 
	}
	else if (ntrozos == 2) { 
                    if (chdir(direct)==0){
			       chdir(direct);}
		    else 	
			perror("Imposible cambiar de directorio");
	}       
	
}

void cmdFecha(int argc, char *argv[]){
        time_t t = time(0);
        struct tm *tlocal = localtime(&t);
        char output[128];
	
        strftime(output,128,"%a %b %d %H:%M:%S %Y",tlocal);
	printf("%s\n", output);
	
}

void cmdHora(int argc, char *argv[]){
        time_t t = time(0);
	struct tm *tlocal = localtime(&t);
	char output[128];
	
        strftime(output,128," %H:%M:%S",tlocal);
	printf("%s\n", output);
}

void cmdHist(int argc, char *argv[]){
     char op_c=0;
     char *op[1]={&op_c};   
     opciones(argc, argv, "c", op); 
       if (op_c) {liberarLista(L); printf("Historic borrado\n");}
       else imprimirLista(L);
}


void cmdInfo(int argc, char *argv[]){
   int i, error=0, x=0;
   struct stat fd;
   char output [80];
   mode_t m;
   char s [PATH_MAX];
   size_t size =0;
   char * file ;
   char copy [PATH_MAX] ;
   char cadena [40];
   
 
   for (i=1; i<argc && argc>0; i++){
         file = NULL; 
         if (lstat (argv[i], &fd) ==-1){
                    perror("cannot access");
         }
         else {
                m=fd.st_mode;
                if ( TipoFichero(m)=='l'){
                     x=1;
                     if ((size=fd.st_size) == 0)
                           size=PATH_MAX;
                     file = (char *) malloc(size);
                     if( (int) readlink(argv[i], file, size)==-1 || file == NULL){
                             perror ("Error link"); error=1; } 
                     else  {
                            strcpy (cadena, argv[i]);
                            strcpy (copy, strcat( strcat (cadena, " -> "), file));
                            strcpy(s, copy);
                           }   
                }
                else {strcpy(s, argv[i]);}
                
                if (!error){
                        strftime(output, 80, "%b %2d %2H:%2M", localtime(&fd.st_mtime));
                        printf("%8ld %10s %2ld %2s %2s %6ld %2s %2s\n",fd.st_ino, ConvierteModo2(m), fd.st_nlink, getpwuid(fd.st_uid)->pw_name, getgrgid(fd.st_gid)->gr_name, fd.st_size, output, s);
                } 
                if (x){
                   free(file);
                  
                 }     
        }
    }

}

void cmdListar (int argc, char *argv []){
  char op_l = 0, op_v=0, op_r=0;
  char *op [3] = {&op_l, &op_v, &op_r};
  opciones (argc, argv, "lvr", op);
  char directorio [PATH_MAX];
  char copia [PATH_MAX];
  char * actual = NULL;
  DIR * open;
  struct stat sd;
  struct dirent * d;
  int i, j, comp;
  char * name;
  char * toInfo[2];
  char * argr [5];
  char * n = malloc(sizeof(char));
  


  strcpy(n, "");
  comp=  op_l + op_r + op_v +1;
  for (i=comp; i<= argc; i++){
      if (i==argc){
             if (i!=comp) {break;}
             strcpy(directorio, ".");
      }
      else {
          strcpy(directorio, argv[i]);
      }
  
  printf("********* %s\n",directorio);

  if ((open = opendir (directorio)) != NULL){

       for (d=readdir(open); d!=NULL; d = readdir(open)){
            name=d->d_name;
            strcpy(copia, directorio);
            strcat(copia, "/");
            strcat(copia, name);
            actual = getcwd (actual, 100);
           
            if (lstat(copia, &sd)!=-1){
               if (chdir(directorio)!=-1){
                  if ( !(op_v && (strncmp(name, ".", 1)==0))){
                      if (op_l) {
                        toInfo[1]=name;
                        cmdInfo(2, toInfo);
                      }
                      else printf ("%s %ld\n", name, sd.st_size);
                   }
                   
               }
               else perror ("Error al listar ");

            }
            else perror ("Error al listar");
           
           if (chdir(actual)==-1){ 
               perror ("Error al listar"); break;}
           
            
       }


   }
   else perror ("Error al listar ");

   closedir (open);

   if (op_r){
    
         if ((open = opendir (directorio)) != NULL){

            for (d=readdir(open); d!=NULL; d = readdir(open)){
              argr [1]=n; argr [2]=n; argr [3]=n; argr [4]=n;
              name=d->d_name;
              if (!(strncmp(name, ".", 1)==0)){
                    strcpy(copia, directorio);
                    strcat(copia, "/");
                    strcat(copia, name);
                    if (lstat(copia, &sd)!=-1){
                         if (TipoFichero(sd.st_mode)=='d'){
                              for (j=1; j<comp; j++){
                                argr[j] = argv[j];
                              }
                              argr[j]=copia;
                              cmdListar(op_v+op_l+3, argr);
                          }
                    }
              }
            }


         }
         else perror ("Error al listar");
         closedir(open);
         free(n);

     }
    
   

  }

}

void cmdBorrar (int argc, char *argv[]){
     char op_r=0;
     char *op [1] = {&op_r};
     opciones (argc, argv, "r", op);
     char *copy = malloc (sizeof(char));
     char directorio [PATH_MAX] ;
     struct stat sd;
     struct dirent * d;
     DIR * open;
     char * name;
     char * buff[3];

     strcpy(copy, ".");
     if ((argc==1) || (op_r && argc==2)){
        argv[1]=copy;
        cmdListar(2, argv);
     } 
     else if (!op_r){
                 if (lstat (argv[1], &sd) !=-1){
                    if (TipoFichero(sd.st_mode)== 'd'){
                        if (rmdir(argv[1])==-1)
                              perror ("Imposible borrar");
                    }                
                    else if (unlink(argv[1])==-1)
                               perror ("Imposible borrar");

                 } 
                 else perror("Imposible borrar");
           } 
           else{
              
                 if ((open= opendir(argv[2]))!=NULL){
                    
                     for (d=readdir(open); d!=NULL; d = readdir(open)){ 
                         name = d->d_name;
                         if (!(strncmp(name, ".", 1)==0)){
                            buff[1]=argv[1];
                            strcpy(directorio, argv[2]);
                            strcat(directorio, "/");
                            strcat(directorio, name);
                            buff[2]=directorio;
                            if (lstat (directorio, &sd) !=-1){
                               if (TipoFichero(sd.st_mode)== 'd'){
                                   cmdBorrar(3, buff);
                               }
                               else if (unlink(directorio)== -1)
                                      perror("Imposible borrar f");
                            }   
                            else perror ("Imposible abrir");
                         }
                     }
                     closedir(open);
                     if (rmdir(argv[2])==-1)
                              perror ("Imposible borrar");


                 }
                 else perror ("Imposible borrar");
           }            


    
     free(copy);
}

void cmdCrear(int argc, char *argv[]){
     char op_d=0;
     char *op[1]={&op_d};
     int i;
     opciones(argc, argv, "d", op);
     char *copy = malloc (sizeof(char));
     
     strcpy(copy, ".");
    if (!(argc==1 || (argc==2 && op_d))){
        if (op_d) {
                   if ((i= mkdir(argv[2],  ACCESSPERMS))== -1)
                       perror("Imposible crear directorio" ); 
        } 
        else
                   if ((i=open(argv[1], O_CREAT | O_EXCL, S_IRWXU | S_IRGRP |S_IROTH)) == -1)
                       perror("Imposible crear fichero" ); 
    }
    else {
          argv[1]=copy;
          cmdListar (2, argv);
    }
    free(copy);
}


void cmdAsignar(int argc, char *argv[]){
  char op_malloc = 0, op_mmap=0, op_crearshared=0, op_shared=0;
  char *op [4] = {&op_malloc, &op_mmap, &op_crearshared, &op_shared};


}

void Cmd_AsignarMmap (char *arg[]){
    char *perm;
    void *p;
    int protection=0;
    if (arg[0]==NULL){ /*Listar Direcciones de Memoria mmap;*/ 
        return;
    }
    if ((perm=arg[1])!=NULL && strlen(perm)<4) {
        if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
    }
    if ((p=MmapFichero(arg[0],protection))==NULL)
        perror ("Imposible mapear fichero");
    else
        printf ("fichero %s mapeado en %p\n", arg[0], p);
}


void cmdDesasignar(int argc, char *argv[]){
  char op_malloc = 0, op_mmap=0, op_shared=0, op_addr=0;
  char *op [4] = {&op_malloc, &op_mmap, &op_shared, &op_addr};
}

void cmdBorrarkey(char *args[]){
    key_t clave;
    int id;
    char *key=args[0];
    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("   rmkey  clave_valida\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
}

void cmdMem(int argc, char *argv[]){
  char op_malloc = 0, op_shared=0, op_mmap=0, op_all=0;
  char *op [4] = {&op_malloc, &op_shared, &op_mmap, &op_all};
}

void cmdVolcar(int argc, char *argv[]){

}

void cmdLlenar(int argc, char *argv[]){

}

void cmdRecursiva(int argc, char *argv[]){

}

void cmdRfich(int argc, char *argv[]){

}

void cmdWfich(int argc, char *argv[]){

}

void * ObtenerMemoriaShmget (key_t clave, size_t tam){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam)  /*si tam no es 0 la crea en modo exclusivo */
        flags=flags | IPC_CREAT | IPC_EXCL;
    /*si tam es 0 intenta acceder a una ya creada*/
    if (clave==IPC_PRIVATE)   /*no nos vale*/{
        errno=EINVAL;
        return NULL;
    }
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;   /*si se ha creado y no se puede mapear*/
        if (tam)     /*se borra */
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);/* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
    return (p);
}

void * MmapFichero (char * fichero, int protection){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;void *p;
    if (protection&PROT_WRITE)
        modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
        return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
        return NULL; /*Guardar Direccion de Mmap (p, s.st_size,fichero,df......);*/
    return p;
}

struct tipocmd tablacmd[] = 
{
    {"autores", cmdAutores},
    {"pid", cmdPid},
    {"cdir", cmdCdir},
    {"fecha", cmdFecha},
    {"hora", cmdHora},
    {"hist", cmdHist},
    {"crear", cmdCrear},
    {"info", cmdInfo},
    {"listar", cmdListar},
    {"borrar", cmdBorrar},
    {"asignar", cmdAsignar},
    {"desasignar", cmdDesasignar},
    {"borrarkey", cmdBorrarkey},
    {"mem", cmdMem},
    {"volcar", cmdVolcar},
    {"llenar", cmdLlenar},
    {"recursiva", cmdRecursiva},
    {"rfich", cmdRfich},
    {"wfich", cmdWfich},
    {"exit", cmdExit},
    {"fin", cmdExit},
    {"end", cmdExit},
    {NULL,NULL}
};

  
 
int main() {
     errno=0;
     L= listaVacia();
     while(1) {
      
      printf("-> ");
      
      if (fgets(linea, MAXLINEA, stdin)==NULL)
               {printf("Error fgets\n"); exit(0);}
      
      InsertElement(L,linea);
      ntrozos=TrocearCadena(linea, trozos); 
      
      i=0;
      while((tablacmd[i].nombre!=NULL) & (ntrozos!=0)){
          if (strcmp(trozos[0],tablacmd[i].nombre)==0){
               tablacmd[i].funcion (ntrozos, trozos);
               break; }
          i++;
          if (tablacmd[i].nombre==NULL)
            printf("%s no encontrado\n", trozos[0] );
      }
        
   }
    
}

