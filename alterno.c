#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
//void borrarElemento(Lista*, int);
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
//Memoria
int memoriaDisponible(Lista**);
void imprimirMemoria(Lista **);
int dividirMemoria(Lista**,int);
int solicitarDirMemoria(int,Lista**,int);
int asignarProceso(Proceso , Lista **, int );
int liberarDireccion(Lista **, int);
int buscarPorceso(Proceso,Lista **);
int unirMemoria(Lista **);
void colocarAlFrente(Cola*, Proceso);
int dividirProceso(Lista**, Proceso, Cola *);


int main(int argc, char* argv[]){
    int id_proceso, tam;
    Cola cola_procesos = crearCola(); //Contiene todos los procesos indicados en el archivo de texto
    Lista **vectorAL = inicializarVectorAL();
    imprimirMemoria(vectorAL);
    imprimirVector(vectorAL);

    FILE * archivo = verificarArchivo(argc,argv);
    if (archivo == NULL){
        return -1;
    }

    //Del archivo, guarda todos los procesos en la cola y cierra el archivo    
    encolarProcesos(&cola_procesos, archivo);
    
    while(!colaEstaVacia(cola_procesos)){
        Proceso proceso_actual = desencolar(&cola_procesos);
        int tam_proceso = proceso_actual.tam, direccion_almacen, dir_proceso_borrar;
        if(tam_proceso==-1){
            dir_proceso_borrar = buscarPorceso(proceso_actual,vectorAL);
            liberarDireccion(vectorAL,dir_proceso_borrar);
        }else if(tam_proceso>0&&tam_proceso<=16){
            if(memoriaDisponible(vectorAL)>tam_proceso){
                direccion_almacen = solicitarDirMemoria(tam_proceso,vectorAL,0);
                if(direccion_almacen == -1){
                    dividirProceso(vectorAL,proceso_actual,&cola_procesos);
                }else{
                    asignarProceso(proceso_actual,vectorAL,direccion_almacen);    
                }
                
            }else{
                printf("No hay suficiente memoria para el proceso %d", proceso_actual.id_proceso);
                //encolar(&cola_procesos,proceso_actual); Posible bucle
            }
        }else{
            printf("El proceso %d tiene un tamaño invalido.\n",proceso_actual.id_proceso);
        }
        
        while(unirMemoria(vectorAL));
        imprimirMemoria(vectorAL);
        imprimirVector(vectorAL);
    }
    
    liberarCola(&cola_procesos);
    cerrarVectorAL(vectorAL);
    return 0;
}

/// @brief Crea el vector de areas libres y devuelve su dirección
Lista** inicializarVectorAL(){
    int i=0;
    Proceso libre;
    libre.id_proceso = 0;
    libre.tam = -1;
    Lista** vectorAL = (Lista**)malloc(5 * sizeof(Lista));
    for(i=0;i<5;i++){
        vectorAL[i] = crearLista();
    }
    agregarFinal(vectorAL[4],libre,0);  //El elemento 4 está inicializado con el "proceso" com tamaño -1
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
    if (lista.longitud)
        return 0;
    else
        return 1;
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
    printf("╔═════╗\n");
    imprimirRenglon(0,lista[0]);
    printf("╠═════╣\n");
    imprimirRenglon(1,lista[1]);
    printf("╠═════╣\n");
    imprimirRenglon(2,lista[2]);
    printf("╠═════╣\n");
    imprimirRenglon(3,lista[3]);
    printf("╠═════╣\n");
    imprimirRenglon(4,lista[4]);
    printf("╚═════╝\n");

}

void imprimirRenglon(int numero,Lista *lista){
    printf("║  %d  ║",numero);

    Nodo* nodoActual = lista->inicio;
    while (nodoActual != NULL) {
        if(nodoActual->proc.tam > 0)
            printf(" -> [Dir: %d, Proc: %d]", nodoActual->dir,nodoActual->proc.id_proceso);
        else
            printf(" -> [Dir: %d, Libre]", nodoActual->dir,nodoActual->proc.id_proceso);
        nodoActual = nodoActual->siguiente;
    }

    printf("\n");  
}

int memoriaDisponible(Lista** vectorAL){
    int memoriaDisponible=0;
    int i;
    for(i=0;i<5;i++){
        Lista listaActual =*vectorAL[i];
        //imprimirRenglon(i,&listaActual);
        if(listaEstaVacia(listaActual)) //Si la lista del slot del vector está vacía, sigue
            continue;
        else{
            Nodo* nodoActual = listaActual.inicio;
            while (nodoActual != NULL) {
                if(nodoActual->proc.tam == -1) //Encontró un proceso libre
                    memoriaDisponible += pow(2,i);
                nodoActual = nodoActual->siguiente;
            }
        }
    }
    //printf("Memoria disponible: %d\n", memoriaDisponible);
    return memoriaDisponible;
}



/// @brief Divide el segmento del vectorAL con el indice dado en 2, debe incluir la direccion de memoria.
/// @param vectorAL El vectorAL que se va a dividir
/// @param indice Indica que segmento de memoria de tamañano 2^indice se divde (2,4,8,16)
/// @param direccion Es la direccion del segmento que se va a dividir
/// @return Regresa 1 si el proceso fue exitoso, cero si ocurrió un error
int dividirMemoria(Lista** vectorAL,int direccion){ 
    if(direccion<0||direccion>15){
        printf("Dirección no válida.\n");
        return 0;
    }

    //Primero verifica que ese segmento exista en la dirección indicada
    int indice = -1;
    int i;
    int posicion = 0;
    int tam_pag;
    int indice_div;
    Lista * listaActual;
    Lista * listaNueva;

    for(i=0;i<5;i++){ //Recorre todo el vector AL
        Lista listaTemp= *vectorAL[i];
        if(listaEstaVacia(listaTemp)) //Si la lista del slot del vector está vacía, sigue
            continue;
        else{
            Nodo* nodoActual = listaTemp.inicio;
            while (nodoActual != NULL) {
                if(nodoActual->dir == direccion){ //Encontró el nodo
                    indice = i;
                    tam_pag = pow(2,indice);
                    listaActual = vectorAL[indice];
                    break;
                }
                nodoActual = nodoActual->siguiente;
            }
        }
    }

    if(indice == -1){
        printf("Esa direccion de memoria pertenece a un segmento.\n");
        return 0;
    }else if(indice == 0){
        printf("No se puede dividir más la memoria.\n");
        return 0;
    }else if(!(direccion%tam_pag==0)){ //No es dirección valida
        printf("No es una dirección divisible valida de memoria.\n", direccion%tam_pag);
        return 0;
    }else{
        indice_div = pow(2,indice-1);
        listaNueva = vectorAL[indice-1];
    }
    
    Nodo* nodoActual = listaActual->inicio;
    while (nodoActual != NULL) {
        if(nodoActual->dir == direccion){ //Encontró el segmento que se quiere dividir
            if(nodoActual->proc.tam==-1){ //Es un segmento, no un proceso
                borrarElemento(listaActual,posicion);
                Proceso libreA, libreB;
                libreA.id_proceso = 0;
                libreA.tam = -1;
                libreB.id_proceso = 0;
                libreB.tam = -1;
                agregarFinal(listaNueva,libreA,direccion);
                agregarFinal(listaNueva,libreB,direccion+indice_div);
                return 1;
            }else{
                printf("Este marco está ocupado por el proceso %d de tamaño %d. No se puede dividir.\n",nodoActual->proc.id_proceso, nodoActual->proc.tam);
                return 0;
            }
        }
        nodoActual = nodoActual->siguiente;
        posicion++;
    }
    printf("Este segmento de memoria no se puede dividir como se solicita.\n");
    return 0;

}

void imprimirMemoria(Lista ** vectorAL){
    printf("Dir    Memoria real\n");
    printf("0 \t╔═══════╗\n");
    int renglon = 0,i,j;
    for(renglon = 0; renglon<16; renglon++){
        for(i=0;i<5;i++){
            Lista listaActual =*vectorAL[i];
            if(listaEstaVacia(listaActual)) //Si la lista del slot del vector está vacía, sigue
                continue;
            else{
                Nodo* nodoActual = listaActual.inicio;
                while (nodoActual != NULL) {
                    if(nodoActual->dir == renglon){
                        for(j=0;j<(pow(2,i));j++){
                            if (nodoActual->proc.tam==-1)
                                printf("\t║       ║\n");
                            else
                                printf("\t║   X   ║  %d\n", nodoActual->proc.id_proceso);
                        }
                        if(renglon+pow(2,i)==16)
                            printf("\t╚═══════╝\n");
                        else
                            printf("%d\t╠═══════╣\n",renglon+ j);
                    }
                    nodoActual = nodoActual->siguiente;
                }
            }
        }
    }
}

/// @brief Consulta la memoria y la divida en caso de ser necesario
/// @param tam El tamaño de memoria solicitado
/// @param vectorAL El vector de areas libres
/// @param divisiones Debe ser 0
/// @return 
int solicitarDirMemoria(int tam, Lista ** vectorAL,int divisiones){
    int indice_segmento = ceil(log2(tam));
    int i,j, dir=-1;
    Lista listaTemp;
    Nodo* nodoActual;

    if(indice_segmento >  4){
        return -1;
    }

    //for(i = indice_segmento; i<5;i++){
    listaTemp = *vectorAL[indice_segmento];
    if (listaEstaVacia(listaTemp)){ //Si no hay segmentos del tamaño requerido
        divisiones++;
        dir = solicitarDirMemoria(2*pow(2,indice_segmento),vectorAL,divisiones);
    }else{ //La lista está llena
        nodoActual = listaTemp.inicio;
        while (nodoActual != NULL) {
            if(nodoActual->proc.tam == -1){ //Encuentra un segmento disponible
                while(divisiones){
                    dividirMemoria(vectorAL,nodoActual->dir);
                    divisiones--;
                }
                return nodoActual->dir;
            }
            nodoActual = nodoActual->siguiente;
        }
        if(dir ==-1){//Ningun elemento de la lista está libre
            divisiones++;
            dir = solicitarDirMemoria(2*pow(2,indice_segmento),vectorAL,divisiones);
        }
    }
    return dir;
}

int asignarProceso(Proceso p, Lista ** vectorAL, int direccion){
    int i;
    if(direccion==-1){
        printf("No hay memoria disponible.\n");
        return -1;
    }else{
        for(i=0;i<5;i++){ //Recorre todo el vector AL
        Lista listaTemp= *vectorAL[i];
            if(listaEstaVacia(listaTemp)) //Si la lista del slot del vector está vacía, sigue
                continue;
            else{
                Nodo* nodoActual = listaTemp.inicio;
                while (nodoActual != NULL) {
                    if(nodoActual->dir == direccion){ //Encontró el nodo
                        nodoActual->proc.id_proceso = p.id_proceso;
                        nodoActual->proc.tam = p.tam;
                        return 1;
                    }
                    nodoActual = nodoActual->siguiente;
                }
            }
        }
    }
    
    printf("No se encontró la direccion de memoria.\n");
    return -1;
}

int liberarDireccion(Lista ** vectorAL, int direccion){
    int i;
    if(direccion==-1){
        printf("La dirección no es válida.\n");
        return -1;
    }else{
        for(i=0;i<5;i++){ //Recorre todo el vector AL
        Lista listaTemp= *vectorAL[i];
            if(listaEstaVacia(listaTemp)) //Si la lista del slot del vector está vacía, sigue
                continue;
            else{
                Nodo* nodoActual = listaTemp.inicio;
                while (nodoActual != NULL) {
                    if(nodoActual->dir == direccion){ //Encontró el nodo
                        nodoActual->proc.id_proceso = 0;
                        nodoActual->proc.tam = -1;
                        return 1;
                    }
                    nodoActual = nodoActual->siguiente;
                }
            }
        }
    }
}

int buscarPorceso(Proceso p, Lista ** vectorAL){
    int i;
    
    for(i=0;i<5;i++){ //Recorre todo el vector AL
    Lista listaTemp= *vectorAL[i];
        if(listaEstaVacia(listaTemp)) //Si la lista del slot del vector está vacía, sigue
            continue;
        else{
            Nodo* nodoActual = listaTemp.inicio;
            while (nodoActual != NULL) {
                if(nodoActual->proc.id_proceso == p.id_proceso){ //Encontró el nodo
                    return nodoActual->dir;
                }
                nodoActual = nodoActual->siguiente;
            }
        }
    }
    printf("No se encontró el proceso.\n");
    
}

int unirMemoria(Lista** vectorAL){ 
    int indice = -1;
    int i,j=0;
    int posicionA=0, posicionB=0;
    int dir=-1,dir2;
    Lista * listaActual;
    Lista * listaSiguiente;
    Nodo * nodoActual;
    Nodo * nodoNuevo;
    

    for(i=0;i<4;i++){ //Recorre todo el vector AL menos el último elemento (de 14)
        posicionA = 0;
        listaActual= vectorAL[i];
        if(listaEstaVacia(*listaActual)) //Si la lista del slot del vector está vacía, sigue
            continue;
        else{
            nodoActual = listaActual->inicio;
            while (nodoActual != NULL) {
                if(nodoActual->proc.tam == -1){ //Encuentra un nodo libre
                    dir=nodoActual->dir;
                    break;
                }
                nodoActual = nodoActual->siguiente;
                posicionA++;
            }
        }

        if(dir!=-1){
            dir2 = dir+pow(2,i);
            if(dir2%(int)pow(2,i)==0){ //Verifica que sean segmentos que sí se puedan unir
                nodoActual = listaActual->inicio;
                while (nodoActual != NULL) {
                    if(nodoActual->proc.tam == -1 && nodoActual->dir == dir2){ //Encontró que el nodo adyacente está libre
                    imprimirMemoria(vectorAL);
                    imprimirVector(vectorAL);
                    printf("Se unen los segmentos con direcciones %d y %d.\n",dir,dir2);
                    listaSiguiente = vectorAL[i+1];
                    Proceso p;
                    p.id_proceso = 0;
                    p.tam = -1;
                    //Unir memoria
                    borrarElemento(listaActual,posicionA);
                    borrarElemento(listaActual,posicionB-1);
                    agregarFinal(listaSiguiente,p,dir);
                    return 1;
                    }
                    nodoActual = nodoActual->siguiente;
                    posicionB++;
                }
            }
        }

    }
    return 0;
}

int dividirProceso(Lista** vectorAL, Proceso P, Cola * cola_procesos){
    int tam = P.tam;
    int mitad = tam/2;
    if(mitad>1){
        Proceso A;
        Proceso B;

        A.id_proceso = P.id_proceso;
        B.id_proceso = P.id_proceso;
        A.tam = mitad;
        B.tam = mitad;
        colocarAlFrente(cola_procesos,A);
        colocarAlFrente(cola_procesos,B);
    }else{
        //No se puede dividir más el proceso+
        return -1;
    }
}

void colocarAlFrente(Cola* cola, Proceso proc) {
    NodoProc* nuevoNodo = (NodoProc*)malloc(sizeof(NodoProc));
    nuevoNodo->proc = proc;
    nuevoNodo->siguiente = cola->frente;
    cola->frente = nuevoNodo;

    if (cola->final == NULL) {
        cola->final = nuevoNodo;
    }
}
