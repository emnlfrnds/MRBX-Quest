#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Constantes da tela
#define TELA_LARGURA 125
#define TELA_ALTURA  25
#define DELAY       30
#define ALTURA_CEU   4

#ifdef _WIN32
    //windows
    #include <windows.h>
    #include <conio.h>
    
    HANDLE hConsole;
    CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];
    COORD bufferSize = {TELA_LARGURA, TELA_ALTURA};
    COORD bufferCoord = {0, 0};
    SMALL_RECT consoleWriteArea = {0, 0, TELA_LARGURA-1, TELA_ALTURA-1};

    void dormirMs(int ms) {
        Sleep(ms);
    }
#else
    //linux/macos
    #include <unistd.h>
    #include <stdio.h>

    void dormirMs(int ms) {
        usleep(ms * 1000);
    }

    // mentindo para o Unix que o WORD e o CHAR_INFO do Windows existem
    typedef unsigned short WORD;
    typedef struct {
        struct
        {
            char AsciiChar;
            
        } Char;
        WORD Attributes;
    } CHAR_INFO;

    // mentindo sobre as cores (damos valores genéricos para o seu jogo continuar usando)
    #define FOREGROUND_BLUE      1
    #define FOREGROUND_GREEN     2
    #define FOREGROUND_RED       4
    #define FOREGROUND_INTENSITY 8
    #define BACKGROUND_BLUE      16
    #define BACKGROUND_GREEN     32
    #define BACKGROUND_RED       64
    #define BACKGROUND_INTENSITY 128

    // criando o buffer falso para o Unix
    CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];
#endif

void atualizarTela() {
    #ifdef _WIN32
        WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
    #else
        printf("\033[H"); // Mova o cursor para o topo (sem piscar a tela)

        int ansiWin[8] = {0, 4, 2, 6, 1, 5, 3, 7};

        for (int i = 0; i < TELA_ALTURA; i++) {
            for (int j = 0; j < TELA_LARGURA; j++) {
                int indice = i * TELA_LARGURA + j;
                char c = consoleBuffer[indice].Char.AsciiChar;
                WORD cor = consoleBuffer[indice].Attributes;
                
                int fgWin = cor & 7; // Pega os 3 primeiros bits (Cor do texto)
                int fgIntenso = (cor & FOREGROUND_INTENSITY) ? 60 : 0; // Se for intenso, muda pra tons claros
            
                int bgWin = (cor >> 4) & 7; // Pega os bits de fundo
                int bgIntenso = (cor & BACKGROUND_INTENSITY) ? 60 : 0; // Se for intenso, muda pra tons claros
            
                int fgAnsi = 30 + ansiWin[fgWin] + fgIntenso;
                int bgAnsi = 40 + ansiWin[bgWin] + bgIntenso;
                
                printf("%c", c != 0 ? c : ' '); // Imprime o caractere
            }
            printf("\n");
        }
        fflush(stdout); // Joga tudo na tela de uma vez
    #endif
}

// Array com 6 cores vibrantes
const WORD PALETA_DE_CORES[] = {
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
}; const int TOTAL_CORES = 9;

// Telas

#define TELA_INICIAL  0
#define TELA_JOGO     1
#define TELA_GAMEOVER 2

//Constantes da tela inicial

#define LARGURA_LOGO 56
#define ALTURA_LOGO 5
#define LOGO_X 35
#define LOGO_Y 8

const char *MRBX_QUESTLOGO[ALTURA_LOGO] = {
    " __  __ ____  ______  __   ___  _   _ _____ ____ _____ ",
    "|  \\/  |  _ \\| __ ) \\/ /  / _ \\| | | | ____/ ___|_   _|",
    "| |\\/| | |_) |  _ \\\\  /  | | | | | | |  _| \\___ \\ | |  ",
    "| |  | |  _ <| |_) /  \\  | |_| | |_| | |___ ___) || |  ",
    "|_|  |_|_| \\_\\____/_/\\_\\  \\__\\_\\\\___/|_____|____/ |_|  "};

//Constantes da tela Game Over: 

#define ALTURA_GAMEOVER 8
#define LARGURA_GAMEOVER 100
#define GAME_OVER_X 13
#define GAME_OVER_Y 3

const wchar_t *GAMEOVER[] = {

    L" .d8888b.         d8888 888b     d888 8888888888      .d88888b.  888     888 8888888888 8888888b. ",
    L"d88P  Y88b       d88888 8888b   d8888 888            d88P   Y88b 888     888 888        888   Y88b",
    L"888    888      d88P888 88888b.d88888 888            888     888 888     888 888        888    888",
    L"888            d88P 888 888Y88888P888 8888888        888     888 Y88b   d88P 8888888    888   d88P",
    L"888  88888    d88P  888 888 Y888P 888 888            888     888  Y88b d88P  888        8888888P  ",
    L"888    888   d88P   888 888  Y8P  888 888            888     888   Y88o88P   888        888 T88b  ",
    L"Y88b  d88P  d8888888888 888       888 888            Y88b. .d88P    Y888P    888        888  T88b ",
    L" Y88888P8  d88P     888 888       888 8888888888      Y8888888P      Y8P     8888888888 888   T88b",

};

//Enzo Capitani: Criação do player

#define ALTURA_PLAYER              2
#define LARGURA_PLAYER             9
#define VELOCIDADE_X_PLAYER        2
#define VELOCIDADE_Y_PLAYER        1
#define TOTAL_FRAMES_JOGADOR       3
#define VELOCIDADE_ANIMACAO_PLAYER 5 // as velocidades de animação são inversamente proporcionais ao seus defines
#define NIVEL_MAX_OXIGENIO         1000
#define TICK_PLAYER                2
/*
    Enzo Capitani: Sprites iniciais do submarino
*/
const char *PLAYER_ESQUERDA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {
        "  |_     ",
        "([___]=|/",
    },
    {
        "  |_     ",
        "([___]=|-",
    },
    {
        "  |_     ",
        "([___]=|\\",
    }};

const char *PLAYER_DIREITA[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {
        "      _| ",
        "\\|=[___])",
    },
    {
        "      _| ",
        "-|=[___])",
    },
    {
        "      _| ",
        "/|=[___])",
    },
};

const char *(*PLAYER_SPRITE)[ALTURA_PLAYER] = PLAYER_DIREITA;

//Struct do player
typedef struct
{
    int x, y, score, nivelOxigenio, frameAtual;
    int vida, respirando;
    WORD cor;
} PLAYER;

PLAYER player;

//Enzo Emanoel: Criação do peixe

#define ALTURA_PEIXE               3
#define LARGURA_PEIXE              6
#define VEL_X_PEIXE                2
#define VEL_Y_PEIXE                1
#define TOTAL_FRAMES_PEIXE         2
#define VEL_ANIMACAO_PEIXE         15
#define PEIXE_MAX                  15
int TICK_PEIXE;

/*
    Enzo Emanoel: Sprites iniciais do peixe
*/
const char *PEIXE_DIREITA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {
        "  _-_ ",
        ">(_<')",
        "  '-' "
    },
    {
        "  _-_ ",
        ">(_>')",
        "  '-' "
    }
};

const char *PEIXE_ESQUERDA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {
        " _-_  ",
        "('>_)<",
        " '-'  "
    },
    {
        " _-_  ",
        "('<_)<",
        " '-'  "
    }
};

//Enzo Emanoel: Criação do tubarão

#define ALTURA_TUBARAO             3
#define LARGURA_TUBARAO            11
#define VEL_X_TUBARAO              2
#define VEL_Y_TUBARAO              1
#define TOTAL_FRAMES_TUBARAO       2
#define VEL_ANIMACAO_TUBARAO       15
#define TUBARAO_MAX                5
int TICK_TUBARAO;

/*
    Enzo Emanoel: Sprites iniciais do tubarão
*/
const char *TUBARAO_DIREITA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "\\____)\\____",
        "/-v___ __`ww",
        "     )/     "
    },
    {
        "\\____)\\____",
        "/-v___ __`==",
        "     )/     "
    }
};

const char *TUBARAO_ESQUERDA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "____/(____/ ",
        "ww`__ ___v-\\",
        "    )/      ",
    },
    {
        "____/(____/ ",
        "==`__ ___v-\\",
        "    )/      ",
    }
};

// Struct dos peixes
typedef struct {
    int x, y;
    int dx, dy;
    int vivo;
    int vida;
    int altura;
    int largura;
    const char **sprite;
    WORD cor;
} PEIXES;

PEIXES peixe[PEIXE_MAX], tubarao[TUBARAO_MAX];

// Tiro
#define VEL_TIRO 5
#define ICON_TIRO '='
#define MAX_TIRO 20
#define POS_TIRO_D 6
#define POS_TIRO_E 1

typedef struct {
    int x, y,
        dx,
        ativo;
} TIRO;

TIRO tiros[MAX_TIRO];

#define MORTO_MAX (PEIXE_MAX) //TODO Deve adicionar o MAX de inimigos no MORTO_MAX sempre que adicionar uma entidade nova
#define ALTURA_MORTO 3
#define LARGURA_MORTO 3

const char *ICON_MORTO[ALTURA_MORTO] = {
    "\\ /",
    " x ",
    "/ \\"
};

typedef struct {
    int x, y,
        timer,
        ativo;
} MORTO;

MORTO morto[MORTO_MAX];

int relogioGlobal = 0;
int telaAtual = TELA_INICIAL;

//portabilidade

#ifndef _WIN32
    // leitura do teclado
    #include <termios.h>
    #include <unistd.h>

    struct termios terminalOriginal;

    // Função que devolve o terminal ao normal quando o jogo fecha
    void restaurarTerminal() {
        tcsetattr(STDIN_FILENO, TCSANOW, &terminalOriginal);
    }

    void prepararTerminalCru() {
        struct termios terminalCru;

        // Salva o estado atual
        tcgetattr(STDIN_FILENO, &terminalOriginal);
        atexit(restaurarTerminal); 

        // Copia as configurações
        terminalCru = terminalOriginal;

        // Desliga o ECHO (para a tecla não aparecer) e o ICANON (para ler direto)
        terminalCru.c_lflag &= ~(ECHO | ICANON);

        // VMIN = 0: Não espera um número mínimo de caracteres serem digitados
        terminalCru.c_cc[VMIN] = 0;
        
        // VTIME = 0: Tempo de espera igual a zero. Retorna na hora!
        terminalCru.c_cc[VTIME] = 0;

        // Aplica as novas configurações imediatamente
        tcsetattr(STDIN_FILENO, TCSANOW, &terminalCru);    
    }

    // O Windows usa esses códigos hexa, vamos replicar pro Unix
    #define VK_UP      0x26
    #define VK_DOWN    0x28
    #define VK_LEFT    0x25
    #define VK_RIGHT   0x27
    #define VK_SPACE   0x20
    #define VK_ESCAPE  0x1B
    #define VK_CONTROL 0x11
    
    // Array para guardar quem está sendo pressionado no momento
    char estadoTeclas[256] = {0};

    void atualizarTecladoUnix() {
        for(int i = 0; i < 256; i++) estadoTeclas[i] = 0;

        unsigned char ch;
    
        // read() retorna > 0 se leu alguma tecla. 
        // STDIN_FILENO (que vale 0) significa o teclado, vindo da <unistd.h>
        while (read(STDIN_FILENO, &ch, 1) > 0) {
        
            if (ch == 27) { // 27 é o ESC (início de uma setinha)
                unsigned char seq1, seq2;
                
                // Tenta ler os próximos caracteres que o terminal manda colados
                if (read(STDIN_FILENO, &seq1, 1) > 0 && seq1 == '[') {
                    if (read(STDIN_FILENO, &seq2, 1) > 0) {
                        if (seq2 == 'A') estadoTeclas[VK_UP] = 3;
                        else if (seq2 == 'B') estadoTeclas[VK_DOWN] = 3;
                        else if (seq2 == 'C') estadoTeclas[VK_RIGHT] = 3;
                        else if (seq2 == 'D') estadoTeclas[VK_LEFT] = 3;
                    }
                } else {
                    // Se não veio o '[', o cara só apertou o botão ESC mesmo
                    estadoTeclas[VK_ESCAPE] = 3; 
                }
            } 
            else if (ch == ' ') {
                estadoTeclas[VK_SPACE] = 3;
            }
            // O Truque do Control: Aceita Enter (\n ou \r) ou a letra 'C'/'c'
            else if (ch == '\n' || ch == '\r' || ch == 'c' || ch == 'C') {
                estadoTeclas[VK_CONTROL] = 3;
            }
        }
    }

    short GetAsyncKeyState(int vKey) {
        if (vKey < 0 || vKey >= 256) return 0;
        if (estadoTeclas[vKey]) return (short)0x8000;
        return 0;
    }
#endif

#ifdef _WIN32
    // No Windows, esse macro não faz NADA (pois o Windows já lê o teclado sozinho)
    #define PROCESSAR_TECLADO() 
#else
    // No Unix, esse macro chama a nossa função que lê as teclas
    #define PROCESSAR_TECLADO() atualizarTecladoUnix()
#endif

//------------------------------- Protótipos -----------------------------------------------

void mudarTela(int);
void limparBufferTeclado();

void resetEntidades();
void reset();

//------------------------------- Métodos de sons -----------------------------------------------

// ---------------------------------- Animações das Entidades ------------------------------

void animacaoEntidades() {

    int framePeixe = (relogioGlobal / VEL_ANIMACAO_PEIXE) % TOTAL_FRAMES_PEIXE;
    int frameTubarao = (relogioGlobal / VEL_ANIMACAO_TUBARAO) % TOTAL_FRAMES_TUBARAO;

    for (int p = 0; p < PEIXE_MAX; p++) {
        if (peixe[p].vivo) {
            if (peixe[p].dx > 0) {
                peixe[p].sprite = (const char **)PEIXE_DIREITA[framePeixe];
            } else {
                peixe[p].sprite = (const char **)PEIXE_ESQUERDA[framePeixe];
            }
        }
    }

    for (int t = 0; t < TUBARAO_MAX; t++) {
        if (tubarao[t].vivo) {
            if (tubarao[t].dx > 0) {
                tubarao[t].sprite = (const char **)TUBARAO_DIREITA[frameTubarao];
            } else {
                tubarao[t].sprite = (const char **)TUBARAO_ESQUERDA[frameTubarao]; 
            }
        }
    }
}

// ---------------------------------- Métodos de desenhos ----------------------------------

// -------------------- Parte da tela inicial --------------------

void desenhaTelaInicial()
{

    for (int i = 0; i < TELA_LARGURA * TELA_ALTURA; ++i)
    {
        consoleBuffer[i].Char.AsciiChar = ' ';
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    for (int i = 0; i < ALTURA_LOGO; ++i)
    {
        for (int j = 0; j < LARGURA_LOGO; ++j)
        {
            int indice = (LOGO_Y + i) * TELA_LARGURA + (LOGO_X + j);
            consoleBuffer[indice].Char.AsciiChar = MRBX_QUESTLOGO[i][j];
            consoleBuffer[indice].Attributes = FOREGROUND_BLUE;
        }
    }

    char textoIniciar[100];
    sprintf(textoIniciar, "PRESSIONE CONTROL PARA INICIAR");

    int inicio = (ALTURA_LOGO + LOGO_Y + 1) * (TELA_LARGURA) + 48;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    atualizarTela();
}

// -------------------- Parte da tela Game Over --------------------

void desenhaTelaGameOver()
{

    for (int i = 0; i < TELA_LARGURA * TELA_ALTURA; ++i)
    {
        consoleBuffer[i].Char.AsciiChar = ' ';
        consoleBuffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
    }

    for (int i = 0; i < ALTURA_GAMEOVER; ++i)
    {
        for (int j = 0; j < LARGURA_GAMEOVER; ++j)
        {
            int indice = (GAME_OVER_Y + i) * TELA_LARGURA + (GAME_OVER_X + j);
            consoleBuffer[indice].Char.AsciiChar = GAMEOVER[i][j];
            consoleBuffer[indice].Attributes = FOREGROUND_RED;
        }
    }

    char textoIniciar[100];
    sprintf(textoIniciar, "PRESSIONE CONTROL PARA VOLTAR AO MENU");

    int inicio = (ALTURA_GAMEOVER + GAME_OVER_Y + 1) * (TELA_LARGURA) + 35;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    atualizarTela();
}

// -------------------- Parte da tela inicial --------------------

void desenhaScore()
{
    char textoScore[30];
    sprintf(textoScore, "Score: %d", player.score);

    int inicio = TELA_LARGURA + 5;
    for (int i = 0; textoScore[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoScore[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED;
    }
}

void desenhaVida()
{
    char textoVida[15];
    sprintf(textoVida, "Vida: %d", player.vida);

    int inicio = TELA_LARGURA * 2 - 15;
    for (int i = 0; textoVida[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoVida[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED;
    }
}

void desenhaBarraOxigenio()
{
    int frameAtual = relogioGlobal % 5;

    WORD corBarraOx = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED;

    if (frameAtual == 0 && player.nivelOxigenio < 250)
    {
        corBarraOx = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED;
    }

    char barras[25];
    int indiceBarras = 0;
    for (int i = 0; i <= 1000; i += 50)
    {
        if (i <= player.nivelOxigenio)
        {
            barras[indiceBarras] = '|';
            indiceBarras++;
            continue;
        }
        barras[indiceBarras] = ' ';
        indiceBarras++;
    }
    barras[21] = '\0';

    char barraOxigenio[51];
    sprintf(barraOxigenio, "OXIGENIO: [%s]", barras);

    int inicio = TELA_LARGURA + 40;
    for (int i = 0; barraOxigenio[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = barraOxigenio[i];
        consoleBuffer[inicio + i].Attributes = corBarraOx;
    }
}

void desenhaPlayer()
{
    int frameAtualPlayer = (relogioGlobal / VELOCIDADE_ANIMACAO_PLAYER) % TOTAL_FRAMES_JOGADOR;

    for (int i = 0; i < ALTURA_PLAYER; i++)
    {
        for (int j = 0; j < LARGURA_PLAYER; j++)
        {

            int posX = player.x + j;
            int posY = player.y + i;
            if (posX >= 0 && posX < TELA_LARGURA && posY >= 0 && posY < TELA_ALTURA)
            {

                int indice = posY * TELA_LARGURA + posX;

                char caractere = PLAYER_SPRITE[frameAtualPlayer][i][j];

                if (caractere != ' ')
                {
                    consoleBuffer[indice].Char.AsciiChar = caractere;
                    if(player.y <= ALTURA_CEU - 1 && i < 1){
                        consoleBuffer[indice].Attributes = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
                        continue;    
                    }
                    consoleBuffer[indice].Attributes = player.cor;
                }
            }
        }
    }
}

void desenharEntidades(PEIXES entidade[], int entidade_max) {
    for (int e = 0; e < entidade_max; e++) {
        if (!entidade[e].vivo) {
            continue;
        }

        int alturaEntidade = entidade[e].altura;
        int larguraEntidade = entidade[e].largura;

        for (int i = 0; i < alturaEntidade; i++) {
            for (int j = 0; j < larguraEntidade; j++) {

                int px = entidade[e].x + j;
                int py = entidade[e].y + i;

                if (px >= 0 && px < TELA_LARGURA && py >= 0 && py < TELA_ALTURA) {
                    char caractere = entidade[e].sprite[i][j];

                    if (caractere != ' ') {
                        int indice = py * TELA_LARGURA + px;
                        
                        consoleBuffer[indice].Char.AsciiChar = caractere;
                        consoleBuffer[indice].Attributes = entidade[e].cor | BACKGROUND_BLUE;
                    }
                }
            }
        }
    }
}

void desenhaTiro()
{
    for(int i = 0; i < MAX_TIRO; i++)
    {   
        TIRO tiro = tiros[i]; 
        if(tiro.ativo)
        {
            int posX = tiro.x,
                posY = tiro.y,
                indice = posY * TELA_LARGURA + posX;


            if(posX < TELA_LARGURA && posX > 0){
                consoleBuffer[indice].Char.AsciiChar = ICON_TIRO;
                consoleBuffer[indice].Attributes =  FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
            }
        }
    }
}

void desenhaMorto()
{
    for(int m = 0; m < MORTO_MAX; m++){
        if(morto[m].ativo){
            for(int i = 0; i < ALTURA_MORTO; i++)
            {
                for(int j = 0; j < LARGURA_MORTO; j++)
                {                    
                    int posX = morto[m].x + i;
                    int posY = morto[m].y + j;
                    
                    if(!(posX < 0 || posX > TELA_LARGURA || posY < 0 || posY > TELA_ALTURA)){
                        char caractere = ICON_MORTO[i][j];

                        if(!caractere != ' ')
                        {
                            consoleBuffer[posY * TELA_LARGURA + posX].Char.AsciiChar = caractere;
                            consoleBuffer[posY * TELA_LARGURA + posX].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE;
                        }
                    }
                }
            }
        }
    }
}

void desenhaMapa()
{
    for (int i = 0; i < TELA_LARGURA * TELA_ALTURA; i++)
    {   
        consoleBuffer[i].Char.AsciiChar = ' ';

        if(i < TELA_LARGURA * ALTURA_CEU){
            consoleBuffer[i].Attributes = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            continue;
        }

        if (i >= TELA_LARGURA * (TELA_ALTURA - 2)) {
            consoleBuffer[i].Attributes = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            continue;
        }

        consoleBuffer[i].Attributes = BACKGROUND_BLUE;
    }
}

void desenhaTela()
{
    desenhaMapa();
    desenhaVida();
    desenhaScore();
    desenhaBarraOxigenio();
    desenhaPlayer();
    desenharEntidades(peixe, PEIXE_MAX);
    desenharEntidades(tubarao, TUBARAO_MAX);
    desenhaTiro();
    desenhaMorto();
    atualizarTela();
}

// ---------------------------------- Sistemas Autônomos --------------------------------

void spawnarPeixes() {
    
    // Chance

    if (rand() % 4 != 0) { return; }

    // Cardume

    int peixesLivres = 0;

    for (int p = 0; p < PEIXE_MAX; p++) { if (!peixe[p].vivo) { peixesLivres++; } }
    if (!peixesLivres) { return; }
    
    int cardume = peixesLivres;
    if (cardume >= 3) { cardume = 3; };

    // Constantes
    
    int tamanhoCardumeFinal, alturaBaseFinal, ladoNascerFinal;

    int posicaoLivre = 0;
    int tentativas = 0;

    // Verificação de linhas livres

    while (tentativas < 5) {

        int tamanhoCardume = rand() % cardume + 1;

        int alturaMin = 4;
        int alturaMax = TELA_ALTURA - 2 - (tamanhoCardume * ALTURA_PEIXE);
        if (alturaMax < alturaMin) { alturaMax = alturaMin; }

        int alturaBase = alturaMin + (rand() % (alturaMax - alturaMin + 1));

        int ladoNascer = (rand() % 2 == 0);

        posicaoLivre = 1;
        
        for (int p = 0; p < PEIXE_MAX; p++) {
            if (peixe[p].vivo) {
                if (abs(alturaBase - peixe[p].y) < (tamanhoCardume * ALTURA_PEIXE + 1)) {
                    posicaoLivre = 0;
                    break;
                }
            }
        }

        if (posicaoLivre == 1) {
            tamanhoCardumeFinal = tamanhoCardume;
            alturaBaseFinal = alturaBase;
            ladoNascerFinal = ladoNascer;
            break;
        }

        tentativas++;
    }

    if (posicaoLivre == 0) {
        return;
    }

    // Nascimento

    int peixesNascidos = 0;

    for (int p = 0; p < PEIXE_MAX; p++)
    {
        if (!peixe[p].vivo)
        {
            peixe[p].vivo = 1;
            peixe[p].vida = 1;
            peixe[p].y = alturaBaseFinal + (peixesNascidos * ALTURA_PEIXE);

            peixe[p].altura = ALTURA_PEIXE;
            peixe[p].largura = LARGURA_PEIXE;

            WORD corPeixe;

            do
            {
                int indiceCor = rand() % TOTAL_CORES;
                corPeixe = PALETA_DE_CORES[indiceCor];
            } while (corPeixe == peixe[p].cor);
            
            peixe[p].cor = corPeixe;

            if (ladoNascerFinal)
            {
                peixe[p].x = 0 - LARGURA_PEIXE;
                peixe[p].dx = 2;
            }
            else
            {
                peixe[p].x = TELA_LARGURA + LARGURA_PEIXE;
                peixe[p].dx = -2;
            }
            
            peixesNascidos++;

            if (peixesNascidos >= tamanhoCardumeFinal) { break; }
        }
    }
}

void spawnarTubarao() {
    
    // Chance

    if (rand() % 4 != 0) { return; }

    // Constantes
    
    int alturaBaseFinal, ladoNascerFinal;

    int posicaoLivre = 0;
    int tentativas = 0;

    // Verificação de linhas livres

    while (tentativas < 5) {

        int alturaMin = 4;
        int alturaMax = TELA_ALTURA - 2 - ALTURA_TUBARAO;
        if (alturaMax < alturaMin) { alturaMax = alturaMin; }

        int alturaBase = alturaMin + (rand() % (alturaMax - alturaMin + 1));

        int ladoNascer = (rand() % 2 == 0);

        posicaoLivre = 1;
        
        for (int t = 0; t < TUBARAO_MAX; t++) {
            if (tubarao[t].vivo) {
                if (abs(alturaBase - tubarao[t].y) < (ALTURA_TUBARAO + 1)) {
                    posicaoLivre = 0;
                    break;
                }
            }
        }

        if (posicaoLivre == 1) {
            alturaBaseFinal = alturaBase;
            ladoNascerFinal = ladoNascer;
            break;
        }

        tentativas++;
    }

    if (posicaoLivre == 0) {
        return;
    }

    // Nascimento

    for (int t = 0; t < TUBARAO_MAX; t++)
    {
        if (!tubarao[t].vivo)
        {
            tubarao[t].vivo = 1;
            tubarao[t].vida = 4;
            tubarao[t].y = alturaBaseFinal;

            tubarao[t].altura = ALTURA_TUBARAO;
            tubarao[t].largura = LARGURA_TUBARAO;

            WORD corTubarao;

            do
            {
                int indiceCor = rand() % TOTAL_CORES;
                corTubarao = PALETA_DE_CORES[indiceCor];
            } while (corTubarao == tubarao[t].cor);
            
            tubarao[t].cor = corTubarao;

            if (ladoNascerFinal)
            {
                tubarao[t].x = 0 - LARGURA_TUBARAO;
                tubarao[t].dx = 3;
            }
            else
            {
                tubarao[t].x = TELA_LARGURA + LARGURA_TUBARAO;
                tubarao[t].dx = -3;
            }

            break;
        }
    }
}

void gerenciarSpawns() {
    int sorteio = rand() % 100;

    if (sorteio < 15) {
        spawnarTubarao(); 
    } else {
        spawnarPeixes(); 
    }
}

void matarEntidade(int posX, int posY)
{
    for(int i = 0; i < MORTO_MAX; i++)
    {
        if(!morto[i].ativo)
        {
            morto[i].ativo = 1;
            morto[i].timer = 3;
            morto[i].x = posX;
            morto[i].y = posY;
            break;
        }
    }
}

// ---------------------------------- Métodos de ações ----------------------------------

void acaoTela(char teclaMudar, int tela)
{
    if(GetAsyncKeyState(VK_CONTROL)){
        mudarTela(tela);
    }
}

void acoesPlayer()
{
    if (relogioGlobal % TICK_PLAYER == 0) {
        if (GetAsyncKeyState(VK_RIGHT))
        {
            player.x += VELOCIDADE_X_PLAYER;
            PLAYER_SPRITE = PLAYER_DIREITA;
        }
        if (GetAsyncKeyState(VK_LEFT))
        {
            player.x -= VELOCIDADE_X_PLAYER;
            PLAYER_SPRITE = PLAYER_ESQUERDA;
        }

        if (GetAsyncKeyState(VK_DOWN)) player.y += VELOCIDADE_Y_PLAYER;
        if (GetAsyncKeyState(VK_UP)) player.y -= VELOCIDADE_Y_PLAYER;
    }
}

void acaoTiro()
{
    if (GetAsyncKeyState(VK_SPACE))
    {   
        for(int i = 0; i < MAX_TIRO; i++)
            {
                if(!tiros[i].ativo)
                {
                    tiros[i].ativo = 1;
                    tiros[i].x = (PLAYER_SPRITE == PLAYER_DIREITA) ? player.x +  POS_TIRO_D: player.x + POS_TIRO_E;
                    tiros[i].y = player.y + 1;
                    tiros[i].dx = (PLAYER_SPRITE == PLAYER_DIREITA) ? VEL_TIRO : -VEL_TIRO;

                    break;
                }
            }
    }
}

// ---------------------------------- Métodos de colisões ----------------------------------

void colisaoPlayerEntidade(PEIXES entidade[], int entidade_MAX)
{
    for (int e = 0; e < entidade_MAX; e++)
    {
        if (player.x + LARGURA_PLAYER > entidade[e].x &&
             player.x < entidade[e].x + LARGURA_PEIXE &&
              player.y + ALTURA_PLAYER > entidade[e].y &&
               player.y < entidade[e].y + ALTURA_PEIXE && entidade[e].vivo == 1)
        {
            resetEntidades();
            player.vida--;

            player.cor = FOREGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY;

        }
    }
}

void colisaoEntidadeTiro(PEIXES entidade[], int entidade_MAX, int altura_entidade, int largura_entidade)
{
    for (int t = 0; t < MAX_TIRO; t++)
    {
        if (tiros[t].ativo)
        {
            for (int e = 0; e < entidade_MAX; e++)
            {
                if (entidade[e].vivo)
                {
                    if (entidade[e].y < tiros[t].y + 1 && entidade[e].y + altura_entidade > tiros[t].y &&
                        entidade[e].x < tiros[t].x + 1 && entidade[e].x + largura_entidade > tiros[t].x)
                    {
                        entidade[e].vida--;
                        tiros[t].ativo = 0;
                        player.score += 50;

                        break;
                    }
                }
            }
            if (!tiros[t].ativo)
            {
                break;
            }
        }
    }
}

int checkColisaoEntidades(PEIXES entidade1, PEIXES entidade2) {
    int colisaoX = (entidade1.x < entidade2.x + entidade2.largura) && (entidade1.x + entidade1.largura > entidade2.x);
    int colisaoY = (entidade1.y < entidade2.y + entidade2.altura) && (entidade1.y + entidade1.altura > entidade2.y);
    
    return colisaoX && colisaoY;
}

void checkEncontrosEntidades(PEIXES entidade1[], int entidade1_MAX, PEIXES entidade2[], int entidade2_MAX) {
    for (int e1 = 0; e1 < entidade1_MAX; e1++) {
        if (!entidade1[e1].vivo) { continue; }

        for (int e2 = 0; e2 < entidade2_MAX; e2++) {
            if (!entidade2[e2].vivo) { continue; }

            if (checkColisaoEntidades(entidade1[e1], entidade2[e2])) {
                entidade2[e2].vivo = 0;
            }
        }

    }
}

void colisoes()
{
    colisaoPlayerEntidade(peixe, PEIXE_MAX);
    colisaoPlayerEntidade(tubarao, TUBARAO_MAX);
    colisaoEntidadeTiro(peixe, PEIXE_MAX, ALTURA_PEIXE, LARGURA_PEIXE);
    colisaoEntidadeTiro(tubarao, TUBARAO_MAX, ALTURA_TUBARAO, LARGURA_TUBARAO);
    checkEncontrosEntidades(tubarao, TUBARAO_MAX, peixe, PEIXE_MAX);
}

// ---------------------------------- Sistema de dificuldade ----------------------------------

// TODO: Adicionar morte por vida
// TODO: Adicionar morte por oxigenio

void aumentarVelEntidades() {
    int dificuldade = player.score / 5000;

    if (dificuldade == 0) {
        TICK_PEIXE = 5;
        TICK_TUBARAO = 5;
    }
    else if (dificuldade == 1) {
        TICK_PEIXE = 4;
        TICK_TUBARAO = 4;
    }
    else if (dificuldade == 3) {
        TICK_PEIXE = 3;
        TICK_TUBARAO = 3;
    }
    else if (dificuldade == 4) {
        TICK_PEIXE = 2;
        TICK_TUBARAO = 2;
    }
    else if (dificuldade >= 6) {
        TICK_PEIXE = 1;
        TICK_TUBARAO = 1;
    }
}

// ---------------------------------- Métodos de atualizações ----------------------------------

void updatePlayer()
{
    acoesPlayer();

    if (player.x < 0)
    {
        player.x = 0;
    }
    if (player.y < ALTURA_CEU - 1)
    {
        player.y = ALTURA_CEU - 1;
    }
    if (player.x + LARGURA_PLAYER > TELA_LARGURA)
    {
        player.x = TELA_LARGURA - LARGURA_PLAYER;
    }
    if (player.y + ALTURA_PLAYER > TELA_ALTURA - 2)
    {
        player.y = TELA_ALTURA - ALTURA_PLAYER - 2;
    }

    if(player.nivelOxigenio < 0)
        player.nivelOxigenio = 0;

    if(player.nivelOxigenio > NIVEL_MAX_OXIGENIO)
        player.nivelOxigenio = NIVEL_MAX_OXIGENIO;

    if(player.y <= ALTURA_CEU - 1){
        player.respirando = 1;
        player.nivelOxigenio += NIVEL_MAX_OXIGENIO * 0.02;
        
    }else{
        player.respirando = 0;
        player.nivelOxigenio -= NIVEL_MAX_OXIGENIO * 0.002;
    }

    if(relogioGlobal % 10 == 0){
        player.cor = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
    }

}

void updateTiro()
{   
    acaoTiro();
    for(int i = 0; i < MAX_TIRO; i++)
    {   
        if(tiros[i].ativo)
        {
            tiros[i].x += tiros[i].dx;
            if(tiros[i].x < 0 || tiros[i].x > TELA_LARGURA)
            {
                tiros[i].ativo = 0;
            }
        }
    }
}

void updateEntidade(PEIXES entidade[], int entidade_MAX, int largura_entidade, int tick_entidade) {
    for (int e = 0; e < entidade_MAX; e++) {
        if (entidade[e].vivo && entidade[e].vida <= 0)
        {
            entidade[e].vivo = 0;
            matarEntidade(entidade[e].x, entidade[e].y);
        }
        if (entidade[e].vivo) {

            if (relogioGlobal % tick_entidade == 0) entidade[e].x += entidade[e].dx;

            if (entidade[e].x <= 0 - largura_entidade || entidade[e].x > TELA_LARGURA + largura_entidade) {
                entidade[e].vivo = 0;
            }
        }
    }
}

void updateMorto()
{
    for(int m = 0; m < MORTO_MAX; m++)
    {
        if(morto[m].timer <= 0)
        {
            morto[m].ativo = 0;
        }
        else
        {
            morto[m].timer--;
        }
    }
}

void update()
{   
    if(telaAtual == TELA_INICIAL){
        PROCESSAR_TECLADO();
        desenhaTelaInicial();
        acaoTela('p', TELA_JOGO);
        dormirMs(400);
        limparBufferTeclado();
    }

    if(telaAtual == TELA_JOGO){
        PROCESSAR_TECLADO();
        gerenciarSpawns();
        animacaoEntidades();
        updatePlayer();
        aumentarVelEntidades();
        updateEntidade(peixe, PEIXE_MAX, LARGURA_PEIXE, TICK_PEIXE);
        updateEntidade(tubarao, TUBARAO_MAX, LARGURA_TUBARAO, TICK_TUBARAO);
        updateTiro();
        updateMorto();
        colisoes();
        desenhaTela();
    }

    if(telaAtual == TELA_GAMEOVER){
        PROCESSAR_TECLADO();
        desenhaTelaGameOver();
        acaoTela('p', TELA_INICIAL);
        dormirMs(400);
        limparBufferTeclado();
    }
    
    relogioGlobal++;
}

// ---------------------------------- Métodos de inicializações ----------------------------------
void iniciarPlayer()
{
    player.x = 30;
    player.y = 15;
    player.nivelOxigenio = 1000;
    player.vida = 5;
    player.score = 0;
    player.respirando = 0;
    player.cor = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
}

void iniciarEntidade(PEIXES entidade[], int entidade_MAX)
{
    for (int e = 0; e < entidade_MAX; e++) {
        entidade[e].vivo = 0;
    }
}

void iniciarTiros()
{
    for(int i = 0; i < MAX_TIRO; i++)
    {   
        tiros[i].ativo = 0;
    }
}

void iniciarMorto()
{
    for(int i = 0; i < MORTO_MAX; i++)
    {
        morto[i].ativo = 0;
    }
}

void iniciar()
{   
    iniciarPlayer();
    iniciarEntidade(peixe, PEIXE_MAX);
    iniciarEntidade(tubarao, TUBARAO_MAX);
    iniciarTiros();
    iniciarMorto();
}

//  ---------------------------------- RESET ----------------------------------

void resetEntidades()
{   
    //PEIXES peixe[PEIXE_MAX], tubarao[TUBARAO_MAX];

    for(int i = 0; i < TUBARAO_MAX; i++){
        tubarao[i].vivo = 0;
        tubarao[i].x = 0;
    }

    for(int i = 0; i < PEIXE_MAX; i++){
        peixe[i].vivo = 0;
        peixe[i].x = 0;
    }

}

void reset()
{   
    resetEntidades();
    iniciarPlayer();
    iniciarTiros();
    iniciarMorto();
}

//  ---------------------------------- UTIL ----------------------------------

void limparBufferTeclado()
{
#ifdef _WIN32
    while(kbhit()) getch();
#else
    int ch;
    while((ch = getchar()) != EOF) {}
#endif
}

void mudarTela(int tela)
{
    telaAtual = tela;
    reset();
}

// ---------------------------------- Main ----------------------------------

int main(int argc, char* argv[]){

#ifdef _WIN32
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#else
    prepararTerminalCru();

    printf("\033[2J");   // Limpa a tela inteira do terminal do Unix
    printf("\033[?25l"); // Esconde o cursor piscando para ficar com cara de jogo
    fflush(stdout);      // Força o terminal a aplicar isso agora
#endif

    srand((unsigned)time(NULL));

    iniciar();

    while(1)
    {   
        update();
        dormirMs(DELAY);
    }
    
#ifndef _WIN32
    printf("\033[?25h"); 
#endif

    return 0;
}