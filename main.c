#include <stdio.h>
#include <stdlib.h>

///********ESTRUCTURAS******************
typedef struct nodoA{
    int id_proceso; // El identificador de proceso
} Pagina;

typedef struct nodoB{
    int *marcos; //Apuntador a los marcos
    int tamano; // tamano del área libre en marcos de página
} AreaLibre;

///*****PROTOTIPO DE FUNCIONES***************
void inicializarAreasLibres(AreaLibre *);
void liberarAreasLibres(AreaLibre *);
void mostrarMapaMemoria(Pagina *, AreaLibre *);


int main(int argc, char *argv[]) { //argc numero de argumentos dados y argv arreglo de cadenas
    // Verificar si se proporcionó un archivo como argumento de línea de comandos
    if (argc < 2) { //dos por el nombre del programa y el del archivp
        printf("Error: Debe proporcionar un archivo como argumento.\n");
        return 1;
    }

    // Abrir el archivo para lectura
    FILE *archivo = fopen(argv[1], "r"); //Toma el primer argumento como el archivo a leer
    if (archivo == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    // Crear el vector de áreas libres y la memoria real
    AreaLibre areasLibres[5];
    inicializarAreasLibres(areasLibres);

    Pagina memoriaReal[16];
    int i;
    for (i = 0; i < 16; i++) {
        memoriaReal[i].id_proceso = -1; // Inicialmente todas las páginas están libres
    }

    // Mostrar el estado inicial
    mostrarMapaMemoria(memoriaReal, areasLibres);

    int id_proceso, accion;
    while (fscanf(archivo, "%d %d", &id_proceso, &accion) == 2) {
        if (accion > 0) {
            // Acción: Asignar páginas al proceso
            int i;
            for (i = 0; i < 5; i++) {
                if (areasLibres[i].tamano >= accion) {
                    // Encontramos un área libre con tamaño suficiente
                    //asignarPaginas(id_proceso, accion, areasLibres[i].marcos, memoriaReal);
                    break;
                }
            }
            if (i == 5) {
                // No se encontró un área libre con tamaño suficiente, dividir en potencias de dos
                int paginasRestantes = accion;
                for (i = 0; i < 5; i++) {
                    if (areasLibres[i].tamano > paginasRestantes) {
                        //asignarPaginas(id_proceso, paginasRestantes, areasLibres[i].marcos, memoriaReal);
                        //dividirAreaLibre(i, paginasRestantes, areasLibres);
                        break;
                    } else {
                        //asignarPaginas(id_proceso, areasLibres[i].tamano, areasLibres[i].marcos, memoriaReal);
                        paginasRestantes -= areasLibres[i].tamano;
                        areasLibres[i].marcos = NULL; // El área libre se ha utilizado completamente
                    }
                }
            }
        } else if (accion == -1) {
            // Acción: Terminar proceso
            //liberarPaginas(id_proceso, memoriaReal, areasLibres);
        } else {
            // Acción inválida
            printf("Acción inválida para el proceso %d\n", id_proceso);
        }
    }
    // Liberar la memoria utilizada
    liberarAreasLibres(areasLibres);
    fclose(archivo);

    return 0;
}

//*******DESARROLLO DE FUNCIONES**************

void inicializarAreasLibres(AreaLibre *areasLibres) {
    int i;
    areasLibres[0].marcos = (int *)malloc(sizeof(int)); //Da memoria para el primer marco
    areasLibres[0].marcos[0] = 0; //Asigna al primer marco a cero
    areasLibres[0].tamano = 1; 
    
    for (i = 1; i < 5; i++) { //Empieza en 1 para los de 2^1,2^2,2^3...
        areasLibres[i].marcos = NULL;
        areasLibres[i].tamano = (1 << i);
    }
}

// Función para liberar la memoria utilizada por el vector de áreas libres
void liberarAreasLibres(AreaLibre *areasLibres) {
    int i;
    for (i = 0; i < 5; i++) {
        free(areasLibres[i].marcos);
    }
}

// Función para mostrar el mapa de memoria real y el vector de áreas libres
void mostrarMapaMemoria(Pagina *memoriaReal, AreaLibre *areasLibres) {
    int i;

    printf("Mapa de memoria real:\n");
    for (i = 0; i < 16; i++) {
        printf("[%d] Proceso: %d\n", i, memoriaReal[i].id_proceso);
    }

    printf("\nVector de áreas libres:\n");
    for (i = 0; i < 5; i++) {
        printf("[%d] Tamaño: %d\n", i, areasLibres[i].tamano);
        printf("    Marcos: ");
        if (areasLibres[i].marcos == NULL) {
            printf("Ninguno\n");
        } else {
            int j;
            for (j = 0; j < areasLibres[i].tamano; j++) {
                printf("%d ", areasLibres[i].marcos[j]);
            }
            printf("\n");
        }
    }
}