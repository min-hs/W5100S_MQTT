#ifndef __WIZCHIP_INIT_H__
#define __WIZCHIP_INIT_H__


#include "main.h"
#include "wizchip_conf.h"



/* CS */
/*
extern SPI_HandleTypeDef hspi2;
#define WIZCHIP_SPI  			hspi2
#define WIZCHIP_CS_PIN			GPIO_PIN_6
#define WIZCHIP_CS_PORT			GPIOB
*/

#define W5x00_RESET_PIN		    GPIO_PIN_7
#define W5x00_RESET_PORT		GPIOC

void resetAssert(void);
void resetDeassert(void);

void WIZCHIPInitialize();

void csEnable(void);
void csDisable(void);
void spiWriteByte(uint8_t tx);
uint8_t spiReadByte(void);

#endif
