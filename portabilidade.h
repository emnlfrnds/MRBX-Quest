#ifndef PORTABILIDADE_H
#define PORTABILIDADE_H

#define TELA_LARGURA 125
#define TELA_ALTURA  25

#ifdef _WIN32
/* --- CONFIGURAÇÃO NATIVA WINDOWS --- */
    #include <windows.h>
    #include <conio.h>
    /* Atalhos para manter a consistência de assinaturas */
    #define preparar_sistema() preparar_terminal_win()
    #define renderizar_jogo() WriteConsoleOutput(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
    #define dormir_ms(ms) Sleep(ms)
    #define ler_input() (_kbhit() ? _getch() : -1)

#else
    /* --- CONFIGURAÇÃO EMULADA LINUX / MACOS --- */
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <termios.h>
    #include <string.h>
    /* Emulação exata dos Bitmasks de Cores do Console Windows */
    #define FOREGROUND_BLUE 0x0001
    #define FOREGROUND_GREEN 0x0002
    #define FOREGROUND_RED 0x0004
    #define FOREGROUND_INTENSITY 0x0008
    #define BACKGROUND_BLUE 0x0010
    #define BACKGROUND_GREEN 0x0020
    #define BACKGROUND_RED 0x0040
    #define BACKGROUND_INTENSITY 0x0080
    /* Definição das structs nativas do Windows para compilação no Linux */
    typedef struct _CHAR_INFO {
        union {
            char AsciiChar;
        } Char;
        unsigned short Attributes;
    } CHAR_INFO;

    typedef unsigned short WORD;

    /* Mapeamento de funções para a HAL do Unix */
    #define preparar_sistema() preparar_terminal_unix()
    #define renderizar_jogo() renderizar_terminal_unix()
    #define dormir_ms(ms) usleep((ms) * 1000)
    int ler_input(void);

#endif
/* Globais do Motor Gráfico */

extern CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];

#ifdef _WIN32
    extern HANDLE hConsole;
    extern COORD bufferSize;
    extern COORD bufferCoord;
    extern SMALL_RECT consoleWriteArea;
#endif
    /* Declaração de Funções de Inicialização */
    void preparar_terminal_win(void);
    void preparar_terminal_unix(void);
    void restaurar_terminal_unix(void);
#endif