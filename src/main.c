#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

//Constantes da tela
#define TELA_LARGURA 125
#define TELA_ALTURA  25
#define DELAY        90
#define ALTURA_CEU   4

//Enzo Capitani: Criação do player

#define ALTURA_PLAYER              2
#define LARGURA_PLAYER             9
#define VELOCIDADE_X_PLAYER        2
#define VELOCIDADE_Y_PLAYER        1
#define TOTAL_FRAMES_JOGADOR       3
#define VELOCIDADE_ANIMACAO_PLAYER 1 // as velocidades de animação são inversamente proporcionais ao seus defines
#define NIVEL_MAX_OXIGENIO         1000

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
#define VEL_ANIMACAO_PEIXE         8
#define PEIXE_MAX                  10
#define PEIXE_ACERELADO            1

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
#define VEL_ANIMACAO_TUBARAO       1
#define TUBARAO_MAX                5
#define TUBARAO_ACELERADO          1

/*
    Enzo Emanoel: Sprites iniciais do tubarão
*/
const char *TUBARAO_DIREITA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "\\____)\\____",
        "/-v___ __`< ",
        "     )/     "
    },
    {
        "\\____)\\____",
        "/-v___ __`= ",
        "     )/     "
    }
};

const char *TUBARAO_ESQUERDA[TOTAL_FRAMES_TUBARAO][ALTURA_TUBARAO] = {
    {
        "____/(____/ ",
        " >`__ ___v-\\",
        "    )/      ",
    },
    {
        "____/(____/ ",
        " =`__ ___v-\\",
        "    )/      ",
    }
};

// Struct das peixes
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

//Coisas do buffer
HANDLE hConsole;
CHAR_INFO consoleBuffer[TELA_LARGURA * TELA_ALTURA];
COORD bufferSize = {TELA_LARGURA, TELA_ALTURA};
COORD bufferCoord = {0, 0};
SMALL_RECT consoleWriteArea = {0, 0, TELA_LARGURA-1, TELA_ALTURA-1};

//Inicialização dos sons
Mix_Chunk* somTiro = NULL;
Mix_Chunk* somAlerta = NULL;
Mix_Chunk* somRespirando = NULL;
Mix_Chunk* somDano = NULL;

int relogioGlobal = 0;

//------------------------------- Métodos de sons -----------------------------------------------

void carregarSom(Mix_Chunk** som, char caminho[]){
    *som = Mix_LoadWAV(caminho);
    
    if(*som == NULL){
        printf("ERRO AO CARREGAR SOM: %s\n", Mix_GetError());
    }
}

void tocarSomCanalEspecifico(Mix_Chunk* som, int canal){
    if(som != NULL){

        if(canal >= 0 && Mix_Playing(canal)) return;

        Mix_PlayChannel(canal, som, 0);
    }
}

void tocarSom(Mix_Chunk* som)
{
    if(som != NULL){
        Mix_PlayChannel(-1, som, 0);
    }
}

void pararSom(int canal)
{
    Mix_HaltChannel(canal);
}

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
            tubarao[t].sprite = (const char **)TUBARAO_DIREITA[frameTubarao];
        } else {
            tubarao[t].sprite = (const char **)TUBARAO_ESQUERDA[frameTubarao]; 
        }
    }
}

// ---------------------------------- Métodos de desenhos ----------------------------------

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
    int frameAtual = relogioGlobal % 3;

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
    WriteConsoleOutputA(hConsole, consoleBuffer, bufferSize, bufferCoord, &consoleWriteArea);
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
                corPeixe = FOREGROUND_RED | (rand() % 2 ? FOREGROUND_GREEN | FOREGROUND_INTENSITY : 0) | FOREGROUND_INTENSITY;
            } while (corPeixe == peixe[p].cor);
            
            peixe[p].cor = corPeixe;

            if (ladoNascerFinal)
            {
                peixe[p].x = 0 - LARGURA_PEIXE;
                peixe[p].dx = 1;
            }
            else
            {
                peixe[p].x = TELA_LARGURA + LARGURA_PEIXE;
                peixe[p].dx = -1;
            }
            
            peixesNascidos++;

            if (peixesNascidos >= tamanhoCardumeFinal) { break; }
        }
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

                    tocarSom(somTiro);

                    break;
                }
            }
    }
}

// ---------------------------------- Métodos de colisões ----------------------------------

void colisaoPlayerPeixe()
{
    for (int p = 0; p < PEIXE_MAX; p++)
    {
        if (player.x + LARGURA_PLAYER > peixe[p].x &&
             player.x < peixe[p].x + LARGURA_PEIXE &&
              player.y + ALTURA_PLAYER > peixe[p].y &&
               player.y < peixe[p].y + ALTURA_PEIXE && peixe[p].vivo == 1)
        {
            peixe[p].vivo = 0;
            peixe[p].x = 0;
            player.vida--;

            player.cor = FOREGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_INTENSITY;

            tocarSom(somDano);
        }
    }
}

void colisaoPeixeTiro()
{
    for (int t = 0; t < MAX_TIRO; t++)
    {
        if (tiros[t].ativo)
        {
            for (int p = 0; p < PEIXE_MAX; p++)
            {
                if (peixe[p].vivo)
                {
                    if (peixe[p].y < tiros[t].y + 1 && peixe[p].y + ALTURA_PEIXE > tiros[t].y &&
                        peixe[p].x < tiros[t].x + 1 && peixe[p].x + LARGURA_PEIXE > tiros[t].x)
                    {
                        peixe[p].vida--;
                        tiros[t].ativo = 0;
                        player.score += 100;
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

void colisoes()
{
    colisaoPlayerPeixe();
    colisaoPeixeTiro();
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
        if(player.nivelOxigenio < 1000 && player.respirando){
            tocarSomCanalEspecifico(somRespirando, 6);
        }else{
            pararSom(6);
        }
    }else{
        player.respirando = 0;
        player.nivelOxigenio -= NIVEL_MAX_OXIGENIO * 0.004;
    }

    if(player.nivelOxigenio < 250)
    {   
        tocarSomCanalEspecifico(somAlerta, 7);

    }

    if(player.respirando){
        pararSom(7);
    }

    if(relogioGlobal % 5 == 0){
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

void updatePeixe() {
    for (int p = 0; p < PEIXE_MAX; p++) {
        if (peixe[p].vivo && peixe[p].vida <= 0)
        {
            peixe[p].vivo = 0;
            matarEntidade(peixe[p].x, peixe[p].y);
        }
        if (peixe[p].vivo) {

            peixe[p].x += peixe[p].dx * PEIXE_ACERELADO;

            if (peixe[p].x <= 0 - LARGURA_PEIXE || peixe[p].x > TELA_LARGURA + LARGURA_PEIXE) {
                peixe[p].vivo = 0;
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
    spawnarPeixes();
    animacaoEntidades();
    updatePlayer();
    updatePeixe();
    updateTiro();
    updateMorto();
    colisoes();
    desenhaTela();

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

void iniciarPeixe()
{
    for (int p = 0; p < PEIXE_MAX; p++) {
        peixe[p].vivo = 0;
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

//---------------- Inicia os sons ----------------------

int iniciarSDL(){
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("Erro ao inicializar áudio do SDL: %s\n", SDL_GetError());
        return 1;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        SDL_Quit();
        return 1;
    }

    return 0;
}

void iniciarSons()
{
    carregarSom(&somTiro, "src/sons/tiro.wav");
    carregarSom(&somAlerta, "src/sons/Alerta1.wav");
    carregarSom(&somRespirando, "src/sons/respirando1.wav");
    carregarSom(&somDano, "src/sons/dano2.wav");
}

void iniciar()
{   
    iniciarSDL();
    iniciarSons();

    iniciarPlayer();
    iniciarPeixe();
    iniciarTiros();
    iniciarMorto();
}

// ---------------------------------- Main ----------------------------------

//acho que é o main
int main(int argc, char* argv[]){
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    srand((unsigned)time(NULL));

    iniciar();

    while(1)
    {   
        update();
        Sleep(DELAY);
    }

    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}