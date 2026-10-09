/*
 * ctls.h
 *
 * Header para o modulo CTLS (cartao contactless / aproximacao).
 */

#ifndef CTLS_H_
#define CTLS_H_

/**
 * Executa o fluxo de deteccao de cartao CTLS:
 *   1. Abre o modulo PICC (contactless)
 *   2. Exibe mensagem "APROXIME O CARTAO"
 *   3. Aguarda o cartao ser aproximado (polling com OsPiccDetect)
 *   4. Exibe mensagem "DETECTADO" ao aproximar
 *   5. Fecha o modulo PICC
 *
 * @return 0 em caso de sucesso, -1 em caso de erro ou cancelamento.
 */
int ctls_detect(void);

#endif /* CTLS_H_ */
