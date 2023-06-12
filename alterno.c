#include <stdio.h>
#include <stdlib.h>

//**********ESTRUCTURAS A USAR********************
/// @brief Esta estructura almacena la info del proceso: su id y tamaño. id= -1 Indica memoria disponible
typedef struct Proceso{
    int id_proceso; // El identificador de proceso
    int tam; //Tamaño del proceso
} Proceso;

/// @brief Este nodo es para la lista de procesos que se lee del archivo
typedef struct nodoPro{
    Proceso proc;
    struct nodoPro* siguiente;
} NodoProc;

/// @brief Pensada únicamente para hacer una cola de procesos
typedef struct 
{
    NodoProc* frente;
    NodoProc* final;
} Cola;

/// @brief Este nodo es para la lista de direcciones del vector de areas libres
typedef struct nodoDirs{
    int dir; //"dirección" de memoria en la que se almacena el proceso, 0-15
    Proceso proc; //Datos del proceso
    struct nodoDirs* siguiente; //La dirección del siguiente elemento en la lisya
} Nodo;

/// @brief Esta lista es la que se incluye en cada slot del vector de areas libres
typedef struct {
    Nodo* inicio;
    Nodo* fin;
    int longitud;
} Lista;

//*********PROTOTIPOS DE FUNCIONES********************
//Operaciones vectorAL
Lista** inicializarVectorAL();
void cerrarVectorAL(Lista **);
void imprimirVector(Lista **);
void imprimirRenglon(int, Lista*);
//Operaciones de lista
Lista* crearLista();
void agregarFinal(Lista*, Proceso, int);
void borrarElemento(Lista*, int);
void liberarLista(Lista*);
int listaEstaVacia(Lista);
//Operaciones de Cola
Cola crearCola();
int colaEstaVacia(Cola);
void encolar(Cola*, Proceso);
Proceso frente(Cola);
Proceso desencolar(Cola*);
void liberarCola(Cola* );
//Lectura del archivo
FILE* verificarArchivo(int, char**);
Cola encolarProcesos(Cola *,FILE *);



int main(int argc, char* argv[]){
    int id_proceso, tam;
    Cola cola_procesos = crearCola(); //Contiene todos los procesos indicados en el archivo de texto
    Lista **vectorAL = inicializarVectorAL();

    FILE * archivo = verificarArchivo(argc,argv);
    if (archivo == NULL){
        return -1;
    }
    
    encolarProcesos(&cola_procesos, archivo);

    while(!colaEstaVacia(cola_procesos)){
        Proceso prueba = desencolar(&cola_procesos);
        printf("Proceso: %d Tamaño: %d.\n",prueba.id_proceso,prueba.tam);
    }    

    //En este punto, todo el archivo fue codificado a la cola de procesos
    //TO-DO:
    //Verificar si el tamaño es -1 o >0
        //Si es mayor a cero, asignarle espacio
            //Verificar si hay espacio
                //Si hay espacio, almacenarlo
                //Si no hay, dividirlo en mitades
        //Si es -1 liberar memoria
    //Actualizar vector de AL
    //Imprimir memoria

    imprimirVector(vectorAL);

    liberarCola(&cola_procesos);
    cerrarVectorAL(vectorAL);
    return 0;
}

/// @brief Crea el vector de areas libres y devuelve su dirección
Lista** inicializarVectorAL(){
    int i=0;
    Proceso libre;
    libre.id_proceso = -1;
    libre.tam = 0;
    Lista** vectorAL = (Lista**)malloc(5 * sizeof(Lista));
    for(i=0;i<5;i++){
        vectorAL[i] = crearLista();
    }
    agregarFinal(vectorAL[4],libre,0);  //El elemento 4 está inicializado con el "proceso" -1
    return vectorAL;
}

void cerrarVectorAL(Lista ** vectorAL){
    int i;
    for(i = 0; i < 5;i++){
        liberarLista(vectorAL[i]);
    }
    free(vectorAL);
}


/// @brief Crea una nueva lista
/// @return Regresa una lista con inicio y fin en NULL, con longitud 0;
Lista * crearLista(){
    Lista* lista = (Lista*)malloc(sizeof(Lista));
    lista->inicio = NULL;
    lista->fin = NULL;
    lista->longitud = 0;
    return lista;
}

/// @brief Agrega un nodo al final de una lista 
/// @param lista Lista que se usa
/// @param proc Proceso del nodo
/// @param dir Dirección del nodo
void agregarFinal(Lista* lista, Proceso proc, int dir){
    Nodo* nuevoNodo = (Nodo*)malloc(sizeof(Nodo));
    nuevoNodo->dir = dir;
    nuevoNodo->proc = proc;
    nuevoNodo->siguiente = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = nuevoNodo;
        lista->fin = nuevoNodo;
    } else {
        lista->fin->siguiente = nuevoNodo;
        lista->fin = nuevoNodo;
    }

    lista->longitud++;

}


/// @brief Elimina un elemento específico de la lista
/// @param lista La lista a la que se quitará un elemento
/// @param posicion Posición del elemento que se va a borrar. Empieza en 0
void borrarElemento(Lista* lista, int posicion) {
    if (posicion < 0 || posicion >= lista->longitud) {
        printf("Posición inválida.\n");
        return;
    }

    Nodo* nodoActual = lista->inicio;
    Nodo* nodoAnterior = NULL;
    int indice = 0;

    while (indice < posicion) { //Recorre la lista nodo por nodo
        nodoAnterior = nodoActual;
        nodoActual = nodoActual->siguiente;
        indice++;
    }

    if (nodoAnterior == NULL) {
        // Borrar el primer nodo
        lista->inicio = nodoActual->siguiente;
    } else {
        // Borrar un nodo en el medio o al final
        nodoAnterior->siguiente = nodoActual->siguiente;

        if (nodoActual == lista->fin) {
            lista->fin = nodoAnterior;
        }
    }
    free(nodoActual);
    lista->longitud--;
}

/// @brief Libera todos los elementos de la lista.
/// @param lista La lista que se va aliberar de memoria
void liberarLista(Lista* lista) {
    Nodo* nodoActual = lista->inicio;
    Nodo* nodoSiguiente = NULL;

    while (nodoActual != NULL) {
        nodoSiguiente = nodoActual->siguiente;
        free(nodoActual);
        nodoActual = nodoSiguiente;
    }

    lista->inicio = NULL;
    lista->fin = NULL;
    lista->longitud = 0;
}

int listaEstaVacia(Lista lista){
    return lista.longitud;
}

Cola crearCola(){
    Cola cola;
    cola.frente = NULL;
    cola.final = NULL;
    return cola;
}

int colaEstaVacia(Cola cola){
    return cola.frente == NULL; //Compara si es NULL
}

void encolar(Cola* cola, Proceso proc) {
    NodoProc* nuevoNodo = (NodoProc*)malloc(sizeof(NodoProc));
    nuevoNodo->proc = proc;
    nuevoNodo->siguiente = NULL;

    if (colaEstaVacia(*cola)) {
        cola->frente = nuevoNodo;
        cola->final = nuevoNodo;
    } else {
        cola->final->siguiente = nuevoNodo;
        cola->final = nuevoNodo;
    }
}

Proceso frente(Cola cola) { //Ve el elemento de enfrente
    Proceso proc;
    if (colaEstaVacia(cola)) {
        printf("La cola está vacía.\n");
        return proc; 
    }
    return cola.frente->proc;
}

/// @brief Regresa el proceso del elemento de enfrente y lo desencola
/// @param cola La cola de procesos
/// @return El proceso que estaba al frente
Proceso desencolar(Cola* cola) { //Regresa el proceso del elemento del frnte y desencola
    Proceso proc;
    if (colaEstaVacia(*cola)) {
        printf("La cola está vacía.\n");
        return proc; 
    }
    NodoProc* nodoEliminado = cola->frente;
    proc = nodoEliminado->proc;
    cola->frente = cola->frente->siguiente;
    if (cola->frente == NULL) {
        cola->final = NULL;
    }
    free(nodoEliminado);
    return proc; 
}

void liberarCola(Cola* cola) {
    while (!colaEstaVacia(*cola)) {
        desencolar(cola);
    }
}

FILE* verificarArchivo(int argc, char* argv[]){
    if (argc < 2) { //Para ejecutarse, se debe dar el nombre del archivo
        printf("Ingresa el nombre del archivo como primer argumento.\n");
        return NULL;
    }

    FILE* archivo = fopen(argv[1], "r"); //De los argumentos abre el 1 (el cero es el programa)
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return NULL;
    }

    return archivo;
}

Cola encolarProcesos(Cola * cola_procesos,FILE * archivo){
    int id_proceso, tam;
    while (fscanf(archivo, "%d %d", &id_proceso, &tam) == 2) {
        Proceso p;
        p.id_proceso = id_proceso;
        p.tam = tam;
        encolar(cola_procesos,p);
    }
    fclose(archivo);
}

void imprimirVector(Lista *lista[]){
    printf("Vector de areas libres\n");
    printf("╔═══════╗\n");
    printf("║       ║\n");
    imprimirRenglon(0,lista[0]);
    printf("║       ║\n");
    printf("╠═══════╣\n");
    printf("║       ║\n");
    imprimirRenglon(1,lista[1]);
    printf("║       ║\n");
    printf("╠═══════╣\n");
    printf("║       ║\n");
    imprimirRenglon(2,lista[2]);
    printf("║       ║\n");
    printf("╠═══════╣\n");
    printf("║       ║\n");
    imprimirRenglon(3,lista[3]);
    printf("║       ║\n");
    printf("╠═══════╣\n");
    printf("║       ║\n");
    imprimirRenglon(4,lista[4]);
    printf("║       ║\n");
    printf("╚═══════╝\n");

}

void imprimirRenglon(int numero,Lista *lista){
    printf("║   %d   ║",numero);

    Nodo* nodoActual = lista->inicio;
    while (nodoActual != NULL) {
        printf(" -> [%d]", nodoActual->dir);
        nodoActual = nodoActual->siguiente;
    }

    printf("\n");  
}