/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "fat_fs/ff.h"

#include "lib_uart.h"

#include "delay.h"

int main(void)
{
	UART_lib_config(e_UART_2, DISABLE, 0, 0);
	delay_init();

	sd_spi_init();

	while(1)
	{

	}
}
