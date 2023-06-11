#include <stdio.h>
#include <stdlib.h>

//**********ESTRUCTURAS A USAR********************
typedef struct Proceso{
    int id_proceso; // El identificador de proceso
    int tam; //Tamaño del proceso
} Proceso;

typedef struct nodoPro{
    Proceso proc;
    struct nodoPro* siguiente;
} NodoProc;

typedef struct 
{
    NodoProc* frente;
    NodoProc* final;
} Cola;

typedef struct nodoDirs{
    int dir; //"dirección" de memoria en la que se almacena el proceso, 0-15
    Proceso proc; //Datos del proceso
    struct nodoDirs* siguiente; //La dirección del siguiente elemento en la lisya
} Nodo;

typedef struct {
    Nodo* inicio;
    Nodo* fin;
    int longitud;
} Lista;

//*********PROTOTIPOS DE FUNCIONES********************
Lista crearLista();
void agregarFinal(Lista*, Proceso, int);
void borrarElemento(Lista*, int);
void inicializarVectorAL();
void liberarLista(Lista*);


int main(int argc, char* argv[]){
    int id_proceso, tam; 

    if (argc != 2) {
        printf("Ingresa solo el nombre del programa y del archivo.\n");
        return 1;
    }

    FILE* archivo = fopen(argv[1], "r"); //De los argumentos abre el 1 (el cero es el programa)
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    while (fscanf(archivo, "%d %d", &id_proceso, &tam) == 2) {
        agregarAlFinal(&lista, entero1, entero2);
    }


    inicializarVectorAL();



    return 0;
}

void inicializarVectorAL(){
    int i=0;
    Lista vectorAL[5];
    for(i = 0; i < 5;i++){
        vectorAL[i] = crearLista();
    }
}


Lista crearLista(){
    Lista lista;
    lista.inicio = NULL;
    lista.fin = NULL;
    lista.longitud = 0;
    return lista;
}

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

Cola crearPila(){
    Cola cola;
    cola.frente = NULL;
    cola.final = NULL;
    return cola;
}

int estaVacia(Cola cola){
    return cola.frente == NULL; //Compara si es NULL
}

void encolar(Cola* cola, Proceso proc) {
    NodoProc* nuevoNodo = (NodoProc*)malloc(sizeof(NodoProc));
    nuevoNodo->proc = proc;
    nuevoNodo->siguiente = NULL;

    if (estaVacia(*cola)) {
        cola->frente = nuevoNodo;
        cola->final = nuevoNodo;
    } else {
        cola->final->siguiente = nuevoNodo;
        cola->final = nuevoNodo;
    }
}

Proceso frente(Cola cola) { //Ve el elemento de enfrente
    if (estaVacia(cola)) {
        printf("La cola está vacía.\n");
        return -1; 
    }
    return cola.frente->proc;
}

Proceso desencolar(Cola* cola) { //Regresa el proceso del elemento del frnte y desencola
    if (estaVacia(*cola)) {
        printf("La cola está vacía.\n");
        return -1; 
    }
    Nodo* nodoEliminado = cola->frente;
    Proceso proc = nodoEliminado->proc;
    cola->frente = cola->frente->siguiente;
    if (cola->frente == NULL) {
        cola->final = NULL;
    }
    free(nodoEliminado);
    return proc; 
}

void liberarCola(Cola* cola) {
    while (!estaVacia(*cola)) {
        desencolar(cola);
    }
}