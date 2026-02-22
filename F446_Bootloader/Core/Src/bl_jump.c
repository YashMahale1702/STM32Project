/*
 * bl_jump.c
 *
 *  Created on: Jan 9, 2026
 *      Author: Yash
 */

#include "main.h"
#include "bl_jump.h"
#include "flash_layout.h"
#include "app_header.h"
//typedef void (*pFunction)(void);

#define APP_MAGIC 0xABCDEFAB

void JumpToApplication(void) {

  /* Read application stack pointer */
  uint32_t appStackPointer = *(volatile uint32_t *)APP_START_ADDR;

  /* Read reset handler address */
  uint32_t appResetHandler = *(volatile uint32_t *)(APP_START_ADDR + 4);


  /* Disable interrupts */
  __disable_irq();

  /* Stop SysTick */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* Set main stack pointer */
  __set_MSP(appStackPointer);

  // Set the VTOR
  SCB->VTOR = APP_START_ADDR;

  /* Jump to application reset handler */
  // appResetHandler contains the address, so typecast it to func_pointer and call it
  ((void (*)(void))appResetHandler)();
}

int bootloader_app_is_valid(void){
	const AppHeader_t *app_header = (AppHeader_t *)APP_HEADER_ADDR;

	// check the magic number
	if(app_header->magic != APP_MAGIC){
		return 1;
	}

	uint32_t reset_handler = *((uint32_t *)(APP_START_ADDR + 4));
	if((reset_handler & 0xFF000000) != 0x08000000){
		return 2;
	}

	return 0;
}
