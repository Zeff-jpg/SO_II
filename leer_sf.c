

#include "fichero_basico.h"




int main(int argc, char **argv){

    struct superbloque SB;
    int fs = open(argv[1], O_RDONLY );

     //Abrimos nuestro fichero donde se "almacena" el disco virtual//
     if(fs==FALLO){

        perror(RED "Error al abrir el disco virtual");
        printf(RESET);
        return FALLO; 

     }

     //leemos el superbloque // 
     if (read(fs, &SB,BLOCKSIZE) == FALLO)
     {
         perror(RED "Error al leer el SB");
         printf(RESET);
         return FALLO;
     }

     //imprimimos los datos del superbloque//
     printf(BLUE"****DATOS DEL SUPER BLOQUE****\n\n");
     
     printf(GRAY"posPrimerBloqueMB = %d\n",SB.posPrimerBloqueMB);
     printf("posUltimoBloqueMB = %d\n",SB.posUltimoBloqueMB);
     printf("posPrimerBloqueAI = %d\n",SB.posPrimerBloqueAI);
     printf("posUltimoBloqueAI = %d\n",SB.posUltimoBloqueAI);
     printf("posPrimerBloqueDatos = %d\n",SB.posPrimerBloqueDatos);
     printf("posUltimoBloqueDatos = %d\n",SB.posUltimoBloqueDatos);
     printf("posInodoRaiz = %d\n",SB.posInodoRaiz);

     printf("posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
     printf("cantBloquesLibres = %d\n",SB.cantBloquesLibres);
     printf("cantInodosLibres = %d\n",SB.cantInodosLibres);
     printf("totBloques = %d\n",SB.totBloques);
     printf("totInodos = %d\n\n",SB.totInodos);

     printf("sizeof struct superbloque= %ld\n",sizeof(struct superbloque));
     printf("sizeof struct inodo = %ld\n",sizeof(struct inodo));

     printf(RESET);

}


