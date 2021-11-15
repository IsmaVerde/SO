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
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "lista.h"
#include "listap2.h"
#include "listaestatica.h"
#define MAXLINEA 2048
#define MAXTROZOS 50
#define TAMANO 2048
#define LEERCOMPLETO ((ssize_t) -1)

/* 	Practica 3. Sistemas Operativos.
	Ismael Verde: ismael.verdec
	Sergio Pinheiro: sergio.pineiro*/


char linea[MAXLINEA];
char *trozos[MAXTROZOS];
int ntrozos;
int i;
extern int errno;
LISTA L;
LISTAp2 LP2;
LISTAE LP; 


void opciones(int argc, char *argv[], char *letras, char *op[]){

   int i, j;
   for (i=1; i<argc; i++)
     if(argv[i][0]=='-' && strlen(argv[i])==2)
       for(j=0;letras[j]!='\0'; j++)
         if(letras[j]==argv[i][1])
           op[j][0]=1;
}

void oplargas(int argc, char *argv[], char *palabras[], char *op[], int size){
    int i;

    if (argc >=2)
       for(i=0; i<size; i++)
           if (strcmp(argv[1], palabras[i])==0)
               op[i][0]=1;
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
    liberarLista(&L);
    liberarListaP2(&LP2);
    liberarlista(LP);
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

void Fecha (char * output, int tam){
     time_t t = time(0);
     struct tm *tlocal = localtime(&t);
     strftime(output,tam,"%a %b %d %H:%M:%S %Y",tlocal);
}




void cmdFecha(int argc, char *argv[]){
        char output[128];
	      Fecha(output, 128);
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
       if (op_c) {borrarElementos(L); printf("Historic borrado\n");}
       else imprimirLista(L);
}


void cmdInfo(int argc, char *argv[]){
   int i, error=0;
   struct stat fd;
   char output [80];
   mode_t m;
   char s [PATH_MAX];
   size_t size =0;
   char file [PATH_MAX];
   char copy [PATH_MAX] ;
   char cadena [40];
   
 
   for (i=1; i<argc && argc>0; i++){
         if (lstat (argv[i], &fd) ==-1){
                    perror("cannot access");
         }
         else {
                m=fd.st_mode;
                if ( TipoFichero(m)=='l'){
      
                     if ((size=fd.st_size) == 0)
                           size=PATH_MAX;
  
                     if( (int) readlink(argv[i], file, size)==-1){
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


void GuardarDireccion(void * p, size_t size, char *n , char *f, int a){
   Datop2 d;
   char output[128];
   Fecha(output, 128);
   d.p=p;
   d.size=size;
   d.n=n;
   d.f=f;
   d.a=a;
   d.fecha=output;
   
   InsertElementP2(LP2, d);

}


void * MmapFichero (char * fichero, int protection, char * mp){
     int df, map=MAP_PRIVATE, modo=O_RDONLY;
     struct stat s;
     void *p;
     
     if (protection&PROT_WRITE)  modo=O_RDWR;

     if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
         return NULL;

     if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
         return NULL;

     GuardarDireccion(p, s.st_size, mp, fichero, df);

      return p;
}


 void Cmd_AsignarMmap (char *arg[], char * mp){
     char *perm;
     void *p;
     int protection=0;

     if (arg[0]==NULL){
         imprimirListaXP2(LP2, mp); return;
     }

     if ((perm=arg[1])!=NULL && strlen(perm)<4) {
         if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
         if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE; 
         if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
      }

    if ((p=MmapFichero(arg[0],protection, mp))==NULL)
           perror ("Imposible mapear fichero");

    else printf ("fichero %s mapeado en %p\n", arg[0], p);
}



void * ObtenerMemoriaShmget (key_t clave, size_t tam, char * cs){
      void * p;
      int aux,id,flags=0777;
      struct shmid_ds s;

      if (tam)  /*si tam no es 0 la crea en modo exclusivo */
          flags=flags | IPC_CREAT | IPC_EXCL;/*si tam es 0 intenta acceder a una ya creada*/

      if (clave==IPC_PRIVATE)   /*no nos vale*/{
              errno=EINVAL; return NULL;
      }

      if ((id=shmget(clave, tam, flags))==-1)
             return (NULL);

      if ((p=shmat(id,NULL,0))==(void*) -1){
             aux=errno;   /*si se ha creado y no se puede mapear*/
             if (tam)     /*se borra */
               shmctl(id,IPC_RMID,NULL);
             errno=aux;return (NULL);
      }
      shmctl (id,IPC_STAT,&s);
      GuardarDireccion(p, s.shm_segsz, cs, NULL, clave);
      return (p);
}

void Cmd_AsignarCreateShared (char *arg[], char * cs){
      key_t k;
      size_t tam=0;
      void *p;

      if (arg[0]==NULL || arg[1]==NULL){ 
         imprimirListaXP2(LP2, cs); return;
      }

      k=(key_t) atoi(arg[0]);

      if (arg[1]!=NULL)
         if(!(tam=(size_t) atoll(arg[1]))){
             printf("No se asignan bloques de 0 bytes\n"); return;
          }


      if ((p=ObtenerMemoriaShmget(k,tam,cs))==NULL){
            fprintf(stderr, "Imposible asignar memoria compartida clave %d: ", k);
            perror ("");
      }
      else
         printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);

}

void Cmd_AsignarShared (char *arg[], char * cs){
      key_t k;
      size_t tam=0;
      void *p;

      if (arg[0]==NULL){ 
         imprimirListaXP2(LP2, cs); return;
      }

      k=(key_t) atoi(arg[0]);

      if ((p=ObtenerMemoriaShmget(k,tam,cs))==NULL){
           fprintf(stderr, "Imposible asignar memoria compartida clave %d: ", k);
            perror ("");
     }
     else
         printf ("Memoria compartida de clave %d asignada en %p\n",k,p);

}


void cmdAsignar(int argc, char *argv[]){
    char op_malloc=0, op_mmap=0, op_crearshared=0, op_shared=0;
    char *op[4] = {&op_malloc, &op_mmap, &op_crearshared, &op_shared};
    char *p[4]  = {"-malloc", "-mmap", "-crearshared", "-shared"};
    oplargas(argc, argv, p, op, 4);

    if (op_malloc){
        char mall [50];
        strcpy(mall, "malloc");
        
        if (argc>=3){   
            size_t tam= (size_t) atoll(argv[2]);
           
           if(tam){   
               char * mem;
               mem= malloc(tam); 
               
               GuardarDireccion(mem, tam, mall, NULL, 0);
               printf("allocated %s in %p \n", argv[2], mem);
           }
           else  printf("No se asignan bloques de 0 bytes\n");
        }
        else imprimirListaXP2(LP2, mall); 
        return;
     }


     if (op_mmap){
         char mp[50];
         strcpy(mp, "mmap");
         Cmd_AsignarMmap(argv+2, mp);
         return;
     }

     if (op_crearshared){
        char cs[50];
        strcpy(cs, "shared");
        Cmd_AsignarCreateShared(argv+2, cs);
        return;
     }

     if (op_shared){
        char cs[50];
        strcpy(cs, "shared");
        Cmd_AsignarShared(argv+2, cs);
        return;

     }

    if (argc>1){
       printf("uso: asignar [-malloc|-shared|-crearshared|-mmap] ....\n");
       return;
    }

    imprimirListaP2(LP2);

}

void cmdDesasignar(int argc, char * argv[]){
    char op_malloc=0, op_mmap=0, op_shared=0;
    char *op[3] = {&op_malloc, &op_mmap, &op_shared};
    char *p[3]  = {"-malloc", "-mmap", "-shared"};
    oplargas(argc, argv, p, op, 3);
    Datop2 d;

    if (op_malloc){
         char mall [50];
         strcpy(mall, "malloc");
        
         if(argc>=3){
             size_t tam = (size_t) atoll(argv[2]);
             
             d.n=mall;
             d.size=tam;

             if(borrarElemento(LP2, d, 1)==1)
                return;

             printf("No hay bloque de ese tamaño asignado con malloc\n");
             
         }
         imprimirListaXP2(LP2, mall);
         return;
    }

    if (op_mmap){
         char map [50];
         strcpy(map, "mmap");
         if (argc>=3){
             int s=0;
             d.n=map;
             d.f=argv[2];

             if((s=borrarElemento(LP2, d, 1))==-1){
                  perror("Imposible desmapear fichero");
                  return;
             }

             if(s==1) return;

             printf("Fichero %s no mapeado\n", argv[2]);

         }
         imprimirListaXP2(LP2, map);
         return;
    }

    if(op_shared){
         char cs [50];
         strcpy(cs, "shared");
         if(argc>=3){
            key_t k=(key_t) atoi(argv[2]);
            d.n=cs;
            d.a=k;
            int s=0;

            if((s=borrarElemento(LP2, d, 1))==-1){
                perror("Imposible desmapear clave");
                return;
            }
            
            if(s==1) return;

            printf("Clave %d no mapeada\n", k);
         }
         imprimirListaXP2(LP2, cs);
         return;
    }

    if (argc>=2){
           void * p;
           Datop2* dato;
           sscanf(argv[1], "%p", &p);
           int s= 0;
           
           dato = buscarDato(LP2, p);
           if(dato!=NULL){
               if((s=borrarElemento(LP2, *dato, 1))==-1){
                    perror("Imposible desmapear dirección");
                    return;              
               }
               if(s==1) return;
           }

          printf("Dirección %p no asignada con malloc, mmap o shared\n", p);
          return;
    }

   imprimirListaP2(LP2);

}


void Cmd_borrakey (char *args[]){
     key_t clave;
     int id;
     char *key=args[0];

     if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
            printf ("   rmkey  clave_valida\n"); return;
     }

     if ((id=shmget(clave,0,0666))==-1){
            perror ("shmget: imposible obtener memoria compartida"); return;
     }

     if (shmctl(id,IPC_RMID,NULL)==-1)
         perror ("shmctl: imposible eliminar memoria compartida\n");
     } 

void cmdBorrarkey(int argc, char * argv []){
     Cmd_borrakey(argv+1);

}


void cmdMem (int argc, char * argv []){
    char op_malloc=0, op_mmap=0, op_shared=0, op_all=0;
    char *op[4] = {&op_malloc, &op_mmap, &op_shared, &op_all};
    char *p[4]  = {"-malloc", "-mmap", "-shared", "-all"};
    oplargas(argc, argv, p, op, 4);

   if(op_malloc){
         char mall [50];
         strcpy(mall, "malloc");
         imprimirListaXP2(LP2, mall);
         return;
   }

   if(op_mmap){
         char map [50];
         strcpy(map, "mmap");
         imprimirListaXP2(LP2, map);
         return;
   }

   if(op_shared){
         char cs [50];
         strcpy(cs, "shared");
         imprimirListaXP2(LP2, cs);
         return;
   }

   if(op_all){
        imprimirListaP2(LP2);
        return;
   }

   if(argc==1){
      printf("Variables locales   %p,  %p,  %p\n", &op_malloc, &op_mmap, &op_shared);
      printf("Variables globales  %p,  %p,  %p\n",  &i, &ntrozos, &linea);
      printf("Funciones programa  %p,  %p,  %p\n",  cmdAutores,cmdAsignar, cmdPid);
      printf("Funciones libreria  %p,  %p,  %p\n",   shmctl, strcmp, printf);
   }

}

void cmdVolcar (int argc, char * argv[]){
  char * ptr;
  int i, j, tam, cont1=0, cont2=0, linea=25, times=0;
  unsigned char ch;
  int x;

  if (argc>1)
        sscanf(argv[1], "%p", &ptr);
  else return;

  if(argc==2)
     tam=linea;
  else tam = atoi(argv[2]);
 
  times = tam / linea +1;


  for (i=0; i<times; i++){
       for (j=0; j<linea && cont1<tam; j++){ 
          ch = (unsigned char ) ptr[0];
          x = (int) ch;
          if(isprint(x))
              printf("%3c", ptr[0]);
          else printf("%3s", " ");
          ptr= ptr +1;
          cont1+=1;
       }
       printf("\n");
       ptr+= -j;

       for(j=0; j<linea && cont2<tam; j++){
          ch = (unsigned char ) ptr[0];
          x = (int) ch;
          printf("%3.2x", x);
          ptr= ptr +1;
          cont2 +=1; 
       }
       printf("\n");
  }
}


void cmdLlenar (int argc, char * argv[]){
     int tam = 128;
     unsigned char ch = 'A';
     unsigned int x;
     char * ptr;
     int i;


     if(argv[1]!=NULL){
     
         sscanf(argv[1], "%p", &ptr);

         if(argv[2]!=NULL)
             tam = atoi(argv[2]);

         if(argv[3]!=NULL){
            if (strncmp(argv[3],"0x", 2)==0){
                sscanf(argv[3], "%x", &x);
                ch = (char) x;
            }
            else if(isdigit(argv[3][0])){
                   sscanf(argv[3], "%u", &x);
                   ch = (char) x;
              }
              else sscanf(argv[3], "%c", &ch);
          }

          for(i=0; i<tam; i++){
              ptr[0]=ch;
              ptr=ptr+1;
          }

     }  

}

void doRecursiva(int n){
    char automatico[TAMANO];
    char estatico[TAMANO];

    printf ("parametro n:%d en %p\n",n,&n);
    printf ("array estático en:%p \n",estatico);
    printf ("array automático en %p\n",automatico);

    n--;

    if (n>0)
      doRecursiva(n);

}

void cmdRecursiva(int argc, char * argv []){
   
    if(argv[1]!=NULL)
        doRecursiva(atoi(argv[1]));
}



ssize_t LeerFichero (char *fich, void *p, ssize_t n){
    ssize_t  nleidos,tam=n;
    int df, aux;
    struct stat s;

    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
            return ((ssize_t)-1);

    if (n==LEERCOMPLETO)
           tam=(ssize_t) s.st_size;

    if ((nleidos=read(df,p, tam))==-1){
            aux=errno;
            close(df);
            errno=aux;
            return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

void cmdRfich (int argc, char * argv []){

    if(argc>2){
       char * file = argv[1];
       void * p;
       sscanf(argv[2], "%p", &p);
       ssize_t size = LEERCOMPLETO;
       ssize_t res;

       if(argv[3]!=NULL)
           size = (ssize_t) atoll(argv[3]);

       if ((res=LeerFichero(file, p, size))== (ssize_t) -1){
          perror("Imposible leer fichero");
          return;
       }
       
       printf("leidos %ld bytes de %s en %p\n", res, file, p);
       return;
    }
   
    printf("faltan parametros\n");

}

ssize_t EscribirFichero (char *fich, void *p, ssize_t n, int modo){
    ssize_t  nleidos,tam=n;
    int df, aux;
    struct stat s;

    if ((df=open(fich,O_WRONLY | modo, S_IRWXU | S_IRGRP |S_IROTH))==-1)
            return ((ssize_t)-1);
    
    if(stat (fich,&s)==-1 )
          return ((ssize_t)-1);

    if ((nleidos=write(df,p, tam))==-1){
            aux=errno;
            close(df);
            errno=aux;
            return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}


void cmdWfich (int argc, char * argv []){
     char op_o=0;
     char *op[1]={&op_o};
     opciones(argc, argv, "o", op);
     
     
     if(argc>3){
        char * file;
        ssize_t size;
        ssize_t res;
        void * p;
        int modo ;
    
        if(op_o){
             modo= O_CREAT;
             sscanf(argv[3], "%p", &p);
             size = (ssize_t) atoll (argv[4]);
             file = argv[2];
        }
        else {
             modo =  O_EXCL | O_CREAT;
             sscanf(argv[2], "%p", &p);
             size = (ssize_t) atoll (argv[3]);
             file = argv[1];   
        }
       
        if((res=EscribirFichero(file, p, size, modo))==-1){
             perror("Imposible escribir fichero");
             return;
        }
        
        printf("escritos %ld bytes en %s desde %p\n",res, file, p);
        return;
     }
    
     printf("faltan parametros\n");

}


void changePriority (int pid, int prio){
       
       if (setpriority(PRIO_PROCESS, pid, prio)!=0){
              perror ("No se pude cambiar la prioridad");
             return;
        }

}

void cmdPriority (int argc, char * argv []){
   pid_t pid;
   int prio;
   errno =0;

   if (argv[1]==NULL){
         pid = getpid();
   }
   else pid = (pid_t) atoi(argv[1]);

   if (argc==3){
        prio = atoi (argv[2]);
        changePriority(pid, prio);   
   } 

   prio = getpriority(PRIO_PROCESS, pid);
   if(errno!=0){
      perror("Error al consultar prioridad");
      return;
   }

   if (argc==3){
        printf("Prioridad del proceso %d cambiada a %d\n", pid, prio );   
        return;
   }
   
   printf("Prioridad del proceso %d es %d\n", pid, prio);

}

void cmdFork (int argc, char *argv []){
   pid_t pid;


   if((pid = fork())!=0){    
      if (pid==-1)
        perror("fork");
      else{
       printf("ejecutando proceso %d\n", pid);
       waitpid(pid, NULL, 0);
      }
    }
}


void cmdExec (int argc, char *argv []){
     int prio=0;
     int desp = 1;
     
     if (argc>1){
          
          if (strncmp(argv[1], "@", 1) ==0){
              if (argc==2) return;

              prio = atoi(argv[1]+1);
              changePriority(0, prio);
              desp = 2;
          }
          
          if(execvp(argv[desp], argv+desp)==-1)
            perror ("Imposible ejecutar");
     }


}

void cmdPplano (int argc, char *argv []){
   pid_t pid;

   if ((pid = fork())!=0){
      if (pid==-1)
        perror("fork");
      else{
       waitpid(pid, NULL, 0);
      }

   }
   else {
          cmdExec(argc, argv);
          exit(0);
   }

}

int cuentaCaracteres (char * argv []){
     int i=0, res=0;
    
     while (argv[i]!=NULL){
          res = res + strlen(argv[i]) + 1;
          i++;
     }
     return res;
}

void concatenaString (char * argv[], char ** res ){
    int i=0; 


    while (argv[i]!=NULL){
       strcat (*res, " ");
       strcat (*res, argv[i]);
       i++;
      }
      
    
}



void insertarProceso (pid_t pid, char * argv []){
    DatoProcesos dato;
    errno=0;
    int prio;
    char time [40] ;
    char * prog = (char *) malloc (cuentaCaracteres(argv));
    
    dato.pid = pid;
   
    prio= getpriority(PRIO_PROCESS, pid);
    dato.prio = prio;
    if (errno != 0){
      perror ("Imposible obtener prioridad");
      return;
    }

    Fecha(time, 40);
    dato.time = time;

    strcpy(dato.estado, "ACTIVO");
    dato.sig = 0;
    strcpy(prog, argv[0]);
    concatenaString(argv+1, &prog);
    dato.prog = prog;
  
    insertarElemento (LP, dato);

}


void cmdSplano (int argc, char *argv []){
   pid_t pid;

   if ((pid = fork())!=0){
      if (pid==-1){
        perror("fork");
      }
      else {
             if (argc>1)
                   insertarProceso(pid, argv+1);
      }
   }
   else {
         cmdExec(argc, argv);
         exit(0);
   }
}

void cmdListarprocs (int argc, char * argv []){
     mostrarElementos(LP);
}

void cmdProc (int argc, char * argv []){
     char op_fg=0;
     char *op[1]={&op_fg};
     char *p[1]  = {"-fg"};
     pid_t pid;
     int pos;
     int estado;
     DatoProcesos dato;


     oplargas(argc, argv, p, op, 1);

     if (argc>1){
           
            if (op_fg && argc>2){
              
                   if ((pid = atoi(argv[2]))!= 0 && (pos=buscarElemento(LP, pid))!=-1){
                        dato = obtenerElemento(LP,pos);
                        if (strncmp(dato.estado, "T", 1)){
                               waitpid (dato.pid, &estado, 0);
                               opcionesEstado(&dato, estado);

                               if (!strncmp(dato.estado, "T", 1)){
                                   printf("El proceso %d ha terminado normalmente. Valor devuelto %d\n", dato.pid, dato.sig);
                               }
                               else printf("Proceso %d terminado por la senal %s\n", dato.pid, dato.sen);

                               eliminarElemento(LP, pos);
                        }
                   }
                   return;
            }
            else{
                  pid = atoi (argv[1]);
                  if ((pos = buscarElemento(LP, pid))!=-1){
                     mostrarElemento(obtenerElemento(LP, pos));
                    return;
                  }
            }   

     }

     mostrarElementos(LP);

}


void cmdBorrarprocs (int argc, char * argv []){
   char op_term=0, op_sig=0;
   char *op[2]={&op_term, &op_sig};
   char *p[2]  = {"-term", "-sig"};
   oplargas(argc, argv, p, op, 2);

   if (op_term){
      if (eliminarElementos (LP, 'T'))
          return;
   }

   if (op_sig){
      if (eliminarElementos (LP, 'S'))
      return;
   }
  
   mostrarElementos(LP);

}

void ejecutaLinea (int argc, char * argv [], int plano){
    char * param [argc+2];
    i=1;
  
    while (i<argc+1){
      param[i] = argv [i-1];
      i++;
    }
    param[i] = NULL;
   

    if (plano==1)
       cmdPplano(argc+2, param);
    else cmdSplano (argc+2, param);

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
    {"priority", cmdPriority},
    {"pplano", cmdPplano},
    {"splano", cmdSplano},
    {"exec", cmdExec},
    {"fork", cmdFork},
    {"listarprocs", cmdListarprocs},
    {"borrarprocs", cmdBorrarprocs},
    {"proc", cmdProc},
    {"exit", cmdExit},
    {"fin", cmdExit},
    {"end", cmdExit},
    {NULL,NULL}
};

  
 
int main() {
     errno=0;
     L= listaVacia();
     LP2= listaVaciaP2();
     LP = crearLista();
     

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
          if (tablacmd[i].nombre==NULL){
             if (!strcmp(trozos[ntrozos-1], "&"))
               ejecutaLinea(ntrozos-1, trozos, 2);
             else ejecutaLinea(ntrozos, trozos, 1);
             break;
          }
      }
        
   }
    
}

