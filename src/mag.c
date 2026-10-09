/*
 * mag.c
 *
 * Modulo MAG: deteccao de cartao de tarja magnetica.
 *
 * Fluxo:
 *   1. OsMsrOpen    -> Abre o leitor de tarja magnetica
 *   2. Exibe "PASSE O CARTAO" na tela via XUI
 *   3. Loop de polling com OsMsrSwiped ate cartao ser passado
 *      (ou CANCEL para sair)
 *   4. Exibe "DETECTADO" por 2 segundos
 *   5. OsMsrClose   -> Fecha o leitor MSR
 */

#include <stdio.h>
#include <string.h>
#include <osal.h>
#include <xui.h>

#include "main.h"
#include "mag.h"

int mag_detect(void) {
    int ret;
    int screenW = XuiRootCanvas()->width;
    int headerH = 50;

    OsLog(LOG_DEBUG, "[MAG] Iniciando mag_detect");

    /* ---- Desenha a tela MAG ---- */
    clearScreen();

    /* Header */
    XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
    writeCenterText(XuiRootCanvas(), "MAG DETECT", 12,
                    NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

    /* Mensagem principal */
    writeCenterText(XuiRootCanvas(), "PASSE O CARTAO", 150,
                    NO_CLEAR_SCREEN, FONT_SIZE_28, font_bold, 1);

    /* Instrucao de cancelamento */
    writeCenterText(XuiRootCanvas(), "[CANCEL] Voltar",
                    XuiRootCanvas()->height - 40,
                    NO_CLEAR_SCREEN, FONT_SIZE_16, font_medium, 1);

    /* ---- Abre o modulo MSR ---- */
    ret = OsMsrOpen();
    if (ret != RET_OK) {
        OsLog(LOG_ERROR, "[MAG] Falha ao abrir modulo MSR: %d", ret);
        writeCenterText(XuiRootCanvas(), "ERRO AO ABRIR MAG", 200,
                        CLEAR_SCREEN, FONT_SIZE_22, font_bold, 1);
        OsSleep(2000);
        return -1;
    }
    OsLog(LOG_DEBUG, "[MAG] Modulo MSR aberto com sucesso");

    /* ---- Loop de deteccao ---- */
    XuiClearKey();

    while (1) {
        /* Verifica se o usuario pressionou CANCEL */
        if (XuiHasKey()) {
            int key = XuiGetKey();
            if (key == XUI_KEYCANCEL) {
                OsLog(LOG_DEBUG, "[MAG] Cancelado pelo usuario");
                OsMsrClose();
                return -1;
            }
        }

        /* Tenta detectar passagem de cartao */
        ret = OsMsrSwiped();
        if (ret == RET_OK) {
            /* Cartao passado detectado! */
            OsLog(LOG_DEBUG, "[MAG] Cartao MAG detectado (swipe)");
            OsBeep(1, 200);

            /* Exibe mensagem de sucesso */
            clearScreen();
            XuiCanvasDrawRect(XuiRootCanvas(), 0, 0, screenW, headerH, COLOR_INDIGO1, 0, 1);
            writeCenterText(XuiRootCanvas(), "MAG DETECT", 12,
                            NO_CLEAR_SCREEN, FONT_SIZE_22, font_bold, 0);

            writeCenterText(XuiRootCanvas(), "DETECTADO", 150,
                            NO_CLEAR_SCREEN, FONT_SIZE_32, font_bold, 1);

            writeCenterText(XuiRootCanvas(), "Cartao passado com sucesso!", 210,
                            NO_CLEAR_SCREEN, FONT_SIZE_18, font_medium, 1);

            OsSleep(2000);
            break;
        }

        /* Pequeno delay para nao sobrecarregar o polling */
        OsSleep(100);
    }

    /* ---- Fecha o modulo MSR ---- */
    OsMsrClose();
    OsLog(LOG_DEBUG, "[MAG] Modulo MSR fechado");

    return 0;
}
