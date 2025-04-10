/*
Alejandro Masmiquel Casado
Carlos López Mihi
*****
*/
#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{

    // Verificar que el camino comienza con '/'
    if (camino[0] != '/')
    {
        return -1; // error: no comienza con '/'
    }

    const char *segundo_slash = strchr(camino + 1, '/'); // buscamos el segundo '/'

    if (segundo_slash != NULL)
    {
        // Hay al menos dos '/': es un directorio
        size_t len = segundo_slash - (camino + 1); // longitud del nombre entre los dos '/'
        strncpy(inicial, camino + 1, len);
        inicial[len] = '\0'; // terminamos la cadena

        strcpy(final, segundo_slash); // copiamos el resto del camino incluyendo '/'
        *tipo = 'd';
    }
    else
    {
        // Solo hay un '/': es un fichero
        strcpy(inicial, camino + 1); // copiamos desde después del primer '/'
        final[0] = '\0';             // cadena vacía
        *tipo = 'f';
    }

    return 0; // éxito
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada,
                   char reservar, unsigned char permisos)
{
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    struct superbloque SB;
    bread(posSB,&SB);

    // Caso base: camino es raíz "/"
    if (strcmp(camino_parcial, "/") == 0)
    {
        *p_inodo = SB.posInodoRaiz;
        *p_entrada = 0;
        return 0;
    }

    // Extraer la siguiente parte del camino
    if (extraer_camino(camino_parcial, inicial, final, &tipo) == -1)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

    // Leer el inodo del directorio actual
    if (leer_inodo(*p_inodo_dir, &inodo_dir) == -1)
        return -1;

    if (!(inodo_dir.permisos & 4))
        return ERROR_PERMISO_LECTURA; // r = 4

    int cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    int num_entrada_inodo = 0;
    int offset = 0;
    struct entrada buffer[BLOCKSIZE / sizeof(struct entrada)];

    int entrada_encontrada = 0;

    while (offset < inodo_dir.tamEnBytesLog && !entrada_encontrada)
    {
        int leidos = mi_read_f(*p_inodo_dir, buffer, offset, sizeof(buffer));
        if (leidos < 0)
            return leidos;

        int nentradas = leidos / sizeof(struct entrada);
        for (int i = 0; i < nentradas; i++)
        {
            if (strcmp(buffer[i].nombre, inicial) == 0)
            {
                entrada_encontrada = 1;
                *p_inodo = buffer[i].ninodo;
                *p_entrada = num_entrada_inodo;
                break;
            }
            num_entrada_inodo++;
        }
        offset += leidos;
    }

    // Si no se encontró la entrada
    if (!entrada_encontrada)
    {
        if (!reservar)
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;

        if (inodo_dir.tipo != 'd')
            return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
        if (!(inodo_dir.permisos & 2))
            return ERROR_PERMISO_ESCRITURA; // w = 2

        struct entrada nueva;
        memset(&nueva, 0, sizeof(struct entrada));
        strncpy(nueva.nombre, inicial, sizeof(nueva.nombre) - 1);

        if (tipo == 'd')
        {
            if (strcmp(final, "/") == 0)
            {
                nueva.ninodo = reservar_inodo('d', permisos);
            }
            else
            {
                return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
            }
        }
        else
        {
            nueva.ninodo = reservar_inodo('f', permisos);
        }

        if (nueva.ninodo == -1)
            return -1;

        int escritos = mi_write_f(*p_inodo_dir, &nueva,
                                  inodo_dir.tamEnBytesLog, sizeof(struct entrada));
        if (escritos < sizeof(struct entrada))
        {
            liberar_inodo(nueva.ninodo);
            return -1;
        }

        *p_inodo = nueva.ninodo;
        *p_entrada = cant_entradas_inodo;
    }

    // ¿Es el final del camino?
    if (strcmp(final, "/") == 0 || strlen(final) == 0)
    {
        return 0;
    }

    return buscar_entrada(final, p_inodo, p_inodo, p_entrada, reservar, permisos);
}

void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -2:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -3:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -6:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -7:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -8:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    }
}
