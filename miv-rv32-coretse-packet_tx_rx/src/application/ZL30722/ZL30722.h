
/*
 * Copyright 2018 Microchip Corporation.  
 * All rights reserved.
 * 
 * ZL30722 configuration.
 */

#ifndef ZL30722_H_
#define ZL30722_H_

void ZL30722_SPI_init(void);
void ZL30722_read(uint8_t *buffer, const int length);
void ZL30722_write(const uint8_t *buffer, const int length);
void ZL30722_compare(const uint8_t *image, const int length);

#endif /* ZL30722_H_ */
