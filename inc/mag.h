/*
 * mag.h
 *
 * Header para o modulo MAG (cartao de tarja magnetica).
 */

#ifndef MAG_H_
#define MAG_H_

/**
 * Executa o fluxo de deteccao de cartao MAG:
 *   1. Abre o modulo MSR (leitor de tarja magnetica)
 *   2. Exibe mensagem "PASSE O CARTAO"
 *   3. Aguarda o cartao ser passado (polling com OsMsrSwiped)
 *   4. Exibe mensagem "DETECTADO" ao passar
 *   5. Fecha o modulo MSR
 *
 * @return 0 em caso de sucesso, -1 em caso de erro ou cancelamento.
 */
int mag_detect(void);

#endif /* MAG_H_ */
