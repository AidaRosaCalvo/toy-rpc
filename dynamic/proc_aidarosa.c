#include "../rpc.h"
#include <stdlib.h>
#include <stdio.h>

/*
Autora: Aida Rosa Calvo Fleites
*/
typedef struct
{
    reportable_t parent;

    char cadena1 [25]; // primera cadena de argumento
    char cadena2 [25]; // segunda cadena de argumento
    int result; // variable en la que se almacena el resultado
} my_struct_t;

/*En esta Función se realiza la carga de los parámetros de entrada*/
void *parse_parameters(void *data)
{
    const char *buf = (const char *)(data);

    my_struct_t *d = (my_struct_t *)(malloc(sizeof(my_struct_t)));

    if (d)
    {   //Se leen las cadenas de entrada
        sscanf(buf, "%s %s", d->cadena1, d->cadena2);
    }

    return (void *)d;
}

/*En en esta función lo que se hace es determinar si dado dos cadenas, una de ellas es subcadena
de la otra. Para ello primero se determina por el tamaño de las cadenas cual es el patrón y cual 
es la cadena de análisis. En caso de cumplirse que una sea subcadena de la otra se guarda el 
result = 1 y en caso de que no lo sea result = 0.

Este algoritmo es basado en el algoritmo de Knuth Morris Pratt
*/
void *do_work(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);
    
    char* patron; // aquí se guardará el patrón
    char* cadena; // aquí se guardará la cadena

    if(strlen(d->cadena1) >= strlen(d->cadena2)){
        cadena = d->cadena1;
        patron = d->cadena2;
    }else{
        patron = d->cadena1;
        cadena = d->cadena2;
    }

    ///Procesamiento KMP///
    int* fail = (int*)malloc(strlen(patron) * sizeof(int));  // Arreglo de fallos para el patrón
    fail[0] = 0;
    for (int i = 1, st; i < strlen(patron); i++) {
        st = fail[i - 1];

        // Calcula la función de fallos para el patrón 
        while (st > 0 && patron[st] != patron[i])
            st = fail[st - 1];

        if (patron[st] == patron[i])
            fail[i] = st + 1;
    }

    for (int i = 0, st = 0; i < strlen(cadena); i++) {
        // Realiza la comparación de caracteres mientras avanza en la cadena
        while (st > 0 && patron[st] != cadena[i])
            st = fail[st - 1];
        if (patron[st] == cadena[i])
            st++;
        if (st == strlen(patron)) {
            // Se encontró una coincidencia completa
            d->result = 1;
            free(fail);  // Liberar la memoria del arreglo de fallos
            return data;
        }
    }

    //No se encontraron coincidencias
    d->result = 0;
    free(fail);  // Liberar la memoria del arreglo de fallos
    return data;
}

/*En esta función se genera el resultado en consola a partir de los datos procesados anteriormente*/
reportable_t *report(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);
    
    d->parent.data = (char *)(malloc(255 * sizeof(char)));
    
    //Decidir cuál es mayor
    char* patron; // aquí se guardará el patrón
    char* cadena; // aquí se guardará la cadena

    if(strlen(d->cadena1) >= strlen(d->cadena2)){
        cadena = d->cadena1;
        patron = d->cadena2;
    }else{
        patron = d->cadena1;
        cadena = d->cadena2;
    }

    //Contenido a imprimir en consola
    snprintf(d->parent.data, 255, "El patrón: '%s' %s de %s\n", patron, 
    d->result?"es subcadena":"no es subcadena", cadena);
    
    d->parent.len = strlen(d->parent.data);

    return (reportable_t *)(data);
}

void clean_up(void *params, void *result, reportable_t *report)
{
    if (report && report->data)
    {
        free(report->data);
    }

    if (params)
    {
        free(params);
    }
}