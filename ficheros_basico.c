/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "fichero_basico.h"
/*Función que devuelve en numero de bloques necesarios para el Mapa de Bits */
int tamMB(unsigned int nbloques){

    int tamMB = (nbloques/ 8)/BLOCKSIZE;
    
    if(((nbloques/ 8) % BLOCKSIZE)== 0){
        return tamMB;
    } else {
        return tamMB+1;
    }

}


/*Función que calcula el tamaño de bloques del array de inodos*/
//paso de parametros desde mi_mkfs ninodos=nbloques/4;//
int tamAI(unsigned int ninodos){

    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if(((ninodos * INODOSIZE) % BLOCKSIZE) == 0 ){
        return tamAI;
    } else {
        return tamAI+1;
    }
   
}

/*Función para inicializar el super bloque*/
int initSB(unsigned int nbloques, unsigned int ninodos){

struct superbloque SB;

//Posiciones del Mapa de bits //
SB.posPrimerBloqueMB = 0+1; // posSB = 0 posMB = 1

SB.posUltimoBloqueMB = SB.posPrimerBloqueMB+tamMB(nbloques)-1;

//Posiciones del array de inodos//
SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

//Posiciones del bloque de datos//
SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

SB.posUltimoBloqueDatos = nbloques-1;

//posiciones de inodos//
SB.posInodoRaiz = 0;

SB.posPrimerInodoLibre = 0;

//Cantidad de bloques e inodos libres  inicialmente//

SB.cantBloquesLibres = nbloques;

SB.cantInodosLibres = ninodos;

//Cantidad totales de inodos y bloques // 

SB.totBloques = nbloques;

SB.totInodos = ninodos;

//Escribimos el superbloque en el disco virtual//
//se puede escribir de esta manera ya que el super bloque ocupa 1 bloque//
bwrite(posSB,&SB);
}
