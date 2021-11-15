#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include "listap2.h"


LISTAp2 listaVaciaP2(){
   LISTAp2 aux=(LISTAp2)(malloc(sizeof(struct nodop2)));
   aux->sig=NULL;
   return aux;
}

int esVaciaP2 (LISTAp2 l){
   return (l->sig==NULL);
}

void InsertElementP2 (LISTAp2 l, Datop2 d){
   LISTAp2 aux = (LISTAp2)(malloc(sizeof(struct nodop2)));
   LISTAp2 p;
   
  aux->dato.p=d.p;
  aux->dato.size=d.size;
  aux->dato.n= (char *) malloc(sizeof(char)*(strlen(d.n)+1));
  strcpy(aux->dato.n, d.n);
  if(d.f!=NULL){
        aux->dato.f = (char *) malloc(sizeof(char)*(strlen(d.f)+1));
        strcpy(aux->dato.f, d.f);
  }
  aux->dato.a=d.a;
  aux->dato.fecha = (char *) malloc(sizeof(char)*(strlen(d.fecha)+1));
  strcpy(aux->dato.fecha, d.fecha);

   aux->sig=NULL;

   if (esVaciaP2(l)){l->sig=aux; return;}
   for(p=l->sig;p->sig!=NULL;p=p->sig);
   p->sig=aux;
       
}


void imprimeElementoP2 (Datop2 dato){
      char imp[50];
      if(strcmp(dato.n, "malloc")==0)
              strcpy(imp, dato.n);

      if(strcmp(dato.n, "mmap")==0)
             sprintf(imp, "mmap %s (fd:%d)", dato.f, dato.a);

      if(strcmp(dato.n, "shared")==0)
             sprintf(imp, "shared memory (key %d)", dato.a);

      printf("%p: size:%lu. %s  %s\n",dato.p, dato.size, imp, dato.fecha);
}

void imprimirListaP2 (LISTAp2 l){
   LISTAp2 p;
  
   for(p=l->sig; p!=NULL; p=p->sig)
   	    imprimeElementoP2(p->dato);
   	  
    
}

void imprimirListaXP2(LISTAp2 l, char * comp){
   LISTAp2 p;

   for(p=l->sig; p!=NULL; p=p->sig){
   	  if(strcmp(p->dato.n, comp)==0)
   	          imprimeElementoP2(p->dato);
    }
}

Datop2 * buscarDato(LISTAp2 l, void * dir){
   LISTAp2 p;


   for(p=l->sig; p!=NULL; p=p->sig){
         if(p->dato.p == dir)
             return &(p->dato);

   }
  return NULL;

}



int borrarElemento(LISTAp2 l, Datop2 d){
   LISTAp2 p;
   struct nodop2 * aux;
   Datop2 comp;
   int made = 0;
   char imp[14];
   

   if(!esVaciaP2(l)){
         for(p=l; p->sig!=NULL; p=p->sig){
                comp=p->sig->dato;
                if(strcmp(comp.n, d.n)==0){
                      if((made=(strcmp(comp.n, "malloc")==0 && comp.size==d.size))){
                        sprintf(imp, "%p", comp.p);
                        free(comp.p);
                      }
                      if(strcmp(comp.n, "mmap")==0 && strcmp(comp.f, d.f)==0){
                        made=1;
                        sprintf(imp, "%p", comp.p);
                        if(close(comp.a)==-1 || munmap(comp.p, comp.size)==-1)
                            return -1;
                      }
                      if(strcmp(comp.n, "shared")==0 && comp.a==d.a){
                        made=1;
                        sprintf(imp, "%p", comp.p);
                        if(shmdt(comp.p)==-1)
                            return -1;
                      }
                      
                      if(made){
                           printf("block at %s deallocated (%s)\n",imp, comp.n);
                           free(comp.n);
                           free(comp.fecha);
                           if(comp.f!=NULL)
                              free(comp.f);
                           aux=p->sig;
                           p->sig=p->sig->sig;
                           free(aux);
                           return 1;
                      }
                 }
         }
    }
     
    return 0;
}


void liberarListaP2 (LISTAp2 l){
   LISTAp2 p;

   while(!esVaciaP2(l)){
       for(p=l;p->sig->sig!=NULL;p=p->sig);
       free(p->sig);
       p->sig=NULL; 
   }
}