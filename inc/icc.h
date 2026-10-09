/*
 * icc.h
 *
 * Header para o modulo ICC (cartao de chip por contato).
 */

#ifndef ICC_H_
#define ICC_H_

/**
 * Executa o fluxo de deteccao de cartao ICC:
 *   1. Abre o modulo ICC (slot do usuario)
 *   2. Exibe mensagem "INSIRA O CARTAO"
 *   3. Aguarda o cartao ser inserido (polling com OsPiccDetect/OsIccDetect)
 *   4. Exibe mensagem "INSERIDO" ao detectar
 *   5. Fecha o modulo ICC
 *
 * @return 0 em caso de sucesso, -1 em caso de erro ou cancelamento.
 */
int icc_detect(void);

#endif /* ICC_H_ */
