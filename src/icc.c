/*
 * icc.c
 *
 * Modulo ICC: deteccao de cartao de chip por contato.
 *
 * Fluxo:
 *   1. OsPiccOpen   -> Abre o slot ICC do usuario
 *   2. Exibe "INSIRA O CARTAO" na tela via XUI
 *   3. Loop de polling com OsIccDetect ate cartao ser inserido
 *      (ou CANCEL para sair)
 *   4. Exibe "INSERIDO" por 2 segundos
 *   5. OsPiccClose  -> Fecha o modulo ICC
 */

#include <stdio.h>
#include <string.h>
#include <osal.h>
#include <xui.h>

#include "main.h"
#include "icc.h"

/* Slot padrao do usuario para ICC */
#define ICC_SLOT  0

int icc_detect(void) {
    int ret;
    int screenW = XuiRootCanvas()->width;
    int headerH = 50;

    OsLog(LOG_DEBUG, "[ICC] Iniciando icc_detect");

    /* ---- Desenha a tela ICC ---- */
    clearScreen();

    /* Header */
    XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
    writeCenterText(XuiRootCanvas(), "ICC DETECT", 12,
                    NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

    /* Mensagem principal */
    writeCenterText(XuiRootCanvas(), "INSIRA O CARTAO", 150,
                    NO_CLEAR_SCREEN, FONT_SIZE_28, font_bold, 1);

    /* Instrucao de cancelamento */
    writeCenterText(XuiRootCanvas(), "[CANCEL] Voltar",
                    XuiRootCanvas()->height - 40,
                    NO_CLEAR_SCREEN, FONT_SIZE_16, font_medium, 1);

    /* ---- Abre o modulo ICC ---- */
    ret = OsIccOpen(ICC_SLOT);
    if (ret != RET_OK) {
        OsLog(LOG_ERROR, "[ICC] Falha ao abrir modulo ICC: %d", ret);
        writeCenterText(XuiRootCanvas(), "ERRO AO ABRIR ICC", 200,
                        CLEAR_SCREEN, FONT_SIZE_22, font_bold, 1);
        OsSleep(2000);
        return -1;
    }
    OsLog(LOG_DEBUG, "[ICC] Modulo ICC aberto com sucesso");

    /* ---- Loop de deteccao ---- */
    XuiClearKey();

    while (1) {
        /* Verifica se o usuario pressionou CANCEL */
        if (XuiHasKey()) {
            int key = XuiGetKey();
            if (key == XUI_KEYCANCEL) {
                OsLog(LOG_DEBUG, "[ICC] Cancelado pelo usuario");
                OsIccClose(ICC_SLOT);
                return -1;
            }
        }

        /* Tenta detectar o cartao */
        ret = OsIccDetect(ICC_SLOT);
        if (ret == RET_OK) {
            /* Cartao detectado! */
            OsLog(LOG_DEBUG, "[ICC] Cartao INSERIDO detectado");
            OsBeep(1, 200);

            /* Exibe mensagem de sucesso */
            clearScreen();
            XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
            writeCenterText(XuiRootCanvas(), "ICC DETECT", 12,
                            NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

            writeCenterText(XuiRootCanvas(), "INSERIDO", 150,
                            NO_CLEAR_SCREEN, FONT_SIZE_32, font_bold, 1);

            writeCenterText(XuiRootCanvas(), "Cartao detectado com sucesso!", 210,
                            NO_CLEAR_SCREEN, FONT_SIZE_18, font_medium, 1);

            OsSleep(2000);
            break;
        }

        /* Pequeno delay para nao sobrecarregar o polling */
        OsSleep(100);
    }

    /* ---- Fecha o modulo ICC ---- */
    OsIccClose(ICC_SLOT);
    OsLog(LOG_DEBUG, "[ICC] Modulo ICC fechado");

    return 0;
}
