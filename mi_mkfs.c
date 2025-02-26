/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/

#include "fichero_basico.h"

int main(int argc, char **argv){

  
  printf("%lu", sizeof( struct superbloque));


    //obtenemos el número de bloques //
    int nbloques = atoi(argv[2]);

    //obtenemos el número de inodos//
    int ninodos = nbloques/4;

    //inicializamos el buffer que utilizaremos para escribir//
    unsigned char buff[BLOCKSIZE];
    memset(buff, 0, BLOCKSIZE);

    //montamos nuestro disco// 
    bmount(argv[1]);

    //Inicializamos los bloques en el disco virtual//
    for (size_t i = 0; i < nbloques; i++)
    {
        bwrite(i,buff);
    }
  
    //inicializamos el superbloque//
    initSB(nbloques,ninodos);

    //Desmontamos el disco//
    bumount();
    return 0;
}