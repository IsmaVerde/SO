

typedef struct nodo {
       char * dato;
       struct nodo *sig;
} *LISTA;

LISTA listaVacia();

int esVacia (LISTA l);

void InsertElement (LISTA l, char *d);

void imprimirLista (LISTA l);

char * Elemento (LISTA l);

void borrarElementos(LISTA l);

void liberarLista (LISTA *l);


