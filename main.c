#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>

//Constantes da tela
#define TELA_LARGURA 125
#define TELA_ALTURA  25
#define DELAY        90

//Enzo Capitani: Criação do player

#define ALTURA_PLAYER              2
#define LARGURA_PLAYER             9
#define VELOCIDADE_X_PLAYER        2
#define VELOCIDADE_Y_PLAYER        1
#define TOTAL_FRAMES_JOGADOR       3
#define VELOCIDADE_ANIMACAO_PLAYER 1 // as velocidades de animação são inversamente proporcionais ao seus defines

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
    int vida;
} PLAYER;

PLAYER player;

//Enzo Emanoel: Criação do peixe

#define ALTURA_PEIXE               3
#define LARGURA_PEIXE              13
#define VEL_X_PEIXE                2
#define VEL_Y_PEIXE                1
#define TOTAL_FRAMES_PEIXE         2
#define VEL_ANIMACAO_PEIXE         1

/*
    Enzo Emanoel: Sprites iniciais do peixe
*/
const char *PEIXE_DIREITA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {
        "\\____)\\____",
        "/-v___ __`<  ",
        "     )/      "
    },
    {
        "\\____)\\____",
        "/-v___ __`=  ",
        "     )/      "
    }
};

const char *PEIXE_ESQUERDA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {
        "____/(____/  ",
        " >`__ ___v-\\",
        "    )/       ",
    },
    {
        "____/(____/  ",
        " >`__ ___v-\\",
        "    )/       ",
    }
};

const char *(*PEIXE_SPRITE)[ALTURA_PEIXE] = PEIXE_DIREITA;

// Struct do peixe
typedef struct {
    int x, y;
    int dx, dy;
    int vivo;
    int vida;
} PEIXE;

PEIXE peixe;

//Enzo Emanoel: Criação do tubarão

#define ALTURA_TUBARAO             3
#define LARGURA_TUBARAO            13
#define VEL_X_TUBARAO              2
#define VEL_Y_TUBARAO              1
#define TOTAL_FRAMES_TUBARAO       2
#define VEL_ANIMACAO_TUBARAO       1

/*
    Enzo Emanoel: Sprites iniciais do tubarão
*/
const char *TUBARAO_DIREITA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "\\____)\\____",
        "/-v___ __`<  ",
        "     )/      "
    },
    {
        "\\____)\\____ ",
        "/-v___ __`=  ",
        "     )/      "
    }
};

const char *TUBARAO_ESQUERDA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "____/(____/  ",
        " >`__ ___v-\\ ",
        "    )/       ",
    },
    {
        "____/(____/  ",
        " >`__ ___v-\\ ",
        "    )/       ",
    }
};

const char *(*TUBARAO_SPRITE)[ALTURA_TUBARAO] = TUBARAO_DIREITA;

// Struct do tubarão
typedef struct {
    int x, y;
    int dx, dy;
    int vivo, ativo;
    int vida;
} TUBARAO;

TUBARAO tubarao;

#define VEL_TIRO 5
#define ICON_TIRO '='
#define MAX_TIRO 10
#define POS_TIRO_D 6
#define POS_TIRO_E 1

typedef struct {
    int x, y,
        dx,
        ativo;
} TIRO;

TIRO tiros[MAX_TIRO];

//Coisas do buffer
HANDLE hConsole;
CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];
COORD bufferSize = {TELA_LARGURA, TELA_ALTURA};
COORD bufferCoord = {0, 0};
SMALL_RECT consoleWriteArea = {0, 0, TELA_LARGURA-1, TELA_ALTURA-1};

int relogioGlobal = 0;

// ---------------------------------- Métodos de desenhos ----------------------------------

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
                    if(player.y <= 4 && i < 1){
                        consoleBuffer[indice].Attributes = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
                        continue;    
                    }
                    consoleBuffer[indice].Attributes = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
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

void desenhaMapa()
{
    for (int i = 0; i < TELA_LARGURA * TELA_ALTURA; i++)
    {   
        consoleBuffer[i].Char.AsciiChar = ' ';
        if(i < TELA_LARGURA * 5){
            consoleBuffer[i].Attributes = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            continue;
        }
        consoleBuffer[i].Attributes = BACKGROUND_BLUE;
    }
}

void desenhaTela()
{
    desenhaMapa();
    desenhaPlayer();
    desenhaTiro();
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

// ---------------------------------- Métodos de ações ----------------------------------

void acoesPlayer()
{

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
    if (GetAsyncKeyState(VK_DOWN))
        player.y += VELOCIDADE_Y_PLAYER;
    if (GetAsyncKeyState(VK_UP))
        player.y -= VELOCIDADE_Y_PLAYER;
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

// ---------------------------------- Métodos de atualizações ----------------------------------

void updatePlayer()
{
    acoesPlayer();

    if (player.x < 0)
    {
        player.x = 0;
    }
    if (player.y < 4)
    {
        player.y = 4;
    }
    if (player.x + LARGURA_PLAYER > TELA_LARGURA)
    {
        player.x = TELA_LARGURA - LARGURA_PLAYER;
    }
    if (player.y + ALTURA_PLAYER > TELA_ALTURA - 2)
    {
        player.y = TELA_ALTURA - ALTURA_PLAYER - 2;
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

void update()
{
    updatePlayer();
    updateTiro();
    desenhaTela();

    relogioGlobal++;
}

// ---------------------------------- Métodos de inicializações ----------------------------------
void iniciarPlayer()
{
    player.x = 30;
    player.y = 15;
}

void iniciarTiros()
{
    for(int i = 0; i < MAX_TIRO; i++)
    {   
        tiros[i].ativo = 0;
    }
}

void iniciar()
{
    iniciarPlayer();
    iniciarTiros();
}

// ---------------------------------- Main ----------------------------------

//acho que é o main
int main(){
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    iniciar();

    while(1)
    {   
        acoesPlayer();
        update();
        Sleep(DELAY);
    }

    return 0;
}