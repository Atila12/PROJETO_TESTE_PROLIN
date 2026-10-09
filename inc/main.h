/*
 * main.h
 *
 * Header para o modulo principal (menu e inicializacao).
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <xui.h>
#include <osal.h>
#include <stdbool.h>

/* ===================== DEFINES ===================== */

/* Tamanhos de fonte */
#define FONT_SIZE_12    12
#define FONT_SIZE_14    14
#define FONT_SIZE_16    16
#define FONT_SIZE_18    18
#define FONT_SIZE_20    20
#define FONT_SIZE_22    22
#define FONT_SIZE_24    24
#define FONT_SIZE_26    26
#define FONT_SIZE_28    28
#define FONT_SIZE_30    30
#define FONT_SIZE_32    32

/* Modos de tela */
#define CLEAR_SCREEN    0
#define NO_CLEAR_SCREEN 1

/* ===================== VARIAVEIS GLOBAIS ===================== */

/* Fontes */
extern XuiFont *font_bold;
extern XuiFont *font_medium;
extern XuiFont *font_verdana;

/* Cores */
extern XuiColor COLOR_WHITE;
extern XuiColor COLOR_BLACK;
extern XuiColor COLOR_GREEN;
extern XuiColor COLOR_RED;
extern XuiColor COLOR_BLUE;
extern XuiColor COLOR_GRAY;
extern XuiColor COLOR_TRANSPARENT;
extern XuiColor COLOR_INDIGO1;
extern XuiColor COLOR_INDIGO2;
extern XuiColor COLOR_INDIGO3;
extern XuiColor COLOR_BACKGROUND;

/* Canvas */
extern XuiWindow *canvas_main;

/* ===================== FUNCOES ===================== */

/**
 * Inicializa o Xui com rotacao e statusbar.
 * @param statusbar_height Altura da statusbar em pixels.
 * @return 0 em caso de sucesso, -1 em caso de falha.
 */
int GuiInit(int statusbar_height);

/**
 * Inicializa as cores globais do projeto.
 */
void colorInit(void);

/**
 * Inicializa as fontes globais do projeto.
 */
void fontInit(void);

/**
 * Libera as fontes globais.
 */
void fontDestroy(void);

/**
 * Limpa a tela principal (RootCanvas).
 */
void clearScreen(void);

/**
 * Escreve texto centralizado na tela.
 * @param canvas    Canvas onde desenhar.
 * @param text      Texto a ser exibido.
 * @param pos_y     Posicao Y do texto.
 * @param mode      CLEAR_SCREEN ou NO_CLEAR_SCREEN.
 * @param fontSize  Tamanho da fonte.
 * @param font      Ponteiro para a fonte.
 * @param color_black  true = texto preto, false = texto branco.
 */
void writeCenterText(XuiWindow *canvas, char *text, int pos_y, int mode,
                     int fontSize, XuiFont *font, bool color_black);

/**
 * Desenha o menu principal com as 3 opcoes.
 */
void drawMainMenu(void);

/**
 * Loop principal do menu.
 * @return 0 ao sair normalmente.
 */
int menuLoop(void);

#endif /* MAIN_H_ */
