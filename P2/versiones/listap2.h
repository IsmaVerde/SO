
typedef struct Datop2 {
   void * p;
   size_t size;
   char * n;
   char * f;
   int a;
   char *fecha;

} Datop2;

typedef struct nodop2 {
       Datop2 dato;
       struct nodop2 *sig;
} *LISTAp2;

LISTAp2 listaVaciaP2();

int esVaciaP2 (LISTAp2 l);

void InsertElementP2 (LISTAp2 l, Datop2 d);

void imprimeElementoP2 (Datop2 dato);

void imprimirListaP2 (LISTAp2 l);

void imprimirListaXP2(LISTAp2 l, char * comp);

Datop2 * buscarDato(LISTAp2 l, void * dir);

int borrarElemento(LISTAp2 l, Datop2 d);

void liberarListaP2 (LISTAp2 l);




