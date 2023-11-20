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
#define SIZEOF_JUGADORES sizeof(jugadores) * JUGADORES_CANT
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

    if(munmap(jugadores, SIZEOF_JUGADORES) == -1)
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

void ataque_espada(struct jugador *atacante, struct jugador *enemigo) {
    int ataque = lanzar_dado(ATAQUE_ESPADA_DADO);
    enemigo->salud -= ataque;
    atacante->ataco = ATAQUE_ESPADA_ATACO;
    assert(strlen(ATAQUE_ESPADA_MENSAJE) <= JUGADOR_MENSAJE_SIZE);
    sprintf(
        atacante->mensaje,
        ATAQUE_ESPADA_MENSAJE,
        ATAQUE_ESPADA_DADO,
        ataque,
        atacante->salud
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
        assert(strlen(ATAQUE_MAZA_MENSAJE) + ataque <= JUGADOR_MENSAJE_SIZE);
        sprintf(
            atacante->mensaje,
            ATAQUE_MAZA_MENSAJE,
            ATAQUE_MAZA_DADO,
            ataque,
            atacante->energia,
            atacante->salud
        );
    } else {
        atacante->energia = 0;
        assert(strlen(ATAQUE_MAZA_FALLIDO_MENSAJE) <= JUGADOR_MENSAJE_SIZE);
        sprintf(
            atacante->mensaje,
            ATAQUE_MAZA_FALLIDO_MENSAJE,
            ATAQUE_MAZA_DADO,
            ataque,
            atacante->energia,
            atacante->salud
        );
    }

    if (atacante->energia == 0) {
        cooldown = JUGADOR_ENERGIA_COOLDOWN;
    }

    atacante->ataco = ATAQUE_MAZA_ATACO;
    return cooldown;
}

void ataque_flecha(struct jugador *atacante, struct jugador *enemigo) {
    int flecha = lanzar_dado(ATAQUE_FLECHA_DADO);
    int ataque;
    int energia;

    if (flecha == ATAQUE_FLECHA_BLANCO) {
        energia = atacante->energia;
        atacante->energia = MIN(energia + ATAQUE_FLECHA_PODER, JUGADOR_ENERGIA_MAX);
        ataque = ATAQUE_FLECHA_PODER;
        enemigo->salud -= ataque;
        assert(strlen(ATAQUE_FLECHA_MENSAJE) + ataque <= JUGADOR_MENSAJE_SIZE);
        sprintf(
            atacante->mensaje,
            ATAQUE_FLECHA_MENSAJE,
            ATAQUE_FLECHA_DADO,
            flecha,
            ataque,
            atacante->salud
        );
    } else {
        assert(strlen(ATAQUE_FLECHA_FALLIDO_MENSAJE) <= JUGADOR_MENSAJE_SIZE);
        sprintf(
            atacante->mensaje,
            ATAQUE_FLECHA_FALLIDO_MENSAJE,
            ATAQUE_FLECHA_DADO,
            flecha,
            atacante->salud
        );
    }

    atacante->ataco = ATAQUE_FLECHA_ATACO;
}

void finalizar_con_exito() {
    destruir();
    printf("\n%s%s%s\n", AZUL, EXIT_SUCCESS_MSJ, NORMAL);
    exit(EXIT_SUCCESS);
}

void handle_finalizar_anormalmente() {
    if (!critical_section) {
        finalizar_con_exito();
    }
}

void log_bitacora(char *file_name, char *msg) {
    FILE *file = fopen(file_name, "a+"); /* abrir el archivo para append */
    if (!file)
        err_exit("fopen (log_bitácora)");
    fprintf(file, msg);
    if (fclose(file) == EOF)
        err_exit("fclose (log_bitácora)");
}

/* devuelve */
void formatear_mensaje(struct jugador *jgdor, char *mensaje) {
    sprintf(
        mensaje,
        JUGADOR_MENSAJE_FORMATO,
        jgdor->nombre,
        jgdor->mensaje
    );
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

    while (!salir) {
        print_menu(&JUGADOR, &MOUNSTRO, cooldown);
        scanf("%i", &opcion);

        /* esperar por confirmación del servidor para modificar la memoria compartida */
        sem_wait(sem_user);
        critical_section = true;

        /* gameover */
        if(JUGADOR.salud <= 0) {
            assert(strlen(JUGADOR_MUERTO_NOMBRE) <= JUGADOR_MENSAJE_SIZE);
            strcpy(JUGADOR.nombre, JUGADOR_MUERTO_NOMBRE);
            print_gameover();
            opcion = MENU_SALIR_OPCION;
        }

        switch (opcion) {
            case MENU_ESPADA_OPCION:
                assert(MENU_ESPADA_OPCION != INVALID_KEY);
                ataque_espada(&JUGADOR, &MOUNSTRO);
                cooldown = cooldown == -1?-1:cooldown-1;
                break;
            case MENU_MAZA_OPCION:
                assert(MENU_MAZA_OPCION != INVALID_KEY);
                new_cooldown = ataque_maza(&JUGADOR, &MOUNSTRO);
                cooldown = cooldown == -1? new_cooldown:cooldown-1;
                break;
            case MENU_FLECHA_OPCION:
                assert(MENU_FLECHA_OPCION != INVALID_KEY);
                cooldown = cooldown == -1?-1:cooldown-1;
                ataque_flecha(&JUGADOR, &MOUNSTRO);
                break;
            case MENU_SALIR_OPCION:
                assert(MENU_SALIR_OPCION != INVALID_KEY);
                salir = true;
                sem_post(sem_user);
                break;
            default:
                printf("\n%s%s%s\n", ROJO, MENU_OPCION_INCORRECTA_MSJ, NORMAL);
                sem_post(sem_user);
                opcion = INVALID_KEY;
                break;
        }

        /* si ingresó una opción correcta que no sea salir del juego */
        if (opcion != INVALID_KEY && !salir) {
            char mounstro_mensaje[LINE_MAX] = {0};
            char jugador_mensaje[LINE_MAX] = {0};

            formatear_mensaje(&MOUNSTRO, mounstro_mensaje);
            formatear_mensaje(&JUGADOR, jugador_mensaje);

            /* imprimir el último mensaje del jugador */
            printf("\n%s%s%s", AZUL, jugador_mensaje, NORMAL);

            /* interpolar el nombre del jugador en el nombre del archivo de bitácora local */
            char bitacora_local_file[FILENAME_MAX] = {0};
            sprintf(bitacora_local_file, BITACORA_LOCAL_FILE, JUGADOR.nombre);

            /* loggear en las bitácoras */
            log_bitacora(bitacora_local_file, mounstro_mensaje);
            log_bitacora(bitacora_local_file, jugador_mensaje);
            log_bitacora(BITACORA_GLOBAL_FILE, mounstro_mensaje);
            log_bitacora(BITACORA_GLOBAL_FILE, jugador_mensaje);

            /* comprobar si el mounstro sigue vivo */
            if (MOUNSTRO.salud <= 0) {
                salir = true;
                print_you_win();
            }

            /* comprobar el cooldown de energía */
            if (cooldown == 0) {
                cooldown = -1;
                JUGADOR.energia = JUGADOR_ENERGIA_MAX;
            }

            /* devolver el control al servidor */
            sem_post(sem_server);
        }

        /* salir de la sección crítica */
        critical_section = false;
    }

    finalizar_con_exito();
}
