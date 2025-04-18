#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size;          //cantidad de datos/pairs en la tabla
    long capacity;      //capacidad de la tabla
    long current;       //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}

/*
2.- Implemente la función void insertMap(HashMap * map, char * key, void * value). 
Esta función inserta un nuevo dato (key,value) en el mapa y actualiza el índice current a esa posición. 

Recuerde que para insertar un par (clave,valor) debe:
a - [LISTO] Aplicar la función hash a la clave para obtener la posición donde debería insertar el nuevo par

b - [LISTO] Si la casilla se encuentra ocupada, avance hasta una casilla disponible (método de resolución de colisiones). Una casilla disponible es una casilla nula, 
    pero también una que tenga un par inválido (key==NULL).

c - [LISTO] Ingrese el par en la casilla que encontró.

[LISTO] No inserte claves repetidas. Recuerde que el arreglo es circular. Recuerde actualizar la variable size.
*/

void insertMap(HashMap * map, char * key, void * value) {
    long pos = hash(key, map->capacity);
    long originalPos = pos;

    // Si la posición pos es efectivamente NULL, este while no hará nada
    while (map->buckets[pos] != NULL) {
        // No se insertan claves duplicadas.
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            return;
        }

        pos = (pos + 1) % map->capacity;
        if (pos == originalPos) return;
    }

    // Insertar en el mapa
    Pair* newPair = (Pair*)malloc(sizeof(Pair));
    newPair->key = strdup(key);
    newPair->value = value;

    map->buckets[pos] = newPair;
    map->size += 1;
    map->current = pos;
}

/*
6.- Implemente la función void enlarge(HashMap * map). 
Esta función agranda la capacidad del arreglo buckets y reubica todos sus elementos. 

Para hacerlo es recomendable mantener referenciado el arreglo actual/antiguo de la tabla con un puntero auxiliar. 

Luego, los valores de la tabla se reinicializan con un nuevo arreglo con el doble de capacidad. 

Por último los elementos del arreglo antiguo se insertan en el mapa vacío con el método insertMap. 

Puede seguir los siguientes pasos:
a - Cree una variable auxiliar de tipo Pair** para matener el arreglo map->buckets (old_buckets);

b - Duplique el valor de la variable capacity.

c - Asigne a map->buckets un nuevo arreglo con la nueva capacidad.

d - Inicialice size a 0.

e - Inserte los elementos del arreglo old_buckets en el mapa (use la función insertMap que ya implementó).
*/

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    if (map == NULL || map->buckets == NULL) return NULL;
    long old_capacity = map->capacity;
    Pair** old_buckets = map->buckets;

    map->capacity += 2;
    map->buckets = (Pair**)calloc(map->capacity, sizeof(Pair*));
    map->size = 0;

    for (long i = 0 ; i < old_capacity ; i++){
        if (old_buckets[i] != NULL){
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]);
        }
    }
    free(old_buckets);
}

/*
Esta función crea una variable de tipo HashMap, inicializa el arreglo de buckets con casillas nulas, 
inicializa el resto de variables y retorna el mapa. Inicialice el índice current a -1.
*/

HashMap * createMap(long capacity) {
    // 1. Reservar memoria para el mapa.
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (map == NULL) return NULL;
    
    // 1.2. Inicializar las variables del mapa.
    map->capacity = capacity;
    map->current = -1;
    map->size = 0;
    
    // 2. Reservar memoria para el arreglo de buckets.
    map->buckets = (Pair**)malloc(sizeof(Pair*) * capacity);
    if (map->buckets == NULL){
        free(map);
        return NULL;
    }
    
    // 2.1. Inicializar el arreglo en NULL.
    for (int i=0; i < capacity ; i++){
        map->buckets[i] = NULL;
    }

    return map;
}

void eraseMap(HashMap * map, char * key) {
    Pair * pair = searchMap(map, key);
    if (pair == NULL) return;

    pair->key = NULL;
    map->size -= 1;
}

/*
3.- Implemente la función Pair * searchMap(HashMap * map, char * key), la cual retorna el Pair asociado a la clave ingresada. 

Recuerde que para buscar el par debe:
a - [LISTO] Usar la función hash para obtener la posición donde puede encontrarse el par con la clave

b - [LISTO] Si la clave no se encuentra avance hasta encontrarla (método de resolución de colisiones)

c - [LISTO] Si llega a una casilla nula, retorne NULL inmediatamente (no siga avanzando, la clave no está)

Recuerde actualizar el índice current a la posición encontrada. Recuerde que el arreglo es circular.
*/

Pair * searchMap(HashMap * map,  char * key) {   
    long pos = hash(key, map->capacity);
    long originalPos = pos;

    while (map->buckets[pos] != NULL){
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)){
            map->current = pos;
            return map->buckets[pos];
        }
        pos = (pos + 1) % map->capacity;

        if (pos == originalPos) break;
    }
    
    // Se topó una key=NULL o no se encontró la key.
    return NULL;
}

/*
5.- Implemente las funciones para recorrer la estructura:
    Pair * firstMap(HashMap * map) retorna el primer Pair válido del arreglo buckets. 
    Pair * nextMap(HashMap * map) retorna el siguiente Pair del arreglo buckets a partir índice current.
Recuerde actualizar el índice.
*/

Pair * firstMap(HashMap * map) {
    if (map == NULL || map->buckets == NULL) return NULL;
    for (long i = 0; i < map->capacity; i++) {
        // Buscar un elemento válido
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    if (map == NULL || map->buckets == NULL) return NULL;
    
    long i = map->current + 1;
    if (i == map->capacity) i = 0; 
    long start = i;

    while (1) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
        i += 1;
        if (i == start ||  i == map->capacity) break;
    }

    return NULL;
}

