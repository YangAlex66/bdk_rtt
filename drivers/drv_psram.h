#ifndef __DRV_PSRAM_H__
#define __DRV_PSRAM_H__

/*
 * line_mode      0: 1 line; 1: 4 line
 * voltage_level  0: 1.8v; 1: 2.5v; 2: 3.3v
 */
void psram_init(uint8_t line_mode,uint8_t voltage_level);
void *psram_malloc(unsigned long size);
void psram_free(void *ptr);
void *psram_calloc(unsigned int n, unsigned int size);
void *psram_realloc(void *ptr, unsigned long size);

#endif