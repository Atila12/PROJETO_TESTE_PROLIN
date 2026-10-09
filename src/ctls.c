/*
 * ctls.c
 *
 * Modulo CTLS: deteccao de cartao por aproximacao (contactless / NFC).
 *
 * Fluxo:
 *   1. OsPiccOpen    -> Abre o modulo PICC (contactless)
 *   2. Exibe "APROXIME O CARTAO" na tela via XUI
 *   3. Loop de polling com OsPiccDetect ate cartao ser aproximado
 *      (ou CANCEL para sair)
 *   4. Exibe "DETECTADO" por 2 segundos
 *   5. OsPiccClose   -> Fecha o modulo PICC
 */

#include <stdio.h>
#include <string.h>
#include <osal.h>
#include <xui.h>

#include "main.h"
#include "ctls.h"

/* Slot padrao para PICC */
#define PICC_SLOT  0

int ctls_detect(void) {
    int ret;
    int screenW = XuiRootCanvas()->width;
    int headerH = 50;
    unsigned char cardType = 0;
    unsigned char uid[32];
    unsigned char uidLen = 0;

    memset(uid, 0, sizeof(uid));

    OsLog(LOG_DEBUG, "[CTLS] Iniciando ctls_detect");

    /* ---- Desenha a tela CTLS ---- */
    clearScreen();

    /* Header */
    XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
    writeCenterText(XuiRootCanvas(), "CTLS DETECT", 12,
                    NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

    /* Mensagem principal */
    writeCenterText(XuiRootCanvas(), "APROXIME O CARTAO", 150,
                    NO_CLEAR_SCREEN, FONT_SIZE_28, font_bold, 1);

    /* Instrucao de cancelamento */
    writeCenterText(XuiRootCanvas(), "[CANCEL] Voltar",
                    XuiRootCanvas()->height - 40,
                    NO_CLEAR_SCREEN, FONT_SIZE_16, font_medium, 1);

    /* ---- Abre o modulo PICC ---- */
    ret = OsPiccOpen(PICC_SLOT);
    if (ret != RET_OK) {
        OsLog(LOG_ERROR, "[CTLS] Falha ao abrir modulo PICC: %d", ret);
        writeCenterText(XuiRootCanvas(), "ERRO AO ABRIR CTLS", 200,
                        CLEAR_SCREEN, FONT_SIZE_22, font_bold, 1);
        OsSleep(2000);
        return -1;
    }
    OsLog(LOG_DEBUG, "[CTLS] Modulo PICC aberto com sucesso");

    /* ---- Loop de deteccao ---- */
    XuiClearKey();

    while (1) {
        /* Verifica se o usuario pressionou CANCEL */
        if (XuiHasKey()) {
            int key = XuiGetKey();
            if (key == XUI_KEYCANCEL) {
                OsLog(LOG_DEBUG, "[CTLS] Cancelado pelo usuario");
                OsPiccClose(PICC_SLOT);
                return -1;
            }
        }

        /* Tenta detectar o cartao por aproximacao */
        ret = OsPiccDetect(PICC_SLOT, &cardType, uid, &uidLen);
        if (ret == RET_OK) {
            /* Cartao detectado! */
            OsLog(LOG_DEBUG, "[CTLS] Cartao CTLS detectado (tipo=%d, uidLen=%d)",
                  cardType, uidLen);
            OsBeep(1, 200);

            /* Exibe mensagem de sucesso */
            clearScreen();
            XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
            writeCenterText(XuiRootCanvas(), "CTLS DETECT", 12,
                            NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

            writeCenterText(XuiRootCanvas(), "DETECTADO", 150,
                            NO_CLEAR_SCREEN, FONT_SIZE_32, font_bold, 1);

            writeCenterText(XuiRootCanvas(), "Cartao aproximado com sucesso!", 210,
                            NO_CLEAR_SCREEN, FONT_SIZE_18, font_medium, 1);

            OsSleep(2000);
            break;
        }

        /* Pequeno delay para nao sobrecarregar o polling */
        OsSleep(100);
    }

    /* ---- Fecha o modulo PICC ---- */
    OsPiccClose(PICC_SLOT);
    OsLog(LOG_DEBUG, "[CTLS] Modulo PICC fechado");

    return 0;
}
