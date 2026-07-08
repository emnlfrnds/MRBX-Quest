// ============================================================================
// INCLUDES
// ============================================================================

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

// ============================================================================
// MACROS E CONSTANTES (#define)
// ============================================================================

#define TELA_LARGURA 125
#define TELA_ALTURA 25

#define DELAY 30

#define ALTURA_CEU 4
#define ALTURA_CHAO 2

#define TELA_INICIAL 0
#define TELA_JOGO 1
#define TELA_GAMEOVER 2

#define LARGURA_LOGO 56
#define ALTURA_LOGO 5
#define LOGO_X 35
#define LOGO_Y 8

#define ALTURA_GAMEOVER 8
#define LARGURA_GAMEOVER 100
#define GAME_OVER_X 13
#define GAME_OVER_Y 3

#define ALTURA_PLAYER 2
#define LARGURA_PLAYER 9
#define VELOCIDADE_X_PLAYER 2
#define VELOCIDADE_Y_PLAYER 1
#define TOTAL_FRAMES_JOGADOR 3
#define VELOCIDADE_ANIMACAO_PLAYER 5
#define NIVEL_MAX_OXIGENIO 1000
#define NUMERO_AUMENTA_VIDA 10000
#define TICK_PLAYER 2
#define TICK_OXIGENIO 2

#define ALTURA_PESSOA 3
#define LARGURA_PESSOA 3
#define VELOCIDADE_PESSOA 2
#define TOTAL_FRAMES_PESSOA 3
#define VELOCIDADE_ANIMACAO_PESSOA 7
#define MAX_PESSOAS 10
// #define TICK_PESSOA

#define ALTURA_PEIXE 3
#define LARGURA_PEIXE 6
#define VEL_X_PEIXE 2
#define VEL_Y_PEIXE 1
#define TOTAL_FRAMES_PEIXE 2
#define VEL_ANIMACAO_PEIXE 15
#define PEIXE_MAX 3

#define ALTURA_TUBARAO 3
#define LARGURA_TUBARAO 11
#define VEL_X_TUBARAO 2
#define VEL_Y_TUBARAO 1
#define TOTAL_FRAMES_TUBARAO 2
#define VEL_ANIMACAO_TUBARAO 15
#define TUBARAO_MAX 1

#define ALTURA_INIMIGO 2
#define LARGURA_INIMIGO 8
#define TOTAL_FRAMES_INIMIGO 3
#define VEL_ANIMACAO_INIMIGO 5
#define VEL_X_INIMIGO 10
#define INTERVALO_TIRO 86
#define INIMIGO_MAX 1

#define VEL_TIRO 5
#define VEL_TIRO_INIMIGO 1
#define ICON_TIRO '='
#define MAX_TIRO 20
#define MAX_TIRO_INIMIGO INIMIGO_MAX
#define POS_TIRO_D 6
#define POS_TIRO_E 1

#define MORTO_MAX (PEIXE_MAX) // TODO Deve adicionar o MAX de inimigos no MORTO_MAX sempre que adicionar uma entidade nova
#define ALTURA_MORTO 3
#define LARGURA_MORTO 3

// ============================================================================
// STRUCTS
// ============================================================================

typedef struct
{
    int x, y, score, nivelOxigenio, frameAtual;
    int vida, respirando, pessoasSalvas, numVida;
    WORD cor;
} PLAYER;

PLAYER player;

typedef struct
{
    int x, y;
    int vivo;
    int lado;
} PESSOAS;

PESSOAS pessoas[MAX_PESSOAS];

typedef struct
{
    int x, y;
    int dx, dy;
    int vivo;
    int vida;
    int altura;
    int largura;
    int tipo;
    int tipo_ataque;
    int intervalo_ataque;
    int indice_ataque;
    const char **sprite;
    WORD cor;
} PEIXES;

PEIXES peixe[PEIXE_MAX], tubarao[TUBARAO_MAX], inimigo[INIMIGO_MAX];

typedef struct
{
    int x, y,
        dx,
        ativo;
} TIRO;

TIRO tiros[MAX_TIRO];
TIRO tirosInimigo[MAX_TIRO_INIMIGO];

typedef struct
{
    int x, y,
        timer,
        ativo;
} MORTO;

MORTO morto[MORTO_MAX];

// ============================================================================
// TELAS E SPRITES
// ============================================================================

const char *MRBX_QUESTLOGO[ALTURA_LOGO] = {

    " __  __ ____  ______  __   ___  _   _ _____ ____ _____ ",
    "|  \\/  |  _ \\| __ ) \\/ /  / _ \\| | | | ____/ ___|_   _|",
    "| |\\/| | |_) |  _ \\\\  /  | | | | | | |  _| \\___ \\ | |  ",
    "| |  | |  _ <| |_) /  \\  | |_| | |_| | |___ ___) || |  ",
    "|_|  |_|_| \\_\\____/_/\\_\\  \\__\\_\\\\___/|_____|____/ |_|  "

};

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

const WORD PALETA_DE_CORES[] = {
    FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
}; const int TOTAL_CORES = 3;

const char *PLAYER_MORTO[TOTAL_FRAMES_JOGADOR][ALTURA_PLAYER] = {
    {
        "  --__-- ",
        "  --  -- ",
    },
    {
        " -  _  -",
        " -     -",
    },
    {
        "        ",
        "        ",
    },
};

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

const char *PESSOA_SPRITE[TOTAL_FRAMES_PESSOA][ALTURA_PESSOA] = {
    {" O ",
     "/|\\",
     "/ \\"},
    {"_O_",
     " | ",
     "/ \\"},
    {"\\O/",
     " | ",
     "/ \\"},
};

const char *PEIXE_DIREITA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {"  _-_ ",
     ">(_<')",
     "  '-' "},
    {"  _-_ ",
     ">(_>')",
     "  '-' "}};

const char *PEIXE_ESQUERDA[TOTAL_FRAMES_PEIXE][ALTURA_PEIXE] = {
    {" _-_  ",
     "('>_)<",
     " '-'  "},
    {" _-_  ",
     "('<_)<",
     " '-'  "}};

const char *TUBARAO_DIREITA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {"\\____)\\____",
     "/-v___ __`ww",
     "     )/     "},
    {"\\____)\\____",
     "/-v___ __`==",
     "     )/     "}};

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
    }};

const char *INIMIGO_DIREITA[TOTAL_FRAMES_INIMIGO][ALTURA_INIMIGO] = {
    {"   _/| ",
     "\\=|__|)"},
    {"   _/| ",
     "-=|__|)"},
    {"   _/| ",
     "/=|__|)"}};
const char *INIMIGO_ESQUERDA[TOTAL_FRAMES_INIMIGO][ALTURA_INIMIGO] = {
    {" |\\_   ",
     "(|__|=/"},
    {" |\\_   ",
     "(|__|=-"},
    {" |\\_   ",
     "(|__|=\\"}};

const char *ICON_MORTO[ALTURA_MORTO] = {
    "\\ /",
    " x ",
    "/ \\"};

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

HANDLE hConsole;
CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];
COORD bufferSize = {TELA_LARGURA, TELA_ALTURA};
COORD bufferCoord = {0, 0};
SMALL_RECT consoleWriteArea = {0, 0, TELA_LARGURA - 1, TELA_ALTURA - 1};

int salvando = 0;
int morrendo = 0;
int frameMorte = 0, primeiroFrame = 1;

int TICK_PEIXE;
int TICK_TUBARAO;
int TICK_INIMIGO;

int relogioGlobal = 0;
int telaAtual = TELA_INICIAL;

// ============================================================================
// PROTÓTIPOS DE FUNÇÕES
// ============================================================================

void animacaoEntidades();
void desenhaTelaInicial();
void desenhaTelaGameOver();
void desenhaScore();
void desenhaVida();
void desenhaBarraOxigenio();
void desenhaPessoasSalvas();
void desenhaPlayer();
void desenharEntidades(PEIXES entidade[], int entidade_max);
void desenhaTiro();
void desenhaPessoa();
void desenhaMorto();
void desenhaMapa();
void desenhaTela();
void salvarPessoa();
void spawnarPessoa();
void spawnarPeixes();
void spawnarTubarao();
void spawnarInimigo();
void gerenciarSpawns();
void matarEntidade(int posX, int posY);
void acaoTela(int tela, int tecla);
void acoesPlayer();
void acaoTiro();
void acaoTiroInimigo();
void levarDano();
void colisaoPlayerEntidade(PEIXES entidade[], int entidade_MAX);
void colisaoPlayerTiro(TIRO tiro[], int tiro_MAX);
void colisaoEntidadeTiro(PEIXES entidade[], int entidade_MAX, int altura_entidade, int largura_entidade, int tiro_MAX, TIRO tiro[], int isPlayer);
int checkColisaoEntidades(PEIXES entidade1, PEIXES entidade2);
void checkEncontrosEntidades(PEIXES entidade1[], int entidade1_MAX, PEIXES entidade2[], int entidade2_MAX);
void colisaoPessoaEntidade(PEIXES peixes[], int tamanhoVetor, int alturaPx, int larguraPx);
void colisaoPessoaPlayer();
void animacaoDano();
void colisoes();
void aumentarVelEntidades();
void updatePlayer();
void updateTiro();
void updateEntidade(PEIXES entidade[], int entidade_MAX, int largura_entidade, int tick_entidade);
void updatePessoa();
void updateMorto();
void update();
void iniciarPlayer();
void iniciarEntidade(PEIXES entidade[], int entidade_MAX, int tipo_ataque, int tipo);
void iniciarTiros();
void iniciarPessoas();
void iniciarMorto();
void iniciar();
void resetEntidades();
void resetTiros();
void reset();
void limparBufferTeclado();
void mudarTela(int tela);

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char *argv[])
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    srand((unsigned)time(NULL));

    iniciar();

    while (1)
    {
        update();
        Sleep(DELAY);
    }

    return 0;
}

void update()
{
    int relogioAtual = relogioGlobal;
    if (telaAtual == TELA_INICIAL)
    {
        desenhaTelaInicial();
        acaoTela(TELA_JOGO, VK_CONTROL);
        Sleep(50);
        limparBufferTeclado();
    }

    if (telaAtual == TELA_JOGO)
    {

        if (salvando && !morrendo)
        {
            salvarPessoa();
            Sleep(400);
        }

        if (morrendo)
        {
            PLAYER_SPRITE = PLAYER_MORTO;
            animacaoDano();
        }

        if (!salvando && !morrendo)
        {
            gerenciarSpawns();
            updatePlayer();
            aumentarVelEntidades();
            updatePessoa();
            animacaoEntidades();
            updateEntidade(peixe, PEIXE_MAX, LARGURA_PEIXE, TICK_PEIXE);
            updateEntidade(tubarao, TUBARAO_MAX, LARGURA_TUBARAO, TICK_TUBARAO);
            updateEntidade(inimigo, INIMIGO_MAX, LARGURA_INIMIGO, TICK_INIMIGO);
            updateTiro();
            colisoes();
            updateMorto();
        }

        acaoTela(TELA_INICIAL, VK_ESCAPE);
        desenhaTela();
    }

    if (telaAtual == TELA_GAMEOVER)
    {
        desenhaTelaGameOver();
        acaoTela(TELA_INICIAL, VK_CONTROL);
        Sleep(50);
        limparBufferTeclado();
    }

    relogioGlobal++;
}

// ============================================================================
// RENDERIZAÇÃO E DESENHOS
// ============================================================================

void desenhaTela()
{
    desenhaMapa();
    desenhaVida();
    desenhaScore();
    desenhaBarraOxigenio();
    desenhaPessoasSalvas();
    desenhaPlayer();
    desenhaPessoa();
    desenharEntidades(peixe, PEIXE_MAX);
    desenharEntidades(tubarao, TUBARAO_MAX);
    desenharEntidades(inimigo, INIMIGO_MAX);
    desenhaTiro();
    desenhaMorto();

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

void desenhaMapa()
{
    for (int i = 0; i < TELA_LARGURA * TELA_ALTURA; i++)
    {
        consoleBuffer[i].Char.AsciiChar = ' ';

        if (i < TELA_LARGURA * ALTURA_CEU)
        {
            consoleBuffer[i].Attributes = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            continue;
        }

        if (i >= TELA_LARGURA * (TELA_ALTURA - 1))
        {
            consoleBuffer[i].Attributes = BACKGROUND_GREEN | BACKGROUND_RED;
            continue;
        }

        if (i >= TELA_LARGURA * (TELA_ALTURA - ALTURA_CHAO))
        {
            consoleBuffer[i].Attributes = BACKGROUND_GREEN;
            continue;
        }

        consoleBuffer[i].Attributes = BACKGROUND_BLUE;
    }
}

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

    char textoIniciar[35];
    sprintf(textoIniciar, "PRESSIONE CONTROL PARA INICIAR");

    int inicio = (ALTURA_LOGO + LOGO_Y + 1) * (TELA_LARGURA) + 48;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    char textoComandos[43];
    sprintf(textoComandos, "COMANDOS: Space -> Atirar, Setas -> Mover");

    inicio = (ALTURA_LOGO + LOGO_Y + 2) * (TELA_LARGURA) + 42;
    for (int i = 0; textoComandos[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoComandos[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

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

    char textoIniciar[35];
    sprintf(textoIniciar, "PRESSIONE CONTROL PARA VOLTAR AO MENU");

    int inicio = (ALTURA_GAMEOVER + GAME_OVER_Y + 1) * (TELA_LARGURA) + 35;
    for (int i = 0; textoIniciar[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = textoIniciar[i];
        consoleBuffer[inicio + i].Attributes = FOREGROUND_BLUE;
    }

    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
}

void desenhaScore()
{
    char textoScore[30];
    sprintf(textoScore, "Score: %d", player.score);

    int inicio = 10;
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

    int inicio = TELA_LARGURA - 25;
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

    int inicio = 40;
    for (int i = 0; barraOxigenio[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = barraOxigenio[i];
        consoleBuffer[inicio + i].Attributes = corBarraOx;
    }
}

void desenhaPessoasSalvas()
{
    int frameAtual = relogioGlobal % 3;

    WORD cor = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_RED;

    if (frameAtual == 0 && player.pessoasSalvas == 5)
    {
        cor = FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED;
    }

    char barras[5];
    int indice = 0;
    for (int i = 0; i < 5; i++)
    {
        if (i < player.pessoasSalvas)
        {
            barras[indice] = 'O';
            indice++;
            continue;
        }
        barras[indice] = ' ';
        indice++;
    }
    barras[5] = '\0';

    char barra[18];
    sprintf(barra, "PESSOAS: [%s]", barras);

    int inicio = TELA_LARGURA + 45;
    for (int i = 0; barra[i] != '\0'; i++)
    {
        consoleBuffer[inicio + i].Char.AsciiChar = barra[i];
        consoleBuffer[inicio + i].Attributes = cor;
    }
}

void desenhaPlayer()
{
    int frameAtualPlayer;
    if (!morrendo)
    {
        frameAtualPlayer = (relogioGlobal / VELOCIDADE_ANIMACAO_PLAYER) % TOTAL_FRAMES_JOGADOR;
    }
    else
    {   
        PLAYER_SPRITE = PLAYER_MORTO;
        frameAtualPlayer = frameMorte;
    }

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
                    if (player.y <= ALTURA_CEU - 1 && i < 1)
                    {
                        consoleBuffer[indice].Attributes = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
                        continue;
                    }
                    consoleBuffer[indice].Attributes = player.cor;
                }
            }
        }
    }
}

void desenhaPessoa()
{
    int frameAtualPessoa = (relogioGlobal / VELOCIDADE_ANIMACAO_PESSOA) % TOTAL_FRAMES_PESSOA;

    for (int p = 0; p < MAX_PESSOAS; p++)
    {
        if (pessoas[p].vivo)
        {
            for (int i = 0; i < ALTURA_PESSOA; i++)
            {
                for (int j = 0; j < LARGURA_PESSOA; j++)
                {

                    int posX = pessoas[p].x + j;
                    int posY = pessoas[p].y + i;

                    int indice = posY * TELA_LARGURA + posX;

                    if (!(posX < 0 || posX > TELA_LARGURA || posY < 0 || posY > TELA_ALTURA))
                    {
                        char caractere = PESSOA_SPRITE[frameAtualPessoa][i][j];

                        if (caractere != ' ')
                        {
                            consoleBuffer[indice].Char.AsciiChar = caractere;
                            consoleBuffer[indice].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE;
                        }
                    }
                }
            }
        }
    }
}

void desenhaMorto()
{
    for (int m = 0; m < MORTO_MAX; m++)
    {
        if (morto[m].ativo)
        {
            for (int i = 0; i < ALTURA_MORTO; i++)
            {
                for (int j = 0; j < LARGURA_MORTO; j++)
                {
                    int posX = morto[m].x + j;
                    int posY = morto[m].y + i;

                    if (!(posX < 0 || posX > TELA_LARGURA || posY < 0 || posY > TELA_ALTURA))
                    {
                        char caractere = ICON_MORTO[i][j];

                        if (caractere != ' ')
                        {
                            consoleBuffer[posY * TELA_LARGURA + posX].Char.AsciiChar = caractere;

                            if (morto[m].y <= ALTURA_CEU - 1 && i < 1)
                            {
                                consoleBuffer[posY * TELA_LARGURA + posX].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
                                continue;
                            }
                            consoleBuffer[posY * TELA_LARGURA + posX].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE;
                        }
                    }
                }
            }
        }
    }
}

void desenharEntidades(PEIXES entidade[], int entidade_max)
{
    for (int e = 0; e < entidade_max; e++)
    {
        if (!entidade[e].vivo)
        {
            continue;
        }

        int alturaEntidade = entidade[e].altura;
        int larguraEntidade = entidade[e].largura;

        for (int i = 0; i < alturaEntidade; i++)
        {
            for (int j = 0; j < larguraEntidade; j++)
            {

                int px = entidade[e].x + j;
                int py = entidade[e].y + i;

                if (px >= 0 && px < TELA_LARGURA && py >= 0 && py < TELA_ALTURA)
                {
                    char caractere = entidade[e].sprite[i][j];

                    if (caractere != ' ')
                    {
                        int indice = py * TELA_LARGURA + px;
                        consoleBuffer[indice].Char.AsciiChar = caractere;
                        if (entidade[e].y <= ALTURA_CEU - 1 && i < 1)
                        {
                            consoleBuffer[indice].Attributes = entidade[e].cor | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
                            continue;
                        }
                        consoleBuffer[indice].Attributes = entidade[e].cor | BACKGROUND_BLUE;
                    }
                }
            }
        }
    }
}

void desenhaTiro()
{
    for (int i = 0; i < MAX_TIRO; i++)
    {
        TIRO tiro = tiros[i];
        if (tiro.ativo)
        {
            int posX = tiro.x,
                posY = tiro.y,
                indice = posY * TELA_LARGURA + posX;

            if (posX < TELA_LARGURA && posX > 0)
            {
                consoleBuffer[indice].Char.AsciiChar = ICON_TIRO;
                consoleBuffer[indice].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
            }
        }
    }

    for (int i = 0; i < MAX_TIRO_INIMIGO; i++)
    {
        TIRO tiro = tirosInimigo[i];
        if (tiro.ativo)
        {
            int posX = tiro.x,
                posY = tiro.y,
                indice = posY * TELA_LARGURA + posX;

            if (posX < TELA_LARGURA && posX > 0)
            {
                consoleBuffer[indice].Char.AsciiChar = ICON_TIRO;
                consoleBuffer[indice].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
            }
        }
    }
}

// ============================================================================
// ATUALIZAÇÕES GERAIS (UPDATES E ANIMAÇÕES)
// ============================================================================

void updatePlayer()
{
    if (player.vida <= 0)
        telaAtual = TELA_GAMEOVER;

    acoesPlayer();

    if (player.nivelOxigenio == 0)
    {
        morrendo = 1;
        resetEntidades();
        resetTiros();
    }

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

    if (player.nivelOxigenio < 0)
        player.nivelOxigenio = 0;

    if (player.nivelOxigenio > NIVEL_MAX_OXIGENIO)
        player.nivelOxigenio = NIVEL_MAX_OXIGENIO;

    if (player.y < ALTURA_CEU && relogioGlobal % TICK_OXIGENIO == 0)
    {
        player.respirando = 1;
        player.nivelOxigenio += NIVEL_MAX_OXIGENIO * 0.02;
    }
    else if (relogioGlobal % TICK_OXIGENIO == 0)
    {
        player.respirando = 0;
        player.nivelOxigenio -= NIVEL_MAX_OXIGENIO * 0.004;
    }

    if (relogioGlobal % 10 == 0)
    {
        player.cor = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
    }

    if (player.pessoasSalvas >= 5 && player.respirando)
    {
        salvando = 1;
    }

    if (player.score == player.numVida && player.score > 0 && player.vida < 10)
    {
        player.vida += 1;
        player.numVida += NUMERO_AUMENTA_VIDA;
    }
}

void updatePessoa()
{
    for (int p = 0; p < MAX_PESSOAS; p++)
    {
        if (relogioGlobal % TICK_PEIXE == 0)
        {
            if (pessoas[p].vivo && pessoas[p].lado == 1)
            {
                pessoas[p].x--;
            }
            else if (pessoas[p].vivo)
            {
                pessoas[p].x++;
            }
        }

        if (pessoas[p].x > TELA_LARGURA + LARGURA_PESSOA || pessoas[p].x < -LARGURA_PESSOA)
        {
            pessoas[p].vivo = 0;
        }
    }
}

void updateEntidade(PEIXES entidade[], int entidade_MAX, int largura_entidade, int tick_entidade)
{
    for (int e = 0; e < entidade_MAX; e++)
    {
        if (entidade[e].vivo && entidade[e].vida <= 0)
        {
            entidade[e].vivo = 0;
            matarEntidade(entidade[e].x, entidade[e].y);

            if (entidade[e].tipo_ataque == 1)
            {
                entidade[e].intervalo_ataque = INTERVALO_TIRO;
                if (entidade[e].indice_ataque >= 0 && entidade[e].indice_ataque < MAX_TIRO_INIMIGO)
                {
                    tirosInimigo[entidade[e].indice_ataque].ativo = 0;
                }
            }
        }
        if (entidade[e].vivo)
        {

            if (relogioGlobal % tick_entidade == 0)
                entidade[e].x += entidade[e].dx;

            if (entidade[e].x <= 0 - largura_entidade || entidade[e].x > TELA_LARGURA + largura_entidade)
            {
                entidade[e].vivo = 0;
            }

            if (entidade[e].tipo_ataque == 1)
            {
                if (entidade[e].intervalo_ataque > 0)
                {
                    entidade[e].intervalo_ataque--;
                }
                else if (tirosInimigo[entidade[e].indice_ataque].ativo == 0)
                {
                    int slotTiro = entidade[e].indice_ataque;
                    if (slotTiro >= 0 && slotTiro < MAX_TIRO_INIMIGO && !tirosInimigo[slotTiro].ativo)
                    {
                        tirosInimigo[entidade[e].indice_ataque].ativo = 1;
                    }
                }
            }
        }
    }
}

void updateTiro()
{
    acaoTiro();
    acaoTiroInimigo();
    for (int i = 0; i < MAX_TIRO; i++)
    {
        if (tiros[i].ativo)
        {
            tiros[i].x += tiros[i].dx;
            if (tiros[i].x < 0 || tiros[i].x > TELA_LARGURA)
            {
                tiros[i].ativo = 0;
            }
        }
    }

    for (int i = 0; i < MAX_TIRO_INIMIGO; i++)
    {
        if (tirosInimigo[i].ativo)
        {
            tirosInimigo[i].x += tirosInimigo[i].dx;
            if (tirosInimigo[i].x < 0 || tirosInimigo[i].x > TELA_LARGURA)
            {
                tirosInimigo[i].ativo = 0;

                for (int ini = 0; ini < INIMIGO_MAX; ini++)
                {
                    if (inimigo[ini].indice_ataque == i)
                    {
                        inimigo[ini].intervalo_ataque = INTERVALO_TIRO;
                    }
                }
            }
        }
    }
}

void updateMorto()
{
    for (int m = 0; m < MORTO_MAX; m++)
    {
        if (morto[m].timer <= 0)
        {
            morto[m].ativo = 0;
        }
        else
        {
            morto[m].timer--;
        }
    }
}

void aumentarVelEntidades()
{
    int dificuldade = player.score / 2000;

    if (dificuldade == 0)
    {
        TICK_PEIXE = 5;
        TICK_TUBARAO = 5;
        TICK_INIMIGO = 5;
    }
    else if (dificuldade == 1)
    {
        TICK_PEIXE = 4;
        TICK_TUBARAO = 4;
        TICK_INIMIGO = 4;
    }
    else if (dificuldade == 3)
    {
        TICK_PEIXE = 3;
        TICK_TUBARAO = 3;
        TICK_INIMIGO = 3;
    }
    else if (dificuldade == 4)
    {
        TICK_PEIXE = 2;
        TICK_TUBARAO = 2;
        TICK_INIMIGO = 2;
    }
    else if (dificuldade >= 6)
    {
        TICK_PEIXE = 1;
        TICK_TUBARAO = 1;
        TICK_INIMIGO = 1;
    }
}

void animacaoEntidades()
{

    int framePeixe = (relogioGlobal / VEL_ANIMACAO_PEIXE) % TOTAL_FRAMES_PEIXE;
    int frameTubarao = (relogioGlobal / VEL_ANIMACAO_TUBARAO) % TOTAL_FRAMES_TUBARAO;
    int frameInimigo = (relogioGlobal / VEL_ANIMACAO_INIMIGO) % TOTAL_FRAMES_INIMIGO;

    for (int p = 0; p < PEIXE_MAX; p++)
    {
        if (peixe[p].vivo)
        {
            if (peixe[p].dx > 0)
            {
                peixe[p].sprite = (const char **)PEIXE_DIREITA[framePeixe];
            }
            else
            {
                peixe[p].sprite = (const char **)PEIXE_ESQUERDA[framePeixe];
            }
        }
    }

    for (int t = 0; t < TUBARAO_MAX; t++)
    {
        if (tubarao[t].vivo)
        {
            if (tubarao[t].dx > 0)
            {
                tubarao[t].sprite = (const char **)TUBARAO_DIREITA[frameTubarao];
            }
            else
            {
                tubarao[t].sprite = (const char **)TUBARAO_ESQUERDA[frameTubarao];
            }
        }
    }

    for (int i = 0; i < INIMIGO_MAX; i++)
    {
        if (inimigo[i].vivo)
        {
            if (inimigo[i].dx > 0)
            {
                inimigo[i].sprite = (const char **)INIMIGO_DIREITA[frameInimigo];
            }
            else
            {
                inimigo[i].sprite = (const char **)INIMIGO_ESQUERDA[frameInimigo];
            }
        }
    }
}

void animacaoDano()
{
    if (!primeiroFrame)
    {
        frameMorte++;
    }

    if (frameMorte > 2)
    {
        frameMorte = 0;
        morrendo = 0;
        PLAYER_SPRITE = PLAYER_DIREITA;
        primeiroFrame = 0;
        levarDano();
    }

    primeiroFrame = 0;

    Sleep(500);
}

// ============================================================================
// FÍSICA E COLISÕES
// ============================================================================

void colisoes()
{
    colisaoPlayerEntidade(peixe, PEIXE_MAX);
    colisaoPlayerEntidade(tubarao, TUBARAO_MAX);
    colisaoPlayerEntidade(inimigo, INIMIGO_MAX);

    colisaoPlayerTiro(tirosInimigo, MAX_TIRO_INIMIGO);

    colisaoEntidadeTiro(peixe, PEIXE_MAX, ALTURA_PEIXE, LARGURA_PEIXE, MAX_TIRO, tiros, 1);
    colisaoEntidadeTiro(tubarao, TUBARAO_MAX, ALTURA_TUBARAO, LARGURA_TUBARAO, MAX_TIRO, tiros, 1);
    colisaoEntidadeTiro(inimigo, INIMIGO_MAX, ALTURA_INIMIGO, LARGURA_INIMIGO, MAX_TIRO, tiros, 1);

    colisaoEntidadeTiro(peixe, PEIXE_MAX, ALTURA_PEIXE, LARGURA_PEIXE, MAX_TIRO_INIMIGO, tirosInimigo, 0);
    colisaoEntidadeTiro(tubarao, TUBARAO_MAX, ALTURA_TUBARAO, LARGURA_TUBARAO, MAX_TIRO_INIMIGO, tirosInimigo, 0);

    colisaoPessoaEntidade(tubarao, TUBARAO_MAX, ALTURA_TUBARAO, LARGURA_TUBARAO);
    colisaoPessoaEntidade(peixe, PEIXE_MAX, ALTURA_PEIXE, LARGURA_PEIXE);
    colisaoPessoaEntidade(inimigo, INIMIGO_MAX, ALTURA_INIMIGO, LARGURA_INIMIGO);
    colisaoPessoaPlayer();

    checkEncontrosEntidades(tubarao, TUBARAO_MAX, peixe, PEIXE_MAX);
    checkEncontrosEntidades(tubarao, TUBARAO_MAX, inimigo, INIMIGO_MAX);
    checkEncontrosEntidades(inimigo, INIMIGO_MAX, peixe, PEIXE_MAX);
}

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
            resetTiros();
            morrendo = 1;

            player.cor = FOREGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
        }
    }
}

void colisaoPlayerTiro(TIRO tiro[], int tiro_MAX)
{
    for (int t = 0; t < tiro_MAX; t++)
    {
        if (!tiro[t].ativo)
        {
            continue;
        }

        if (player.y < tiro[t].y + 1 && player.y + ALTURA_PLAYER > tiro[t].y &&
            player.x < tiro[t].x + 1 && player.x + LARGURA_PLAYER > tiro[t].x)
        {
            morrendo = 1;
            tiro[t].ativo = 0;

            player.cor = FOREGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        }
    }
}

void colisaoEntidadeTiro(PEIXES entidade[], int entidade_MAX, int altura_entidade, int largura_entidade, int tiro_MAX, TIRO tiro[], int isPlayer)
{
    for (int t = 0; t < tiro_MAX; t++)
    {
        if (tiro[t].ativo)
        {
            for (int e = 0; e < entidade_MAX; e++)
            {
                if (entidade[e].vivo)
                {
                    if (entidade[e].y < tiro[t].y + 1 && entidade[e].y + altura_entidade > tiro[t].y &&
                        entidade[e].x < tiro[t].x + 1 && entidade[e].x + largura_entidade > tiro[t].x)
                    {
                        entidade[e].vida--;
                        tiro[t].ativo = 0;
                        if (isPlayer && entidade[e].vida <= 0)
                        {
                            player.score += 50;
                        }

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

void colisaoPessoaEntidade(PEIXES peixes[], int tamanhoVetor, int alturaPx, int larguraPx)
{

    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        if (pessoas[i].vivo)
        {
            for (int j = 0; j < tamanhoVetor; j++)
            {
                if (pessoas[i].x < peixes[j].x + larguraPx &&
                    pessoas[i].x + LARGURA_PESSOA > peixes[j].x &&
                    pessoas[i].y < peixes[j].y + alturaPx &&
                    pessoas[i].y + ALTURA_PESSOA > peixes[j].y)
                {
                    if (peixes[j].tipo > 0)
                    {
                        pessoas[i].vivo = 0;
                    }
                    else
                    {
                        if (peixes[j].dx < 0)
                        {
                            pessoas[i].x = peixes[j].x - LARGURA_PESSOA;
                        }
                        else
                        {
                            pessoas[i].x = peixes[j].x + larguraPx;
                        }
                    }

                    break;
                }
            }
        }
    }
}

void colisaoPessoaPlayer()
{
    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        if (player.x + LARGURA_PLAYER > pessoas[i].x &&
            player.x < pessoas[i].x + LARGURA_PESSOA &&
            player.y + ALTURA_PLAYER > pessoas[i].y &&
            player.y < pessoas[i].y + ALTURA_PESSOA && pessoas[i].vivo == 1)
        {
            pessoas[i].vivo = 0;

            if (player.pessoasSalvas < 5)
                player.pessoasSalvas++;
        }
    }
}

int checkColisaoEntidades(PEIXES entidade1, PEIXES entidade2)
{
    int colisaoX = (entidade1.x < entidade2.x + entidade2.largura) && (entidade1.x + entidade1.largura > entidade2.x);
    int colisaoY = (entidade1.y < entidade2.y + entidade2.altura) && (entidade1.y + entidade1.altura > entidade2.y);

    return colisaoX && colisaoY;
}

void checkEncontrosEntidades(PEIXES entidade1[], int entidade1_MAX, PEIXES entidade2[], int entidade2_MAX)
{
    for (int e1 = 0; e1 < entidade1_MAX; e1++)
    {
        if (!entidade1[e1].vivo)
        {
            continue;
        }

        for (int e2 = 0; e2 < entidade2_MAX; e2++)
        {
            if (!entidade2[e2].vivo)
            {
                continue;
            }

            if (checkColisaoEntidades(entidade1[e1], entidade2[e2]))
            {
                entidade2[e2].vivo = 0;
            }
        }
    }
}

// ============================================================================
// AÇÕES, COMBATE E ESTADOS DO JOGO
// ============================================================================

void acoesPlayer()
{
    if (relogioGlobal % TICK_PLAYER == 0)
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
}

void acaoTiro()
{
    if (GetAsyncKeyState(VK_SPACE))
    {
        for (int i = 0; i < MAX_TIRO; i++)
        {
            if (!tiros[i].ativo)
            {
                tiros[i].ativo = 1;
                tiros[i].x = (PLAYER_SPRITE == PLAYER_DIREITA) ? player.x + POS_TIRO_D : player.x + POS_TIRO_E;
                tiros[i].y = player.y + 1;
                tiros[i].dx = (PLAYER_SPRITE == PLAYER_DIREITA) ? VEL_TIRO : -VEL_TIRO;

                break;
            }
        }
    }
}

void acaoTiroInimigo()
{
    for (int ini = 0; ini < INIMIGO_MAX; ini++)
    {
        if (inimigo[ini].vivo && inimigo[ini].intervalo_ataque <= 0 && !(inimigo[ini].y <= ALTURA_CEU - 1))
        {
            int i = inimigo[ini].indice_ataque;

            if (i >= 0 && i < MAX_TIRO_INIMIGO && !tirosInimigo[i].ativo)
            {
                tirosInimigo[i].ativo = 1;
                tirosInimigo[i].x = (inimigo[ini].dx > 0) ? inimigo[ini].x + POS_TIRO_D : inimigo[ini].x + POS_TIRO_E;
                tirosInimigo[i].y = inimigo[ini].y + 1;
                tirosInimigo[i].dx = (inimigo[ini].dx > 0) ? VEL_TIRO_INIMIGO : -VEL_TIRO_INIMIGO;

                inimigo[ini].intervalo_ataque = INTERVALO_TIRO;
            }
        }
    }
}

void levarDano()
{
    player.vida--;
    resetEntidades();
    resetTiros();
    player.x = 64 - LARGURA_PLAYER;
    player.y = ALTURA_CEU;
    player.nivelOxigenio = 1000;
    player.pessoasSalvas = 0;
}

void matarEntidade(int posX, int posY)
{
    for (int i = 0; i < MORTO_MAX; i++)
    {
        if (!morto[i].ativo)
        {
            morto[i].ativo = 1;
            morto[i].timer = 3;
            morto[i].x = posX;
            morto[i].y = posY;
            break;
        }
    }
}

void salvarPessoa()
{
    player.pessoasSalvas--;
    player.score += 250;

    if (player.pessoasSalvas <= 0)
    {
        salvando = 0;
        resetEntidades();
        resetTiros();
        player.nivelOxigenio = 1000;
    }
}

void acaoTela(int tela, int tecla)
{
    if (GetAsyncKeyState(tecla))
    {
        mudarTela(tela);
        iniciar();
    }
}

void mudarTela(int tela)
{
    telaAtual = tela;
    reset();
}

// ============================================================================
// SISTEMA DE SPAWN E GERENCIAMENTO
// ============================================================================

void gerenciarSpawns()
{
    int sorteio = rand() % 100;

    if (sorteio < 10)
    {
        spawnarInimigo();
    }
    else if (sorteio < 20)
    {
        spawnarTubarao();
    }
    else
    {
        spawnarPeixes();
    }

    spawnarPessoa();
}

void spawnarPessoa()
{
    if (rand() % 60 != 0)
    {
        return;
    }

    int indice = rand() % MAX_PESSOAS;

    if (pessoas[indice].vivo)
        return;

    if (rand() % 2 == 0)
    {
        pessoas[indice].lado = 0; // 0 -> nasce à esquerda e vai para a direita
        pessoas[indice].x = 0 - LARGURA_PESSOA;
    }
    else
    {
        pessoas[indice].lado = 1; // 1 -> nasce à direita e vai para a esquerda
        pessoas[indice].x = TELA_LARGURA + LARGURA_PESSOA;
    }

    pessoas[indice].vivo = 1;
    pessoas[indice].y = TELA_ALTURA - ALTURA_PESSOA - ALTURA_CHAO; // posiciona próximo ao chão

    if (player.pessoasSalvas >= 5 && player.respirando)
    {
        salvando = 1;
    }
}

void spawnarPeixes()
{
    if (rand() % 4 != 0)
    {
        return;
    }

    int peixesLivres = 0;

    for (int p = 0; p < PEIXE_MAX; p++)
    {
        if (!peixe[p].vivo)
        {
            peixesLivres++;
        }
    }
    if (!peixesLivres)
    {
        return;
    }

    int cardume = peixesLivres;
    if (cardume >= 3)
    {
        cardume = 3;
    };

    int tamanhoCardumeFinal, alturaBaseFinal, ladoNascerFinal;

    int posicaoLivre = 0;
    int tentativas = 0;

    while (tentativas < 5)
    {

        int tamanhoCardume = rand() % cardume + 1;

        int alturaMin = 4;
        int alturaMax = TELA_ALTURA - 2 - (tamanhoCardume * ALTURA_PEIXE);
        if (alturaMax < alturaMin)
        {
            alturaMax = alturaMin;
        }

        int alturaBase = alturaMin + (rand() % (alturaMax - alturaMin + 1));

        int ladoNascer = (rand() % 2 == 0);

        posicaoLivre = 1;

        for (int p = 0; p < PEIXE_MAX; p++)
        {
            if (peixe[p].vivo)
            {
                if (abs(alturaBase - peixe[p].y) < (tamanhoCardume * ALTURA_PEIXE + 1))
                {
                    posicaoLivre = 0;
                    break;
                }
            }
        }

        if (posicaoLivre == 1)
        {
            tamanhoCardumeFinal = tamanhoCardume;
            alturaBaseFinal = alturaBase;
            ladoNascerFinal = ladoNascer;
            break;
        }

        tentativas++;
    }

    if (posicaoLivre == 0)
    {
        return;
    }

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

            if (peixesNascidos >= tamanhoCardumeFinal)
            {
                break;
            }
        }
    }
}

void spawnarTubarao()
{
    if (rand() % 4 != 0)
    {
        return;
    }

    int alturaBaseFinal, ladoNascerFinal;

    int posicaoLivre = 0;
    int tentativas = 0;

    while (tentativas < 5)
    {

        int alturaMin = 4;
        int alturaMax = TELA_ALTURA - 2 - ALTURA_TUBARAO;
        if (alturaMax < alturaMin)
        {
            alturaMax = alturaMin;
        }

        int alturaBase = alturaMin + (rand() % (alturaMax - alturaMin + 1));

        int ladoNascer = (rand() % 2 == 0);

        posicaoLivre = 1;

        for (int t = 0; t < TUBARAO_MAX; t++)
        {
            if (tubarao[t].vivo)
            {
                if (abs(alturaBase - tubarao[t].y) < (ALTURA_TUBARAO + 1))
                {
                    posicaoLivre = 0;
                    break;
                }
            }
        }

        if (posicaoLivre == 1)
        {
            alturaBaseFinal = alturaBase;
            ladoNascerFinal = ladoNascer;
            break;
        }

        tentativas++;
    }

    if (posicaoLivre == 0)
    {
        return;
    }

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

void spawnarInimigo()
{

    if (rand() % 4 != 0)
    {
        return;
    }

    int alturaBaseFinal, ladoNascerFinal;

    int posicaoLivre = 0;
    int tentativas = 0;

    while (tentativas < 5)
    {
        int alturaMin = 3;
        int alturaMax = TELA_ALTURA - 4 - ALTURA_INIMIGO;
        if (alturaMax == alturaMin)
        {
            alturaMax = alturaMin;
        }

        int alturaBase = alturaMin + (rand() % (alturaMax - alturaMin + 1));

        int ladoNascer = (rand() % 2 == 0);

        posicaoLivre = 1;

        for (int i = 0; i < INIMIGO_MAX; i++)
        {
            if (inimigo[i].vivo)
            {
                if (abs(alturaBase - inimigo[i].y) < (ALTURA_INIMIGO + 1))
                {
                    posicaoLivre = 0;
                    break;
                }
            }
        }

        if (posicaoLivre == 1)
        {
            alturaBaseFinal = alturaBase;
            ladoNascerFinal = ladoNascer;
            break;
        }

        tentativas++;
    }

    if (posicaoLivre == 0)
    {
        return;
    }

    for (int i = 0; i < INIMIGO_MAX; i++)
    {
        if (!inimigo[i].vivo)
        {
            int slotTiro = i % MAX_TIRO_INIMIGO;

            inimigo[i].vivo = 1;
            inimigo[i].vida = 4;
            inimigo[i].y = alturaBaseFinal;
            inimigo[i].indice_ataque = slotTiro;

            inimigo[i].altura = ALTURA_INIMIGO;
            inimigo[i].largura = LARGURA_INIMIGO;
            inimigo[i].tipo_ataque = 1;
            inimigo[i].intervalo_ataque = INTERVALO_TIRO;

            WORD corInimigo = FOREGROUND_RED | FOREGROUND_GREEN;
            inimigo[i].cor = corInimigo;

            if (ladoNascerFinal)
            {
                inimigo[i].x = 0 - LARGURA_INIMIGO;
                inimigo[i].dx = 2;
            }
            else
            {
                inimigo[i].x = TELA_LARGURA + LARGURA_INIMIGO;
                inimigo[i].dx = -2;
            }

            break;
        }
    }
}

// ============================================================================
// INICIALIZAÇÃO E RESETS
// ============================================================================

void iniciar()
{
    iniciarPlayer();
    iniciarEntidade(peixe, PEIXE_MAX, 0, 0);
    iniciarEntidade(tubarao, TUBARAO_MAX, 0, 1);
    iniciarEntidade(inimigo, INIMIGO_MAX, 1, 2);
    iniciarTiros();
    iniciarMorto();
    iniciarPessoas();
}

void iniciarPlayer()
{
    player.x = 64 - LARGURA_PLAYER;
    player.y = ALTURA_CEU;
    player.nivelOxigenio = 1000;
    player.vida = 5;
    player.numVida = NUMERO_AUMENTA_VIDA;
    player.score = 0;
    player.respirando = 0;
    player.pessoasSalvas = 0;
    player.cor = FOREGROUND_RED | BACKGROUND_BLUE | FOREGROUND_INTENSITY;
}

void iniciarPessoas()
{
    for (int p = 0; p < MAX_PESSOAS; p++)
    {
        pessoas[p].vivo = 0;
        pessoas[p].x = 0;
        pessoas[p].y = 0;
    }
}

void iniciarTiros()
{
    for (int i = 0; i < MAX_TIRO; i++)
    {
        tiros[i].ativo = 0;
    }
    for (int i = 0; i < MAX_TIRO_INIMIGO; i++)
    {
        tirosInimigo[i].ativo = 0;
    }
}

void iniciarMorto()
{
    for (int i = 0; i < MORTO_MAX; i++)
    {
        morto[i].ativo = 0;
    }
}

void iniciarEntidade(PEIXES entidade[], int entidade_MAX, int tipo_ataque, int tipo)
{
    for (int e = 0; e < entidade_MAX; e++)
    {
        entidade[e].vivo = 0;

        if (tipo_ataque == 1)
        {
            entidade[e].intervalo_ataque = INTERVALO_TIRO;
            entidade[e].indice_ataque = -1;
        }

        entidade[e].tipo = tipo;
    }
}

void reset()
{
    resetEntidades();
    resetTiros();
    iniciarPlayer();
    iniciarEntidade(peixe, PEIXE_MAX, 0, 0);

    iniciarEntidade(tubarao, TUBARAO_MAX, 0, 1);
    iniciarEntidade(inimigo, INIMIGO_MAX, 1, 2);

    iniciarTiros();
    iniciarMorto();
}

void resetEntidades()
{
    for (int i = 0; i < TUBARAO_MAX; i++)
    {
        tubarao[i].vivo = 0;
        tubarao[i].x = 0;
    }

    for (int i = 0; i < PEIXE_MAX; i++)
    {
        peixe[i].vivo = 0;
        peixe[i].x = 0;
    }

    for (int i = 0; i < INIMIGO_MAX; i++)
    {
        inimigo[i].vivo = 0;
        inimigo[i].x = 0;
    }
    
    for (int i = 0; i < MAX_PESSOAS; i++)
    {
        pessoas[i].vivo = 0;
        pessoas[i].x = 0;
    }
}

void resetTiros()
{
    for (int t = 0; t < MAX_TIRO; t++)
    {
        tiros[t].ativo = 0;
        tirosInimigo[t].ativo = 0;
    }
}

// ============================================================================
// UTILITÁRIOS E FUNÇÕES AUXILIARES
// ============================================================================

void limparBufferTeclado()
{
    if (kbhit())
        getch();
}