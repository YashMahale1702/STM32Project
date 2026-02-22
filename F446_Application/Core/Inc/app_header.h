/*
 * app_header.h
 *
 *  Created on: Jan 10, 2026
 *      Author: Yash
 */

#ifndef INC_APP_HEADER_H_
#define INC_APP_HEADER_H_

typedef struct {
	uint32_t ota_flag;
	uint32_t magic;
	uint32_t size;
	uint32_t crc;
	uint32_t version;
}AppHeader_t;

#endif /* INC_APP_HEADER_H_ */
