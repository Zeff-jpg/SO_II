/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "fichero_basico.h"

int bitSobrantes(int n); //Funcion que devuelve valor decimal de los bits sobrantes//

/*Función que devuelve en numero de bloques necesarios para el Mapa de Bits */
int tamMB(unsigned int nbloques)
{

    int tamMB = (nbloques / 8) / BLOCKSIZE;

    if (((nbloques / 8) % BLOCKSIZE) == 0)
    {
        return tamMB;
    }
    else
    {
        return tamMB + 1;
    }
}

/*Función que calcula el tamaño de bloques del array de inodos*/
// paso de parametros desde mi_mkfs ninodos=nbloques/4;//
int tamAI(unsigned int ninodos)
{

    int tamAI = (ninodos * INODOSIZE) / BLOCKSIZE;
    if (((ninodos * INODOSIZE) % BLOCKSIZE) == 0)
    {
        return tamAI;
    }
    else
    {
        return tamAI + 1;
    }
}


/*Función para inicializar el super bloque*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{

    struct superbloque SB;

    // Posiciones del Mapa de bits //
    SB.posPrimerBloqueMB = 0 + 1; // posSB = 0 posMB = 1

    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;

    // Posiciones del array de inodos//
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

    // Posiciones del bloque de datos//
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

    SB.posUltimoBloqueDatos = nbloques - 1;

    // posiciones de inodos//
    SB.posInodoRaiz = 0;

    SB.posPrimerInodoLibre = 0;

    // Cantidad de bloques e inodos libres  inicialmente//

    SB.cantBloquesLibres = nbloques;

    SB.cantInodosLibres = ninodos;

    // Cantidad totales de inodos y bloques //

    SB.totBloques = nbloques;

    SB.totInodos = ninodos;

    // Escribimos el superbloque en el disco virtual//
    // se puede escribir de esta manera ya que el super bloque ocupa 1 bloque//
    if (bwrite(posSB, &SB) == FALLO)
    {
        perror(RED "Error al escribir el SB");
        printf(RESET);
        return FALLO;
    };
    

    return 0;
}



int initMB()
{
    // Buffer para escribir a 1 los bloques ocupados por la inicialización//
    unsigned char bufferMB[BLOCKSIZE];

    struct superbloque SB;

    // leemos el superbloque para obtener los bloques e inodos totales //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el SB");
        printf(RESET);
        return FALLO;
    }

    // obtenemos los bloques que ocupan los metadatos//
    int bloquesMetadatos = tamMB(SB.totBloques) + tamAI(SB.totInodos) + tamSB;

    // bloques completos del mapa de bits a poner a 1 //
    int bloques1 = bloquesMetadatos / 8 / 1024;

    // cantidad de bytes a poner a 1 //
    int bytes1 = bloquesMetadatos / 8;

    // se requiere poner todo a 1 mas de 1 bloque//
    if (bloques1 != 0)
    {
        // preparamos el buffer
        memset(bufferMB, 255, BLOCKSIZE);

        // posicion de los bloques a poner a 1//
        int posBloquesMB = SB.posPrimerBloqueMB;

        // cantidad de bytes a poner a 1 del bloque incompleto //
        int bytes1 = (bloquesMetadatos % 8);

        // ponemos a 1 todos los bloques //
        for (int i = 0; i < bloques1; i++)
        {
            if (bwrite(posBloquesMB++, bufferMB) == FALLO)
            {
                perror(RED "Error al inicializar bloques ocupados en el MB");
                printf(RESET);
                return FALLO;
            }

           
        }
         // limpiamos el buffer//
        memset(bufferMB, 0, BLOCKSIZE);

        // preparamos el buffer con los bytes necesarios a 1//
        for (int i = 0; i < bytes1; i++)
        {
            bufferMB[i] = 255;
            
        //Actualizamos los bloques libres del SB -> 8 bits/bloques por cada byte escrito//
        SB.cantBloquesLibres-=8;
        }

        if(bloquesMetadatos%8 != 0){
            bufferMB[bytes1] = bitSobrantes(bloquesMetadatos%8); 

            //Actualizamos los bloques libres del SB//
            SB.cantBloquesLibres-=(bloquesMetadatos%8); 
        }

        // ponemos a 1 los bytes del bloque incompleto//
        if (bwrite(posBloquesMB, bufferMB) == FALLO)
        {
            perror(RED "Error al inicializar bloques ocupados en el MB");
            printf(RESET);
            return FALLO;
        }
        

    }
    else
    { // No llega a 1 bloque del mapa de bits

         // limpiamos el buffer//
         memset(bufferMB, 0, BLOCKSIZE);

         // preparamos el buffer con los bytes necesarios a 1//
         for (int i = 0; i < bytes1; i++)
         {
             bufferMB[i] = 255;

            //Actualizamos los bloques libres del SB -> 8 bits/bloques por cada byte escrito//
            SB.cantBloquesLibres-=8;
         }
 
         if(bloquesMetadatos%8 != 0){
             bufferMB[bytes1] = bitSobrantes(bloquesMetadatos%8);

             //Actualizamos los bloques libres del SB//
            SB.cantBloquesLibres-=(bloquesMetadatos%8); 
 
         }
 
         // ponemos a 1 los bytes del bloque incompleto//
         if (bwrite(SB.posPrimerBloqueMB, bufferMB) == FALLO)
         {
             perror(RED "Error al inicializar bloques ocupados en el MB");
             printf(RESET);
             return FALLO;
         }
        
    }

    
 if (bwrite(posSB, &SB) == FALLO)
         {
             perror(RED "Error al actualuzar datos del SB");
             printf(RESET);
             return FALLO;
         }
 return 0;
}

/*Funcion para inicializar el array de inodos*/
int initAI()
{

    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    struct superbloque SB;
    int contInodos;

    // leemos el superbloque //
    if (bread(posSB, &SB) == FALLO)
    {
        perror(RED "Error al leer el SB");
        printf(RESET);
        return FALLO;
    }

    contInodos = SB.posPrimerInodoLibre + 1; // si hemos inicializado SB.posPrimerInodoLibre = 0

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    { // para cada bloque del AI
        // leer el bloque de inodos i en el dispositivo virtual
        if (bread(i, &inodos) == FALLO)
        {
            perror(RED "Error al leer el SB");
            printf(RESET);
            return FALLO;
        }

        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {                         // para cada inodo del bloque
            inodos[j].tipo = 'l'; // libre
            if (contInodos < SB.totInodos)
            {                                               // si no hemos llegado al último inodo del AI
                inodos[j].punterosDirectos[0] = contInodos; // enlazamos con el siguiente
                contInodos++;
            }
            else
            { // hemos llegado al último inodo
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
                // hay que salir del bucle, el último bloque no tiene por qué estar completo !!!
            }
        }

        // escribimos el bloque de inodos en el disco virtual//
        if (bwrite(i, &inodos) == FALLO)
        {
            perror(RED "Error al escribir AI en el SB");
            printf(RESET);
            return FALLO;
        } 

    }
    
    return 0;
}


//Funcion que devuelve valor decimal de los bits sobrantes//
int bitSobrantes(int n){
 int sol;
    for(int i = 7; n > 0;n-- ){
        sol += 2^i;

    }
return sol; 
}