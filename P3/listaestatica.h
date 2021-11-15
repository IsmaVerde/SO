#define MAX 4096

typedef struct DatoProcesos {
    pid_t pid;
    int prio;
    char * time;
    char estado [20];
    int sig;
    char sen [20];
    char * prog;

} DatoProcesos;

typedef struct nodoest {
	DatoProcesos arr [MAX];
	int fin;
} * LISTAE;


LISTAE crearLista ();

int esListaVacia (LISTAE l);

int insertarElemento (LISTAE l, DatoProcesos d);

DatoProcesos obtenerElemento (LISTAE l, int pos);

int buscarElemento (LISTAE l, pid_t pid);

void eliminarElemento (LISTAE l, int pos);

void opcionesEstado (DatoProcesos *dato, int estado);

void actualizaElemento (DatoProcesos * dato);

void mostrarElemento (DatoProcesos dato);

void mostrarElementos (LISTAE l);

int eliminarElementos (LISTAE l, char condicion);

void liberarlista(LISTAE l);