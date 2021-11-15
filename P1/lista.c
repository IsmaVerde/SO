#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

LISTA listaVacia(){
   LISTA aux=(LISTA)(malloc(sizeof(struct nodo)));
   aux->dato=NULL;
   aux->sig=NULL;
   return aux;
}

int esVacia (LISTA l){
   return ((l->sig==NULL)&(l->dato==NULL));
}



void InsertElement (LISTA l, char *d){
   LISTA aux = (LISTA)(malloc(sizeof(struct nodo)));
   LISTA p;
   
   aux->dato=(char *)(malloc(sizeof(d)*strlen(d)));
   strcpy(aux->dato, d);
   aux->sig=NULL;

   if (esVacia(l)){l->sig=aux; return;}
   for(p=l->sig;p->sig!=NULL;p=p->sig);
   p->sig=aux;
       
}

void imprimirLista (LISTA l){
   LISTA p;

   for(p=l->sig; p!=NULL; p=p->sig){
      printf("%s", p->dato);}
}

void liberarLista (LISTA l){
   LISTA p;

   while(!esVacia(l)){
       for(p=l;p->sig->sig!=NULL;p=p->sig);
       free(p->sig);
       p->sig=NULL; 
   }
}





