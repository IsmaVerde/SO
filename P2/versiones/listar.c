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



extern int errno;

void opciones(int argc, char *argv[], char *letras, char *op[]){

   int i, j;
   for (i=1; i<argc; i++)
     if(argv[i][0]=='-' && strlen(argv[i])==2)
       for(j=0;letras[j]!='\0'; j++)
         if(letras[j]==argv[i][1])
           op[j][0]=1;
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





int main (int argc, char *argv []){
  errno=0;
  cmdListar(argc, argv);

return 0;


}