/*
 *  @file  : spi.h
 *	
 *  @brief	
 *
 *  @author: franc
 *  @date  : 25 nov 2019
 */
#ifndef SPI_H_
#define SPI_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define SPIx                           SPI2

#define SPIx_CLK                       RCC_APB1Periph_SPI2
#define SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define SPIx_IRQn                      SPI2_IRQn
#define SPIx_IRQHANDLER                SPI2_IRQHandler

/* CLK - GPIOB 10*/
#define SPIx_SCK_PIN                   GPIO_Pin_10
#define SPIx_SCK_GPIO_PORT             GPIOB
#define SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPIx_SCK_SOURCE                GPIO_PinSource10
#define SPIx_SCK_AF                    GPIO_AF_SPI2

#if 0
#define SPIx_MISO_PIN                  GPIO_Pin_2
#define SPIx_MISO_GPIO_PORT            GPIOC
#define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPIx_MISO_SOURCE               GPIO_PinSource2
#define SPIx_MISO_AF                   GPIO_AF_SPI2
#endif

/* MOSI/SDA -  GPIOC 03 */
#define SPIx_MOSI_PIN                  GPIO_Pin_3
#define SPIx_MOSI_GPIO_PORT            GPIOC
#define SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPIx_MOSI_SOURCE               GPIO_PinSource3
#define SPIx_MOSI_AF                   GPIO_AF_SPI2

/* MISO -  GPIOC 02 */
#define SPIx_MISO_PIN                  GPIO_Pin_2
#define SPIx_MISO_GPIO_PORT            GPIOC
#define SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOC
#define SPIx_MISO_SOURCE               GPIO_PinSource2
#define SPIx_MISO_AF                   GPIO_AF_SPI2

/* CHIP SELECT - GPIOB 1*/
#define SPIx_CS_PIN                    GPIO_Pin_1
#define SPIx_CS_GPIO_PORT              GPIOB
#define SPIx_CS_GPIO_CLK               RCC_AHB1Periph_GPIOB

// CS pin macros
#define CS_L() GPIO_ResetBits(SPIx_CS_GPIO_PORT, SPIx_CS_PIN)
#define CS_H() GPIO_SetBits(SPIx_CS_GPIO_PORT, SPIx_CS_PIN)

#define BUFFERSIZE                     100	/* */

#define CS_LOW		GPIO_WriteBit(GPIOC, GPIO_Pin_2, 0)
#define CS_HIGH		GPIO_WriteBit(GPIOC, GPIO_Pin_2, 1)

#define SPI_DUMMY_WRITE			SPIx->DR = 0xFF;

extern void SPI_Config(void);
extern void spi_read(uint8_t* rx_buffer, uint16_t n_byte);
extern void spi_multiple_write(const uint8_t* tx_buffer, uint16_t n_byte);
extern void spi_single_write(uint8_t data);
extern uint8_t spi_exchange(uint8_t tx_byte);

#endif /* SPI_H_ */
