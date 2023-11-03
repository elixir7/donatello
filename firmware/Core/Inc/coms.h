/*
 * coms.h
 *
 *  Created on: Nov 1, 2023
 *      Author: Isak
 */

#ifndef INC_COMS_H_
#define INC_COMS_H_

#include <stdint.h>

void coms_add_rx(uint8_t c);
void coms_add_tx(uint8_t c);
void coms_task(void const * argument);

//TODO: coms_trannsmit(const uint8_t * buffer, uint16_t len)


#endif /* INC_COMS_H_ */
