#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>
#include <assert.h>
#include <signal.h>
#include <limits.h>

#include "constantes.h"

/* macros de utilidad */
#define SIZEOF_JUGADORES sizeof(struct jugador) * JUGADORES_CANT
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define JUGADOR jugadores[JUGADOR_POS]
#define MOUNSTRO jugadores[MOUNSTRO_POS]

struct jugador {
    char nombre[JUGADOR_NOMBRE_SIZE];
    int  salud;
    int  energia;
    int  ataco;
    char mensaje[JUGADOR_MENSAJE_SIZE];
};

/* variables globales para los semáforos y la memoria compartida */
sem_t *sem_user;
sem_t *sem_server;
struct jugador *jugadores;
bool critical_section;

/* devuelve un entero entre 1-caras */
int lanzar_dado(int caras) {
    assert(caras > 0);
    return (rand() % caras) + 1;
}

/* libera la memoria reservada y cierra descriptores de archivos */
void destruir() {
    if (critical_section) {
        sem_post(sem_user);
        critical_section = false;
    }

    if (sem_close(sem_user) == -1)
        perror("sem_close (sem_user)");

    if (sem_close(sem_server) == -1)
        perror("sem_close (sem_server)");

    if (munmap(jugadores, SIZEOF_JUGADORES) == -1)
        perror("munmap (jugadores)");
}


/*
 * función llamada en caso de error
 * muestra el error, devuelve la memoria reservada por el programa y sale con código de error.
 */
void err_exit(char *mensaje) {
    perror(mensaje);
    destruir();
    exit(EXIT_FAILURE);
}

/*
 * llena una barra de longitud barra_len-1 wchar_t (widechar) caracteres en función de la relación entre
 * un valor entero y su máximo, usando los wchar_t ingresados.
 * barra_len tiene un caracter de más para el caracter de terminación.
 * valor_max debe ser mayor a cero.
 */
void llenar_barra(int valor, int valor_max, wchar_t *barra, int barra_len, wchar_t ok_char, wchar_t nok_char) {
    assert(valor_max > 0);
    int porcentaje = (int) round((double) valor / (double) valor_max * (barra_len-1));

    for (int i = 0; i < barra_len-1; ++i) {
        barra[i] = i < porcentaje? ok_char:nok_char;
    }

    /* insertar caracter de terminación */
    barra[barra_len-1] = WCHAR_NULL_TERMINATED;
}

/* limpiar la pantalla si no está en modo DEBUG*/
void limpiar_pantalla() {
    #if !DEBUG && defined(WINDOWS)
        system("cls");
    #elif !DEBUG
        system("clear");
    #endif
}

void print_menu(struct jugador *jgdor, struct jugador *mounstro, int cooldown) {
    wchar_t barra_salud[MENU_BARRA_SALUD_MAX+1] = {0};
    wchar_t barra_energia[MENU_BARRA_ENERGIA_MAX+1] = {0};
    wchar_t barra_cooldown[MENU_BARRA_COOLDOWN_MAX+1] = {0};

    llenar_barra(
        jgdor->salud,
        JUGADOR_SALUD_MAX,
        barra_salud,
        sizeof(barra_salud)/sizeof(wchar_t),
        MENU_BARRA_SALUD_OK_CHAR,
        MENU_BARRA_SALUD_NOK_CHAR
    );

    llenar_barra(
        jgdor->energia,
        JUGADOR_ENERGIA_MAX,
        barra_energia,
        sizeof(barra_energia)/sizeof(wchar_t),
        MENU_BARRA_ENERGIA_OK_CHAR,
        MENU_BARRA_ENERGIA_NOK_CHAR
    );

    if (cooldown > 0) {
        llenar_barra(
            cooldown,
            JUGADOR_ENERGIA_COOLDOWN,
            barra_cooldown,
            sizeof(barra_cooldown)/sizeof(wchar_t),
            MENU_BARRA_COOLDOWN_OK_CHAR,
            MENU_BARRA_COOLDOWN_NOK_CHAR
        );
    }

    printf("\n%s%s%s\n", AMARILLO, ESTADO_TITULO, NORMAL);
    printf("%s%s: %s%s\n", VERDE, ESTADO_NOMBRE, NORMAL, jgdor->nombre);
    printf("%s%s: %s%ls (%i)%s\n", VERDE, ESTADO_SALUD, ROJO, barra_salud, jgdor->salud, NORMAL);
    printf("%s%s: %s%ls (%i)%s\n", VERDE, ESTADO_ENERGIA, AMARILLO, barra_energia, jgdor->energia, NORMAL);
    if (cooldown > 0) printf("%s%s: %s%ls%s\n", VERDE, ESTADO_COOLDOWN, AMARILLO, barra_cooldown, NORMAL);
    printf("%s%s: %s%s\n", VERDE, ESTADO_MSJ_MOUNSTRO, NORMAL, mounstro->mensaje);

    printf("\n%s%s\n%s", AMARILLO, MENU_TITULO, NORMAL);
    printf("%s%i. %s%s\n", VERDE, MENU_ESPADA_OPCION, NORMAL, MENU_ESPADA_TEXTO);
    printf("%s%i. %s%s\n", VERDE, MENU_MAZA_OPCION, NORMAL, MENU_MAZA_TEXTO);
    printf("%s%i. %s%s\n", VERDE, MENU_FLECHA_OPCION, NORMAL, MENU_FLECHA_TEXTO);
    printf("%s%i. %s%s\n", VERDE, MENU_SALIR_OPCION, NORMAL, MENU_SALIR_TEXTO);

    printf("\n\n%s", MENU_INGRESAR_TEXTO);
}

void print_you_win() {
    printf("\n%s%s%s\n", AMARILLO, SPLASH_CUP, NORMAL);
    printf("\n%s%s%s\n", AMARILLO, MENU_YOU_WIN_MSJ, NORMAL);
}

void print_gameover() {
    printf("\n%s%s%s\n", ROJO, SPLASH_GAMEOVER, NORMAL);
    printf("\n%s%s%s\n", ROJO, MENU_JUGADOR_MUERTO_MSJ, NORMAL);
}

void log_ataque(struct jugador *atacante, char *ataque, int caras, int dado, int global) {
    /* obtener el nombre del arcivo con el formato guerrero-(nombre).csv */
    char bitacora_local_file[NAME_MAX] = {0};
    sprintf(bitacora_local_file, BITACORA_LOCAL_FILE, JUGADOR.nombre);

    /* formatear el mensaje */
    char mensaje_formateado[LINE_MAX] = {0};
    sprintf(
        mensaje_formateado,
        BITACORA_MENSAJE_FORMATO,
        atacante->nombre,
        atacante->salud,
        atacante->energia,
        ataque,
        caras,
        dado,
        atacante->mensaje
    );

    /* escribir en las bitácoras */
    char comando[LINE_MAX * 2] = {0};
    sprintf(comando, "echo '%s' >> '%s'", mensaje_formateado, bitacora_local_file);
    system(comando);

    if (global) {
        sprintf(comando, "echo '%s' >> '%s'", mensaje_formateado, BITACORA_GLOBAL_FILE);
        system(comando);
    }
}

void ataque_espada(struct jugador *atacante, struct jugador *enemigo) {
    int ataque = lanzar_dado(ATAQUE_ESPADA_DADO);
    enemigo->salud -= ataque;
    atacante->ataco = ATAQUE_ESPADA_ATACO;
    strcpy(atacante->mensaje, ATAQUE_ESPADA_MENSAJE);

    /* imprimir el ataque del jugador */
    printf(
        ATAQUE_ESPADA_FORMATO,
        AZUL,
        atacante->nombre,
        ATAQUE_ESPADA_MENSAJE,
        ATAQUE_ESPADA_DADO,
        ataque,
        atacante->salud,
        NORMAL
    );

    /* loggear en la bitácora */
    log_ataque(
        &JUGADOR,
        ATAQUE_ESPADA_NOMBRE,
        ATAQUE_ESPADA_DADO,
        ataque,
        1
    );
}

/*
 * en caso de que la energía haya quedado en cero despues del ataque,
 * devuelve un entero mayor a cero que representa el cooldown para recargar energía
 */
int ataque_maza(struct jugador *atacante, struct jugador *enemigo) {
    int cooldown = -1;
    int ataque = lanzar_dado(ATAQUE_MAZA_DADO);

    if (atacante->energia >= ataque) {
        enemigo->salud -= ataque;
        atacante->energia -= ataque;
        strcpy(atacante->mensaje, ATAQUE_MAZA_MENSAJE);
    } else {
        atacante->energia = 0;
        strcpy(atacante->mensaje, ATAQUE_MAZA_FALLIDO_MENSAJE);
    }

    /* imprimir el ataque del jugador */
    printf(
        ATAQUE_MAZA_FORMATO,
        AZUL,
        atacante->nombre,
        atacante->mensaje,
        ATAQUE_MAZA_DADO,
        ataque,
        atacante->salud,
        atacante->energia,
        NORMAL
    );

    if (atacante->energia == 0) {
        cooldown = JUGADOR_ENERGIA_COOLDOWN;
    }

    atacante->ataco = ATAQUE_MAZA_ATACO;

    /* loggear en la bitácora */
    log_ataque(
        &JUGADOR,
        ATAQUE_MAZA_NOMBRE,
        ATAQUE_MAZA_DADO,
        ataque,
        1
    );

    return cooldown;
}

void ataque_flecha(struct jugador *atacante, struct jugador *enemigo) {
    int flecha = lanzar_dado(ATAQUE_FLECHA_DADO);
    int ataque = 0;
    int energia;

    if (flecha == ATAQUE_FLECHA_BLANCO) {
        energia = atacante->energia;
        atacante->energia = MIN(energia + ATAQUE_FLECHA_PODER, JUGADOR_ENERGIA_MAX);
        ataque = ATAQUE_FLECHA_PODER;
        enemigo->salud -= ataque;
        strcpy(atacante->mensaje, ATAQUE_FLECHA_MENSAJE);
    } else {
        strcpy(atacante->mensaje, ATAQUE_FLECHA_FALLIDO_MENSAJE);
    }

    /* imprimir el ataque del jugador */
    printf(
        ATAQUE_FLECHA_FORMATO,
        AZUL,
        atacante->nombre,
        atacante->mensaje,
        ATAQUE_FLECHA_DADO,
        flecha,
        ataque,
        atacante->salud,
        NORMAL
    );

    atacante->ataco = ATAQUE_FLECHA_ATACO;

    /* loggear en la bitácora */
    log_ataque(
        &JUGADOR,
        ATAQUE_FLECHA_NOMBRE,
        ATAQUE_FLECHA_DADO,
        flecha,
        1
    );
}

void finalizar_con_exito() {
    destruir();
    printf("%s%s%s\n", AZUL, EXIT_SUCCESS_MSJ, NORMAL);
    exit(EXIT_SUCCESS);
}

void handle_finalizar_anormalmente() {
    if (!critical_section) {
        finalizar_con_exito();
    }
}

int main() {
    bool salir;
    int opcion;
    int cooldown;
    int new_cooldown;
    int fd;

    /* comprobar que las constantes no sean inválidas */
    assert(JUGADORES_CANT > 1);
    assert(JUGADOR_POS >= 0);
    assert(JUGADOR_POS < JUGADORES_CANT);
    assert(MOUNSTRO_POS >= 0);
    assert(MOUNSTRO_POS < JUGADORES_CANT);

    /* inicializar seed para generar números pseudo-aleatorios */
    srand(time(NULL));

    /* establece la configuración regional en tiempo de ejecución */
    setlocale(LC_ALL, "");

    /* manejar señales para la terminación del proceso */
    signal(SIGINT, handle_finalizar_anormalmente);
    signal(SIGTERM, handle_finalizar_anormalmente);
    signal(SIGABRT, handle_finalizar_anormalmente);
    signal(SIGQUIT, handle_finalizar_anormalmente);
    signal(SIGTSTP, handle_finalizar_anormalmente);

    /* agregar al espacio de direccionamiento del programa la memoria compartida */
    fd = shm_open(SHM_FILE, O_RDWR, 0600);
    if (fd == -1)
        err_exit("shm_open");

    jugadores = (struct  jugador *) mmap(
        NULL,
        SIZEOF_JUGADORES,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    if (close(fd) == -1)
        perror("close (fd)");

    if (jugadores == MAP_FAILED)
        err_exit("mmap");

    /* abrir los semáforos creados por el servidor */
    sem_user = sem_open(SEM_USER_FILE, 0);
    if (sem_user == SEM_FAILED)
        err_exit("sem_open (sem_user)");

    sem_server = sem_open(SEM_SERVER_FILE, 0);
    if (sem_server == SEM_FAILED)
        err_exit("sem_open (sem_server)");

    salir = false;
    critical_section = false; /* entrar en la sección crítica */
    cooldown = -1;

    /* cambiar el nombre de usuario si no está muerto */
    if (strcmp(JUGADOR.nombre, JUGADOR_MUERTO_NOMBRE) != 0)
        strcpy(JUGADOR.nombre, JUGADOR_NICK);

    limpiar_pantalla();

    while (!salir) {
        print_menu(&JUGADOR, &MOUNSTRO, cooldown);
        scanf("%i", &opcion);
        limpiar_pantalla();

        /* esperar por confirmación del servidor para modificar la memoria compartida */
        sem_wait(sem_user);
        critical_section = true;

        /* gameover */
        if (JUGADOR.salud <= 0) {
            assert(strlen(JUGADOR_MUERTO_NOMBRE) <= JUGADOR_MENSAJE_SIZE);

            /* si no estaba muerto antes */
            if (strcmp(JUGADOR.nombre, JUGADOR_MUERTO_NOMBRE) != 0) {
                strcpy(JUGADOR.mensaje, JUGADOR_MUERTO_MSJ);
                log_ataque(&JUGADOR, ATAQUE_NULO_NOMBRE, 0, 0, 1);
                /* setear nombre en fuera de juego (debe ejecutarse despues del log_ataque) */
                strcpy(JUGADOR.nombre, JUGADOR_MUERTO_NOMBRE);
            }

            limpiar_pantalla();
            print_gameover();
            opcion = MENU_SALIR_OPCION;
        }

        switch (opcion) {
            case MENU_ESPADA_OPCION:
                assert(MENU_ESPADA_OPCION != INVALID_KEY);
                log_ataque(&MOUNSTRO, ATAQUE_NULO_NOMBRE, 0, 0, 0);
                ataque_espada(&JUGADOR, &MOUNSTRO);
                cooldown = cooldown == -1?-1:cooldown-1;
                break;
            case MENU_MAZA_OPCION:
                assert(MENU_MAZA_OPCION != INVALID_KEY);
                log_ataque(&MOUNSTRO, ATAQUE_NULO_NOMBRE, 0, 0, 0);
                new_cooldown = ataque_maza(&JUGADOR, &MOUNSTRO);
                cooldown = cooldown == -1? new_cooldown:cooldown-1;
                break;
            case MENU_FLECHA_OPCION:
                assert(MENU_FLECHA_OPCION != INVALID_KEY);
                log_ataque(&MOUNSTRO, ATAQUE_NULO_NOMBRE, 0, 0, 0);
                cooldown = cooldown == -1?-1:cooldown-1;
                ataque_flecha(&JUGADOR, &MOUNSTRO);
                break;
            case MENU_SALIR_OPCION:
                assert(MENU_SALIR_OPCION != INVALID_KEY);
                salir = true;
                sem_post(sem_user);
                break;
            default:
                printf("%s%s%s\n", ROJO, MENU_OPCION_INCORRECTA_MSJ, NORMAL);
                sem_post(sem_user);
                opcion = INVALID_KEY;
                break;
        }

        /* si ingresó una opción correcta que no sea salir del juego */
        if (opcion != INVALID_KEY && !salir) {

            /* comprobar si el mounstro sigue vivo */
            if (MOUNSTRO.salud <= 0) {
                salir = true;
                strcpy(JUGADOR.mensaje, JUGADOR_GANO_MSJ);
                log_ataque(&JUGADOR, ATAQUE_NULO_NOMBRE, 0, 0, 0);
                limpiar_pantalla();
                print_you_win();
            }

            /* comprobar el cooldown de energía */
            if (cooldown == 0) {
                cooldown = -1;
                JUGADOR.energia = JUGADOR_ENERGIA_MAX;
            }

            sem_post(sem_server); /* devolver el control al servidor */
            sleep(MENU_PRINT_DELAY); /* esperar respuesta del servidor para volver a imprimir la pantalla*/
        }

        /* salir de la sección crítica */
        critical_section = false;
    }

    finalizar_con_exito();
}