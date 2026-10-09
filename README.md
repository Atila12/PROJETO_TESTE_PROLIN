# icc-test-prolin

## Estrutura de arquivos

```
icc-test-prolin/
    README.md                Este documento

    src/
        main.c               Ponto de entrada, inicializacao, menu e helpers de tela
        icc.c                Fluxo de deteccao de cartao ICC (chip por contato)
        ctls.c               Fluxo de deteccao de cartao CTLS (contactless)
        mag.c                Fluxo de deteccao de cartao MAG (tarja magnetica)

    inc/
        main.h               Declaracoes globais, prototipos de funcoes auxiliares
        icc.h                Prototipo de icc_detect()
        ctls.h               Prototipo de ctls_detect()
        mag.h                Prototipo de mag_detect()

    res/font/                Fontes TTF usadas pela interface (Barlow, Verdana)
```

---

## Como funciona

Ao iniciar, a aplicacao abre o XUI, carrega fontes e cores, e apresenta um menu
com tres botoes na tela:

```
1 - ICC Detect
2 - CTLS Detect
3 - MAG Detect
```

O usuario seleciona uma opcao pela tecla numerica correspondente (1, 2 ou 3)
ou pelo toque no botao. Cada opcao abre o modulo de hardware apropriado,
exibe uma mensagem pedindo a insercao/aproximacao/passagem do cartao, e fica
em loop esperando a deteccao. Ao detectar, exibe uma mensagem de confirmacao
por 2 segundos, fecha o modulo e retorna ao menu.

A tecla CANCEL volta ao menu (dentro de qualquer tela de deteccao) ou encerra
a aplicacao (no menu principal).

---

## Fluxo do ICC (icc.c)

1. Desenha a tela com o titulo "ICC DETECT" e a mensagem "INSIRA O CARTAO".
2. Abre o slot ICC com `OsIccOpen`.
3. Entra em loop: a cada 100ms verifica se o cartao foi inserido com `OsIccDetect`
   e se o usuario pressionou CANCEL.
4. Ao detectar o cartao, emite um beep, exibe "INSERIDO" por 2 segundos.
5. Fecha o slot com `OsIccClose`.

O CTLS e o MAG seguem a mesma logica, trocando o modulo de hardware e a mensagem.

---

## Referencia das funcoes OSAL utilizadas

Todas as funcoes abaixo sao parte da camada OSAL (Operating System Abstraction Layer)
do Prolin, declaradas em `<osal.h>`.

### Sistema e log

| Funcao | Descricao |
|--------|-----------|
| `OsLogSetTag(const char *tag)` | Define a tag que aparece em todas as mensagens de log da aplicacao. Deve ser chamada uma vez no inicio. |
| `OsLog(int level, const char *fmt, ...)` | Grava uma mensagem no log do sistema. O parametro `level` aceita valores como `LOG_DEBUG` e `LOG_ERROR`. Funciona como `printf` com format string. |
| `OsSleep(int ms)` | Suspende a execucao da thread atual por `ms` milissegundos. Usado para delays e para evitar polling excessivo. |
| `OsSysSleepEx(int mode)` | Controla o modo de suspensao do terminal. Chamada com `0` acorda o terminal (liga a tela). |
| `OsRegGetValue(const char *key, char *value)` | Le um valor do registro do sistema. Usado aqui para obter a rotacao da tela (`ro.fac.lcd.rotate`). Retorna o tamanho do valor lido, ou negativo em caso de erro. |
| `OsSaveCrashReport` | Handler de sinal fornecido pelo sistema. Ao ser registrado com `signal()`, captura crashes (SIGSEGV, SIGBUS etc.) e gera um relatorio de crash no terminal para depuracao posterior. |
| `OsBeep(int count, int duration)` | Emite um beep sonoro. `count` define quantas vezes, `duration` define a duracao em milissegundos de cada beep. |

### ICC (cartao de chip por contato)

| Funcao | Descricao |
|--------|-----------|
| `OsIccOpen(int slot)` | Abre o modulo de leitura de cartao ICC no slot indicado. O slot `0` e o slot principal do usuario. Retorna `RET_OK` (0) em caso de sucesso. |
| `OsIccDetect(int slot)` | Verifica se existe um cartao inserido no slot ICC. Retorna `RET_OK` quando um cartao esta presente. Chamada repetidamente em loop para polling. |
| `OsIccClose(int slot)` | Fecha o modulo ICC do slot indicado e libera os recursos. Deve sempre ser chamada apos o uso, mesmo em caso de erro ou cancelamento. |

### PICC (cartao contactless / aproximacao)

| Funcao | Descricao |
|--------|-----------|
| `OsPiccOpen(int slot)` | Abre o modulo PICC (Proximity IC Card), que controla a antena NFC/contactless. Retorna `RET_OK` em caso de sucesso. |
| `OsPiccDetect(int slot, unsigned char *cardType, unsigned char *uid, unsigned char *uidLen)` | Tenta detectar um cartao no campo NFC. Quando um cartao e aproximado, preenche `cardType` com o tipo do cartao, `uid` com o identificador unico e `uidLen` com o tamanho do UID. Retorna `RET_OK` ao detectar. |
| `OsPiccClose(int slot)` | Fecha o modulo PICC e desliga a antena contactless. Deve ser chamada apos o uso. |

### MSR (leitor de tarja magnetica)

| Funcao | Descricao |
|--------|-----------|
| `OsMsrOpen(void)` | Abre o modulo de leitura de tarja magnetica. Ativa o leitor em modo de interrupcao, pronto para detectar passagem de cartao. Retorna `RET_OK` em caso de sucesso. |
| `OsMsrSwiped(void)` | Verifica se um cartao foi passado no leitor. Retorna `RET_OK` quando dados de tarja estao disponiveis. Chamada em loop para polling. |
| `OsMsrClose(void)` | Fecha o leitor de tarja magnetica e libera os recursos. |

---

## Referencia das funcoes XUI utilizadas

Todas as funcoes abaixo sao parte da biblioteca grafica XUI do Prolin,
declaradas em `<xui.h>`.

### Ciclo de vida

| Funcao | Descricao |
|--------|-----------|
| `XuiOpen(int argc, char **argv)` | Inicializa a biblioteca XUI. Aceita argumentos de configuracao como `ROTATE=0` (rotacao da tela) e `STATUSBAR=18` (altura da statusbar em pixels). Retorna `XUI_RET_OK` em caso de sucesso. Deve ser chamada uma unica vez no inicio da aplicacao. |
| `XuiClose()` | Encerra a biblioteca XUI e libera todos os recursos graficos. Deve ser a ultima chamada XUI antes de sair. |

### Canvas e tela

| Funcao | Descricao |
|--------|-----------|
| `XuiRootCanvas()` | Retorna o ponteiro para o canvas raiz (tela principal). Todas as operacoes de desenho sao feitas sobre esse canvas ou sobre canvas filhos criados a partir dele. O canvas retornado tem os campos `width` e `height` com as dimensoes da tela. |
| `XuiClearArea(XuiWindow *canvas, int x, int y, int w, int h)` | Limpa uma area retangular do canvas, preenchendo-a com a cor de fundo atual. Usada para apagar conteudo antes de redesenhar. |
| `XuiCanvasSetBackground(XuiWindow *canvas, int mode, XuiImg *img, XuiColor color)` | Define a cor ou imagem de fundo de um canvas. O `mode` pode ser `XUI_BG_CENTER`, `XUI_BG_NORMAL` etc. Quando `img` e `NULL`, usa a cor solida passada em `color`. |
| `XuiCanvasDrawRect(XuiWindow *canvas, int x, int y, int w, int h, XuiColor color, int border, int fill)` | Desenha um retangulo no canvas. `border` define a espessura da borda (0 para sem borda), `fill` indica se deve preencher o interior (1 = sim). Usado para desenhar o header e divisorias visuais. |
| `XuiCanvasDrawText(XuiWindow *canvas, int x, int y, int size, XuiFont *font, int style, XuiColor color, char *text)` | Desenha uma string no canvas na posicao (x, y). `size` e o tamanho da fonte em pixels, `style` geralmente e `XUI_TEXT_NORMAL`. A cor do texto e definida por `color`. |
| `XuiShowWindow(XuiWindow *window, int visible, int mode)` | Controla a visibilidade de um canvas ou botao. `visible=1` mostra, `visible=0` esconde. `mode` controla o tipo de transicao (0 para imediato). |
| `XuiDestroyWindow(XuiWindow *window)` | Destroi um canvas ou botao criado dinamicamente, liberando a memoria associada. |

### Fontes

| Funcao | Descricao |
|--------|-----------|
| `XuiCreateFont(const char *path, int index, int flags)` | Carrega uma fonte TrueType (.ttf) do caminho indicado. `index` e `flags` geralmente sao 0 para fontes simples. Retorna um ponteiro `XuiFont*` usado nas chamadas de desenho de texto. |
| `XuiDestroyFont(XuiFont *font)` | Libera uma fonte carregada. Deve ser chamada para cada fonte criada com `XuiCreateFont` antes de encerrar a aplicacao. |
| `XuiTextWidth(XuiFont *font, int size, const char *text)` | Calcula a largura em pixels que o texto ocuparia se desenhado com a fonte e tamanho indicados. Usado para centralizar texto horizontalmente. |

### Botoes

| Funcao | Descricao |
|--------|-----------|
| `XuiCreateButton(XuiWindow *parent, int x, int y, int w, int h)` | Cria um botao como filho do canvas `parent`, na posicao e dimensoes indicadas. Retorna um ponteiro `XuiWindow*`. |
| `XuiButtonSetStat(XuiWindow *btn, int state, XuiButtonStat *stat)` | Define a aparencia do botao para um estado. `XUI_BTN_NORMAL` e o estado padrao, `XUI_BTN_PRESSED` e quando pressionado. A struct `XuiButtonStat` contem campos como `btn_bg` (cor de fundo), `text` (texto do botao), `text_fg` (cor do texto), `text_font`, `text_height`, `text_x`, `text_y` e `btn_round` (raio de arredondamento dos cantos). |
| `XuiButtonSetKey(XuiWindow *btn, int keyValue)` | Associa uma tecla fisica ao botao. Quando o usuario pressiona essa tecla, o botao e ativado como se tivesse sido tocado. Valores comuns: `XUI_KEY1` a `XUI_KEY9`, `XUI_KEYENTER`, `XUI_KEYCANCEL`. |

### Teclado e eventos

| Funcao | Descricao |
|--------|-----------|
| `XuiClearKey()` | Limpa o buffer de teclas. Deve ser chamada antes de entrar em um loop de espera por tecla, para descartar teclas pressionadas anteriormente. |
| `XuiHasKey()` | Verifica se existe uma tecla no buffer. Retorna 1 se ha uma tecla disponivel, 0 caso contrario. Nao bloqueia — retorna imediatamente. |
| `XuiGetKey()` | Retorna o valor da proxima tecla no buffer. Deve ser chamada somente quando `XuiHasKey()` retornar 1. Valores comuns: `XUI_KEY1`..`XUI_KEY9`, `XUI_KEY0`, `XUI_KEYENTER`, `XUI_KEYCANCEL`, `XUI_KEYCLEAR`, `XUI_KEYFUNC`. |

### Macro auxiliar

| Macro | Descricao |
|-------|-----------|
| `XUI_CENTER_X(start, total, width)` | Calcula a posicao X para centralizar um elemento de largura `width` dentro de um espaco que vai de `start` ate `start + total`. Equivale a `start + (total - width) / 2`. |
