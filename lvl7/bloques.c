/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "bloques.h"
static int descriptor = -1;

/*Función para montar el dispositivo virtual*/

int bmount(const char *camino)
{
    umask(0111);
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);

    if (descriptor == FALLO)
    {
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    };

    fchmod(descriptor, 0666);
    return descriptor;
}

/*Función para desmontar el dispositivo virtual*/
int bumount(){
 
 if(close(descriptor)==FALLO){
    perror(RED "Error");
    printf(RESET);
    return FALLO;
 };

 return EXITO;
}

/*Función para escribir en un bloque del dispositivo virtual*/
int bwrite(unsigned int nbloque, const void *buf){

    off_t desplazamiento = nbloque * BLOCKSIZE;
    /*posicionamos el puntero para escribir con lseek*/
    if(lseek(descriptor,desplazamiento,SEEK_SET)==FALLO){
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    };
    
    if(write( descriptor, buf, BLOCKSIZE)==FALLO){
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    } else{

        return BLOCKSIZE;
    };

}

/*Función para leer un bloque del dispositivo virtual*/
int bread(unsigned int nbloque, void *buf){
/*buf ha de tener el tamaño de un bloque(1024B)*/

    /*posicionamos el puntero para leer con lseek*/

    off_t desplazamiento = nbloque * BLOCKSIZE;
    if(lseek(descriptor,desplazamiento,SEEK_SET)==FALLO){
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    };

    if(read( descriptor, buf, BLOCKSIZE)==FALLO){
        perror(RED "Error");
        printf(RESET);
        return FALLO;
    } else{

        return BLOCKSIZE;
    };

}
