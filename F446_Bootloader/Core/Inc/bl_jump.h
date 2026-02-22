/*
 * bl_jump.h
 *
 *  Created on: Jan 9, 2026
 *      Author: Yash
 */

#ifndef INC_BL_JUMP_H_
#define INC_BL_JUMP_H_

#include <stdint.h>

void JumpToApplication(void);
int bootloader_app_is_valid(void);

#endif /* INC_BL_JUMP_H_ */
