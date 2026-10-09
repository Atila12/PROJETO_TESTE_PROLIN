/*
 * main.c
 *
 * Modulo principal: inicializacao do XUI, funcoes auxiliares
 * de desenho e loop do menu com 3 opcoes:
 *   1 - ICC Detect
 *   2 - CTLS Detect
 *   3 - MAG Detect
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <osal.h>
#include <xui.h>

#include "main.h"
#include "icc.h"
#include "ctls.h"
#include "mag.h"

/* ===================== VARIAVEIS GLOBAIS ===================== */

/* Fontes */
XuiFont *font_bold   = NULL;
XuiFont *font_medium = NULL;
XuiFont *font_verdana = NULL;

/* Canvas */
XuiWindow *canvas_main = NULL;

/* Cores */
XuiColor COLOR_WHITE;
XuiColor COLOR_BLACK;
XuiColor COLOR_GREEN;
XuiColor COLOR_RED;
XuiColor COLOR_BLUE;
XuiColor COLOR_GRAY;
XuiColor COLOR_TRANSPARENT;
XuiColor COLOR_INDIGO1;
XuiColor COLOR_INDIGO2;
XuiColor COLOR_INDIGO3;
XuiColor COLOR_BACKGROUND;

/* Botoes do menu */
static XuiWindow *btn_icc  = NULL;
static XuiWindow *btn_ctls = NULL;
static XuiWindow *btn_mag  = NULL;

/* ===================== INICIALIZACAO ===================== */

/**
 * Inicializa o XUI com rotacao e statusbar.
 */
int GuiInit(int statusbar_height) {
    char value[128];
    char rotate_str[32];
    char statusbar_str[32];
    int ret;
    char *xui_argv[10];
    int  xui_argc;

    ret = OsRegGetValue("ro.fac.lcd.rotate", value);
    if (ret > 0) {
        snprintf(rotate_str, sizeof(rotate_str), "ROTATE=%s", value);
    } else {
        strcpy(rotate_str, "ROTATE=0");
    }

    if (statusbar_height > 0) {
        snprintf(statusbar_str, sizeof(statusbar_str), "STATUSBAR=%d", statusbar_height);
    } else {
        strcpy(statusbar_str, "STATUSBAR=0");
    }

    xui_argv[0] = rotate_str;
    xui_argv[1] = statusbar_str;
    xui_argv[2] = NULL;
    xui_argc = 2;

    ret = XuiOpen(xui_argc, xui_argv);
    if (ret == XUI_RET_OK) {
        return 0;
    }
    return -1;
}

/**
 * Inicializa todas as cores do projeto.
 */
void colorInit(void) {
    COLOR_WHITE.r = 0xFF; COLOR_WHITE.g = 0xFF; COLOR_WHITE.b = 0xFF; COLOR_WHITE.a = 0xFF;
    COLOR_BLACK.r = 0x00; COLOR_BLACK.g = 0x00; COLOR_BLACK.b = 0x00; COLOR_BLACK.a = 0xFF;

    COLOR_GREEN.r = 0x43; COLOR_GREEN.g = 0xAC; COLOR_GREEN.b = 0x6A; COLOR_GREEN.a = 0xFF;
    COLOR_RED.r   = 0xF0; COLOR_RED.g   = 0x41; COLOR_RED.b   = 0x24; COLOR_RED.a   = 0xFF;
    COLOR_BLUE.r  = 0x00; COLOR_BLUE.g  = 0x00; COLOR_BLUE.b  = 0xFF; COLOR_BLUE.a  = 0xFF;

    COLOR_GRAY.r = 0xF0; COLOR_GRAY.g = 0xF0; COLOR_GRAY.b = 0xF0; COLOR_GRAY.a = 0xFF;

    COLOR_TRANSPARENT.r = 0; COLOR_TRANSPARENT.g = 0; COLOR_TRANSPARENT.b = 0; COLOR_TRANSPARENT.a = 0;

    /* Tons para o tema */
    COLOR_INDIGO1.r = 0x1A; COLOR_INDIGO1.g = 0x23; COLOR_INDIGO1.b = 0x3B; COLOR_INDIGO1.a = 0xFF;
    COLOR_INDIGO2.r = 0x2C; COLOR_INDIGO2.g = 0x3E; COLOR_INDIGO2.b = 0x6B; COLOR_INDIGO2.a = 0xFF;
    COLOR_INDIGO3.r = 0x3B; COLOR_INDIGO3.g = 0x52; COLOR_INDIGO3.b = 0x8A; COLOR_INDIGO3.a = 0xFF;

    COLOR_BACKGROUND.r = 0xF5; COLOR_BACKGROUND.g = 0xF5; COLOR_BACKGROUND.b = 0xF5; COLOR_BACKGROUND.a = 0xFF;
}

/**
 * Carrega as fontes do projeto.
 * As fontes devem estar na pasta res/font/.
 */
void fontInit(void) {
    font_bold    = XuiCreateFont("./res/font/Barlow-Bold.ttf", 0, 0);
    font_medium  = XuiCreateFont("./res/font/Barlow-Medium.ttf", 0, 0);
    font_verdana = XuiCreateFont("./res/font/verdana.ttf", 0, 0);
}

/**
 * Libera as fontes carregadas.
 */
void fontDestroy(void) {
    if (font_bold)    { XuiDestroyFont(font_bold);    font_bold    = NULL; }
    if (font_medium)  { XuiDestroyFont(font_medium);  font_medium  = NULL; }
    if (font_verdana) { XuiDestroyFont(font_verdana); font_verdana = NULL; }
}

/* ===================== FUNCOES AUXILIARES DE TELA ===================== */

/**
 * Limpa a tela inteira (RootCanvas).
 */
void clearScreen(void) {
    XuiClearArea(XuiRootCanvas(), 0, 0,
                 XuiRootCanvas()->width, XuiRootCanvas()->height);
    XuiCanvasSetBackground(XuiRootCanvas(), XUI_BG_CENTER, NULL, COLOR_BACKGROUND);
}

/**
 * Escreve texto centralizado horizontalmente na tela.
 */
void writeCenterText(XuiWindow *canvas, char *text, int pos_y, int mode,
                     int fontSize, XuiFont *font, bool color_black) {
    int iTextWidth;

    if (mode == CLEAR_SCREEN) {
        clearScreen();
    }

    iTextWidth = XuiTextWidth(font_medium, fontSize, text);
    if (iTextWidth > XuiRootCanvas()->width) {
        iTextWidth = XuiRootCanvas()->width;
    }

    if (color_black) {
        XuiCanvasDrawText(canvas,
            XUI_CENTER_X(0, XuiRootCanvas()->width, iTextWidth), pos_y,
            fontSize, font, XUI_TEXT_NORMAL, COLOR_BLACK, text);
    } else {
        XuiCanvasDrawText(canvas,
            XUI_CENTER_X(0, XuiRootCanvas()->width, iTextWidth), pos_y,
            fontSize, font, XUI_TEXT_NORMAL, COLOR_WHITE, text);
    }
}

/* ===================== MENU PRINCIPAL ===================== */

/**
 * Destroi os botoes do menu se existirem.
 */
static void destroyMenuButtons(void) {
    if (btn_icc)  { XuiDestroyWindow(btn_icc);  btn_icc  = NULL; }
    if (btn_ctls) { XuiDestroyWindow(btn_ctls); btn_ctls = NULL; }
    if (btn_mag)  { XuiDestroyWindow(btn_mag);  btn_mag  = NULL; }
}

/**
 * Desenha o menu principal com 3 botoes.
 * Layout:
 *   - Header com titulo "MENU PRINCIPAL" em fundo escuro
 *   - 3 botoes empilhados: ICC Detect, CTLS Detect, MAG Detect
 *   - Rodape com instrucao "[CANCEL] Sair"
 */
void drawMainMenu(void) {
    int screenW = XuiRootCanvas()->width;
    int screenH = XuiRootCanvas()->height;
    XuiButtonStat statNormal, statPressed;
    int btnW, btnH, btnX, btnY, spacing;
    int headerH = 50;
    int textWidth;

    /* Limpa a tela */
    clearScreen();

    /* ---- Header ---- */
    XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
    writeCenterText(XuiRootCanvas(), "MENU PRINCIPAL", 12,
                    NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

    /* ---- Configuracao dos botoes ---- */
    btnW = screenW - 40;    /* Margem de 20px de cada lado */
    btnH = 60;
    btnX = 20;
    spacing = 20;
    btnY = headerH + 40;

    /* Destroi botoes anteriores */
    destroyMenuButtons();

    /* ---- Botao 1: ICC Detect ---- */
    btn_icc = XuiCreateButton(XuiRootCanvas(), btnX, btnY, btnW, btnH);
    memset(&statNormal, 0, sizeof(statNormal));
    statNormal.btn_round = 8;
    statNormal.btn_bg = COLOR_INDIGO2;
    statNormal.text_fg = COLOR_WHITE;
    statNormal.text_font = font_verdana;
    statNormal.text_height = 20;
    statNormal.text = "1 - ICC Detect";
    textWidth = XuiTextWidth(font_verdana, 20, "1 - ICC Detect");
    statNormal.text_x = (btnW - textWidth) / 2;
    statNormal.text_y = (btnH - 20) / 2;
    XuiButtonSetStat(btn_icc, XUI_BTN_NORMAL, &statNormal);

    memset(&statPressed, 0, sizeof(statPressed));
    statPressed.btn_round = 8;
    statPressed.btn_bg = COLOR_INDIGO3;
    statPressed.text_fg = COLOR_WHITE;
    statPressed.text_font = font_verdana;
    statPressed.text_height = 22;
    statPressed.text = "1 - ICC Detect";
    statPressed.text_x = statNormal.text_x;
    statPressed.text_y = statNormal.text_y;
    XuiButtonSetStat(btn_icc, XUI_BTN_PRESSED, &statPressed);

    XuiButtonSetKey(btn_icc, XUI_KEY1);
    XuiShowWindow(btn_icc, 1, 0);

    /* ---- Botao 2: CTLS Detect ---- */
    btnY += btnH + spacing;
    btn_ctls = XuiCreateButton(XuiRootCanvas(), btnX, btnY, btnW, btnH);

    statNormal.text = "2 - CTLS Detect";
    textWidth = XuiTextWidth(font_verdana, 20, "2 - CTLS Detect");
    statNormal.text_x = (btnW - textWidth) / 2;
    XuiButtonSetStat(btn_ctls, XUI_BTN_NORMAL, &statNormal);

    statPressed.text = "2 - CTLS Detect";
    statPressed.text_x = statNormal.text_x;
    XuiButtonSetStat(btn_ctls, XUI_BTN_PRESSED, &statPressed);

    XuiButtonSetKey(btn_ctls, XUI_KEY2);
    XuiShowWindow(btn_ctls, 1, 0);

    /* ---- Botao 3: MAG Detect ---- */
    btnY += btnH + spacing;
    btn_mag = XuiCreateButton(XuiRootCanvas(), btnX, btnY, btnW, btnH);

    statNormal.text = "3 - MAG Detect";
    textWidth = XuiTextWidth(font_verdana, 20, "3 - MAG Detect");
    statNormal.text_x = (btnW - textWidth) / 2;
    XuiButtonSetStat(btn_mag, XUI_BTN_NORMAL, &statNormal);

    statPressed.text = "3 - MAG Detect";
    statPressed.text_x = statNormal.text_x;
    XuiButtonSetStat(btn_mag, XUI_BTN_PRESSED, &statPressed);

    XuiButtonSetKey(btn_mag, XUI_KEY3);
    XuiShowWindow(btn_mag, 1, 0);

    /* ---- Rodape ---- */
    writeCenterText(XuiRootCanvas(), "[CANCEL] Sair",
                    screenH - 40, NO_CLEAR_SCREEN, FONT_SIZE_16, font_medium, 1);
}

/**
 * Loop principal do menu.
 * Espera tecla pressionada e despacha para o modulo correto.
 */
int menuLoop(void) {
    int key;

    while (1) {
        drawMainMenu();
        XuiClearKey();

        /* Espera por tecla */
        while (!XuiHasKey()) {
            OsSleep(50);
        }

        key = XuiGetKey();

        if (key == XUI_KEY1) {
            /* ICC Detect */
            destroyMenuButtons();
            icc_detect();
        }
        else if (key == XUI_KEY2) {
            /* CTLS Detect */
            destroyMenuButtons();
            ctls_detect();
        }
        else if (key == XUI_KEY3) {
            /* MAG Detect */
            destroyMenuButtons();
            mag_detect();
        }
        else if (key == XUI_KEYCANCEL) {
            /* Sair do aplicativo */
            destroyMenuButtons();
            break;
        }
    }

    return 0;
}

/* ===================== PONTO DE ENTRADA ===================== */

/**
 * CrashReport: captura sinais fatais para gerar relatorio de crash.
 */
static void CrashReportInit(void) {
    signal(SIGABRT,   OsSaveCrashReport);
    signal(SIGBUS,    OsSaveCrashReport);
    signal(SIGFPE,    OsSaveCrashReport);
    signal(SIGSEGV,   OsSaveCrashReport);
    signal(SIGSTKFLT, OsSaveCrashReport);
    signal(SIGPIPE,   OsSaveCrashReport);
}

int main(int argc, char **argv) {

    OsLogSetTag("===== PROLIN ICC DEMO =====");

    /* 1. Inicializa o XUI */
    if (GuiInit(0) != 0) {
        OsLog(LOG_ERROR, "Falha ao inicializar o XUI");
        return -1;
    }

    /* 2. Crash report */
    CrashReportInit();

    /* 3. Inicializa cores e fontes */
    colorInit();
    fontInit();

    /* 4. Acorda o terminal */
    OsSysSleepEx(0);

    /* 5. Executa o loop do menu */
    menuLoop();

    /* 6. Libera recursos */
    fontDestroy();
    XuiClose();

    return 0;
}
