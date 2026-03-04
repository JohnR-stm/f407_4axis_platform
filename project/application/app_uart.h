/**
 * Copyright (c) 2026 IvanRomanov
 * Licensed under the MIT License. See LICENSE file in the project root.
 */

#ifndef _APP_UART_H_
#define _APP_UART_H_

//------------------------------------------

#define  MB_BUF_SIZE    20

typedef struct {
    uint8_t     buffer[MB_BUF_SIZE];
    uint8_t     pos;
    _Bool       frameReady;
} MB_ASCII_t;

//------------------------------------------

void dma_init(void);

void uart1_handler(void);


void MB_fill_buffer(MB_ASCII_t *mb);

void MB_check_command(MB_ASCII_t *mb);
   
//------------------------------------------


#endif /* _APP_UART_H_ */

