/**
 * Copyright (c) 2026 IvanRomanov
 * Licensed under the MIT License. See LICENSE file in the project root.
 */

#include <stdint.h>
#include <stdio.h>

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

#define RX_BUF_SIZE 32
uint8_t rx_dma_buffer[RX_BUF_SIZE]; // for DMA
uint16_t last_pos = 0;              // last processed position


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


    led_green_blink();
    

  //uint8_t val = uart1_read_byte(); // read byte
  //if (val == 'v')
  //  uart1_send_string(string1);  
}


//-----------------------------------------------------------------------------
// DMA Interrupts handler
//-----------------------------------------------------------------------------
/*void DMA2_Stream2_IRQHandler(void)
{
  
}
*/


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
