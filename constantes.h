/* Configuración */
#define MOUNSTRO_POS                  0  /* profesor */
#define JUGADOR_POS                   5  /* alu-136148 */
#define JUGADOR_ALU                   136148
#define JUGADORES_CANT                16
#define JUGADOR_NOMBRE_SIZE           15
#define JUGADOR_MENSAJE_SIZE          100
#define JUGADOR_NICK                  "mAnu"
#define SHM_FILE                      "/soyd_memoria_compartida_alu_136148"
#define SEM_SERVER_FILE               "/soyd_semaforo_server_alu_136148"
#define SEM_USER_FILE                 "/soyd_semaforo_user_alu_136148"
#define INVALID_KEY                   -1
#define WCHAR_NULL_TERMINATED         L'\0'
#define DEBUG                         0

/* Estado */
#define ESTADO_TITULO                 "==== Estado posterior al ataque del Mounstro ===="
#define ESTADO_NOMBRE                 "Nombre       "
#define ESTADO_SALUD                  "Salud        "
#define ESTADO_ENERGIA                "Energía      "
#define ESTADO_COOLDOWN               "Cooldown     "
#define ESTADO_MSJ_MOUNSTRO           "Msj. Mounstro"

/* Menu */
#define MENU_PRINT_DELAY              0 /* tiempo en segundos que tarda en volver a imprimir la pantalla */
#define MENU_TITULO                   "============ Menu selección de ataque ==========="
#define MENU_ESPADA_OPCION            1
#define MENU_ESPADA_TEXTO             "Ataque con espada"
#define MENU_MAZA_OPCION              2
#define MENU_MAZA_TEXTO               "Ataque con maza"
#define MENU_FLECHA_OPCION            3
#define MENU_FLECHA_TEXTO             "Ataque con flecha"
#define MENU_SALIR_OPCION             4
#define MENU_SALIR_TEXTO              "Salir"
#define MENU_OPCION_INCORRECTA_MSJ    "ERROR: La opción seleccionada es incorrecta - Elija entre (1-4)!"
#define MENU_JUGADOR_MUERTO           5
#define MENU_JUGADOR_MUERTO_MSJ       "GAMEOVER: El guerrero ha quedado fuera de juego"
#define MENU_YOU_WIN_MSJ              "            Ganaste\n    El mounstro fue vencido!"
#define MENU_INGRESAR_TEXTO           "Introduzca opción (1-4): "
#define MENU_BARRA_SALUD_OK_CHAR      L'♥'
#define MENU_BARRA_SALUD_NOK_CHAR     L'♡'
#define MENU_BARRA_SALUD_MAX          10
#define MENU_BARRA_ENERGIA_OK_CHAR    L'■'
#define MENU_BARRA_ENERGIA_NOK_CHAR   L'□'
#define MENU_BARRA_ENERGIA_MAX        10
#define MENU_BARRA_COOLDOWN_OK_CHAR   L'⚡'
#define MENU_BARRA_COOLDOWN_NOK_CHAR  L' '
#define MENU_BARRA_COOLDOWN_MAX       JUGADOR_ENERGIA_COOLDOWN

#define EXIT_SUCCESS_MSJ              "La aplicación finalizó con éxito"

/* Jugador */
#define JUGADOR_MUERTO_NOMBRE         "fueradejuego"
#define JUGADOR_MUERTO_MSJ            "Quedé fuera de juego"
#define JUGADOR_GANO_MSJ              "El mounstro fue vencido! Se reinicia el juego"
#define JUGADOR_SALUD_MAX             100
#define JUGADOR_ENERGIA_MAX           25
#define JUGADOR_ENERGIA_COOLDOWN      3

/* Ataques */
#define ATAQUE_ESPADA_NOMBRE          "espada"
#define ATAQUE_ESPADA_ATACO           1
#define ATAQUE_ESPADA_FORMATO         "%s(%s) %s, D%i = %i, Salud = %i%s\n"
#define ATAQUE_ESPADA_MENSAJE         "Ataqué al mounstro con la espada!"
#define ATAQUE_ESPADA_DADO            6

#define ATAQUE_MAZA_NOMBRE            "maza"
#define ATAQUE_MAZA_ATACO             2
#define ATAQUE_MAZA_FORMATO           "%s(%s) %s, D%i = %i, Salud = %i, Energía = %i%s\n"
#define ATAQUE_MAZA_MENSAJE           "Ataqué al mounstro con la maza!"
#define ATAQUE_MAZA_FALLIDO_MENSAJE   "Fallé el ataque al mounstro con la maza!"
#define ATAQUE_MAZA_DADO              20

#define ATAQUE_FLECHA_NOMBRE          "arco"
#define ATAQUE_FLECHA_ATACO           3
#define ATAQUE_FLECHA_FORMATO         "%s(%s) %s, D%i = %i, Daño: %i, Salud: %i%s\n"
#define ATAQUE_FLECHA_MENSAJE         "Ataqué al mounstro con el arco y dí al blanco!"
#define ATAQUE_FLECHA_FALLIDO_MENSAJE "Fallé el ataque al mounstro con el arco!"
#define ATAQUE_FLECHA_DADO            5
#define ATAQUE_FLECHA_BLANCO          3
#define ATAQUE_FLECHA_PODER           10
#define ATAQUE_FLECHA_ENERGIA_BONUS   10

#define ATAQUE_NULO_NOMBRE            "nulo"

/* Bitácora */
#define BITACORA_LOCAL_FILE           "./guerrero-%s.csv"
#define BITACORA_GLOBAL_FILE          "/tmp/guerra-soyd.csv"
#define BITACORA_MENSAJE_FORMATO      "%s,%i,%i,%s,D%i = %i,%s;"

/* Colores */
#define NORMAL                        "\x1B[0m"
#define ROJO                          "\x1B[31m"
#define VERDE                         "\x1B[32m"
#define AMARILLO                      "\x1B[33m"
#define AZUL                          "\x1B[34m"

/* Splash */
/* https://emojicombos.com/ */
#define SPLASH_GAMEOVER               "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⡀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣤⠀⠀⠀⢀⣴⣿⡶⠀⣾⣿⣿⡿⠟⠛⠁\n⠀⠀⠀⠀⠀⠀⣀⣀⣄⣀⠀⠀⠀⠀⣶⣶⣦⠀⠀⠀⠀⣼⣿⣿⡇⠀⣠⣿⣿⣿⠇⣸⣿⣿⣧⣤⠀⠀⠀\n⠀⠀⢀⣴⣾⣿⡿⠿⠿⠿⠇⠀⠀⣸⣿⣿⣿⡆⠀⠀⢰⣿⣿⣿⣷⣼⣿⣿⣿⡿⢀⣿⣿⡿⠟⠛⠁⠀⠀\n⠀⣴⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⢠⣿⣿⣹⣿⣿⣿⣿⣿⣿⡏⢻⣿⣿⢿⣿⣿⠃⣼⣿⣯⣤⣴⣶⣿⡤⠀\n⣼⣿⠏⠀⣀⣠⣤⣶⣾⣷⠄⣰⣿⣿⡿⠿⠻⣿⣯⣸⣿⡿⠀⠀⠀⠁⣾⣿⡏⢠⣿⣿⠿⠛⠋⠉⠀⠀⠀\n⣿⣿⠲⢿⣿⣿⣿⣿⡿⠋⢰⣿⣿⠋⠀⠀⠀⢻⣿⣿⣿⠇⠀⠀⠀⠀⠙⠛⠀⠀⠉⠁⠀⠀⠀⠀⠀⠀⠀\n⠹⢿⣷⣶⣿⣿⠿⠋⠀⠀⠈⠙⠃⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠈⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣴⣶⣦⣤⡀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⣠⡇⢰⣶⣶⣾⡿⠷⣿⣿⣿⡟⠛⣉⣿⣿⣿⠆\n⠀⠀⠀⠀⠀⠀⢀⣤⣶⣿⣿⡎⣿⣿⣦⠀⠀⠀⢀⣤⣾⠟⢀⣿⣿⡟⣁⠀⠀⣸⣿⣿⣤⣾⣿⡿⠛⠁⠀\n⠀⠀⠀⠀⣠⣾⣿⡿⠛⠉⢿⣦⠘⣿⣿⡆⠀⢠⣾⣿⠋⠀⣼⣿⣿⣿⠿⠷⢠⣿⣿⣿⠿⢻⣿⣧⠀⠀⠀\n⠀⠀⠀⣴⣿⣿⠋⠀⠀⠀⢸⣿⣇⢹⣿⣷⣰⣿⣿⠃⠀⢠⣿⣿⢃⣀⣤⣤⣾⣿⡟⠀⠀⠀⢻⣿⣆⠀⠀\n⠀⠀⠀⣿⣿⡇⠀⠀⢀⣴⣿⣿⡟⠀⣿⣿⣿⣿⠃⠀⠀⣾⣿⣿⡿⠿⠛⢛⣿⡟⠀⠀⠀⠀⠀⠻⠿⠀⠀\n⠀⠀⠀⠹⣿⣿⣶⣾⣿⣿⣿⠟⠁⠀⠸⢿⣿⠇⠀⠀⠀⠛⠛⠁⠀⠀⠀⠀⠀⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠈⠙⠛⠛⠛⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
#define SPLASH_CUP                    "⠀⠀⠀⠀⢀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⠀⠀⠀⠀\n⢠⣤⣤⣤⣼⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣄⣤⣤⣠\n⢸⠀⡶⠶⠾⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡷⠶⠶⡆⡼\n⠈⡇⢷⠀⠀⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⠇⠀⢸⢁⡗\n⠀⢹⡘⡆⠀⢹⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡸⠀⢀⡏⡼⠀\n⠀⠀⢳⡙⣆⠈⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠇⢀⠞⡼⠁⠀\n⠀⠀⠀⠙⣌⠳⣼⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣞⡴⣫⠞⠀⠀⠀\n⠀⠀⠀⠀⠈⠓⢮⣻⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡼⣩⠞⠉⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠉⠛⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠞⠋⠁⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠳⢤⣀⠀⠀⠀⠀⢀⣠⠖⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⡇⢸⡏⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⢸⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⠖⠒⠓⠚⠓⠒⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⣀⣠⣞⣉⣉⣉⣉⣉⣉⣉⣉⣉⣉⣙⣆⣀⡀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠓⠲⠶⠶⠶⠶⠶⠶⠶⠶⠶⠶⠶⠶⠶⠖⠃⠀⠀⠀⠀⠀⠀\n"
