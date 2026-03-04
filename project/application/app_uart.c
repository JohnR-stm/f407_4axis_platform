/**
 * Copyright (c) 2026 IvanRomanov
 * Licensed under the MIT License. See LICENSE file in the project root.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


//--- temp include ---//
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_usart.h"
   
//-------------------//

#include "app_uart.h"
#include "led_hw.h"

#include "uart.h"

static char string1[] = "Hello! I RECEIVED YOUR MESSAGE! \r\n";

void dma_init(void);

//-----------------------------------------------------------------------------

#define RX_BUF_SIZE 64
uint8_t rx_dma_buffer[RX_BUF_SIZE]; // for DMA
uint16_t last_pos = 0;              // last processed position


//-----------------------------------------------------------------------------


MB_ASCII_t modbus_cmd;

const char *addr = "11";

const char *code[] = {
  "01",
  "02",
  "03"
};

//-----------------------------------------------

typedef void (*command_func_t)(void); 

typedef struct {
    const char *command_str;   // command
    command_func_t function;   // function
} Command_t;

//---------------------------------------------- 

const Command_t command_table[] = {
    {"red ld on",  led_red_off},
    {"red ld off", led_red_on},
    {"gr ld on",   led_green_off},
    {"gr ld off",  led_green_on}
};

#define CMD_COUNT (sizeof(command_table) / sizeof(Command_t))

//----------------------------------------------


//-----------------------------------------------------------------------------
// DMA configurations
//-----------------------------------------------------------------------------

void dma_init(void)
{
  
  /* DMA controller clock enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
  
   /* DMA Config */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_2, LL_DMA_CHANNEL_4);
  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_2, LL_DMA_PRIORITY_LOW);
  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_BYTE);
  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_2, LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_2);
 
  /* DMA Interrupts  */
  /* DMA2_Stream2_IRQn interrupt configuration */
 // NVIC_SetPriority(DMA2_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
 // NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  
  /* DMA memory */
  LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_2, 
                         LL_USART_DMA_GetRegAddr(USART1), 
                         (uint32_t)rx_dma_buffer, 
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, RX_BUF_SIZE);

  /* Enable DMA and interrupt */
  
  
  LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);
  LL_USART_EnableDMAReq_RX(USART1);

  
}


//-----------------------------------------------------------------------------
// uart1 HANDLER
//-----------------------------------------------------------------------------

void uart1_handler(void)
{

  
    //led_green_blink();
    
    MB_fill_buffer(&modbus_cmd);
    

  //uint8_t val = uart1_read_byte(); // read byte
  //if (val == 'v')
  //  uart1_send_string(string1);  
}


//-----------------------------------------------------------------------------
// Fill Buffer
//-----------------------------------------------------------------------------


void MB_fill_buffer(MB_ASCII_t *mb)
{
  ///--- current DMA position ---///
  uint16_t current_pos = RX_BUF_SIZE - LL_DMA_GetDataLength(DMA2, LL_DMA_STREAM_2);

  ///--- start from last position ---///
  while(last_pos != current_pos)
  {
    uint8_t byte = rx_dma_buffer[last_pos];
    
    ///--- beginning of the package ---///
    if (byte == ':') 
    {
      mb->pos = 0;
      mb->frameReady = false;
    }
    
    mb->buffer[mb->pos++] = byte;
    
    if (byte == 0x0D && mb->pos > 1) 
    {
      ///--- comand processing ---///
      mb->frameReady = true;
      MB_check_command(mb);
    }

    last_pos++;
    if (last_pos >= RX_BUF_SIZE) last_pos = 0;
    if (mb->pos >= MB_BUF_SIZE) mb->pos = 0;    /// Overflow protection
  }
}


//-----------------------------------------------------------------------------
// Check Command
//-----------------------------------------------------------------------------


void MB_check_command(MB_ASCII_t *mb) {
    if (!mb->frameReady) return;

    // change 0x0D to '\0' 
    mb->buffer[mb->pos - 1] = '\0'; 

    ///--- skip character ---///
    char *received_data = (char *)&mb->buffer[1];
    
    for (int i = 0; i < CMD_COUNT; i++) 
    {
      //--- check strings ---//
      size_t cmd_len = strlen(command_table[i].command_str);
      size_t str_len = strlen(received_data);
      if ( (strncmp(received_data, command_table[i].command_str, str_len) == 0)
          &&
           (cmd_len == str_len)) 
      {
        command_table[i].function(); // start a function
        break;
      }
    }

    mb->frameReady = false; // ?????????? ???? ????? ?????????
    mb->pos = 0;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

