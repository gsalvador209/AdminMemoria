/*
Aquí empezamos el programa
*/

#include <stdio.h>
#include <stdlib.h>

/*definir estructura nodo  */

typedef struct nodo{
    char *contenido; //contiene un valor de tipo int llamado contenido
    //autoreferenciando
    struct nodo * sig; //apuntador siguiente de tipo struct nodo sirve para unir una estructura de tipo nodo con otra del mismo tipo
}Memoria;//asi se renombra a la estructura para crear nuevas variables de este tipo

/*Creacion de funciones*/
//Funcion que me devuelve un puntero de tipo Memoria
Memoria *listaMemoria(Memoria * Lista){ 
    Lista  = NULL; //Siempre que se cree la lista va aestar vacia
    return Lista; //retorna el valor de Lista
}
//Funcion que sirve para agregar elementos a la lista

Memoria *agregarEspacio(Memoria *Lista, char *contenido ){//recibe como parametro la lista de tipo MEMORIA donde se va a colocar y el valor de contenido
    Memoria *nuevoEspacio; //nuevo Espacio de memoria 
    Memoria *aux; //variable del tipo memoria llamada aux permite guardar el elemento que ya existia
    nuevoEspacio =  (Memoria*)malloc(sizeof(Memoria));//reservar lugar suficiente, sizeof el valor que reserva la memoria suficinete haciendo referencia al valor de la estructura, devuelve un puntero de tipo MEMORIA
    nuevoEspacio -> contenido = contenido;//le decimos al nuevo ezpacio que asigne el contenido a lo que traiga contenido
    nuevoEspacio->sig = NULL;//EL nuevo espacio apunta a otra estructura
    //¿En donde se va a almacenar el nuevo espacio?
    //Preguntar si la lista esta vacia
    if(Lista == NULL){
        Lista = nuevoEspacio; //si esta vacia se guarda en la primera posicion
    }else{ // si NO esta vacía se guarda en la siguiente posicion
        //variable auxiliar
        aux = Lista;
        while (aux->sig != NULL)
        {
            aux = aux->sig;
        }
        aux-> sig = nuevoEspacio;
    }
    return Lista;
}

int main(){
    //Creacion de una variable apuntador
    Memoria *Lista = listaMemoria(Lista);//se manda a llamar a la funcion listaMemoria
    //Probando funcion de agregacion
    Lista = agregarEspacio(Lista, "OCUPADO");
    Lista = agregarEspacio(Lista, "VACIO");
    
    //Mientras la lista este vacia se va a recorrer la lista
    while (Lista != NULL)
    {
        printf("%s\n",Lista->contenido); //imprimiendo lo que tenga la lista
        Lista = Lista->sig; //iterar
    }
    
    
    return 0;
}
/*Creacion de una lista ligada*/