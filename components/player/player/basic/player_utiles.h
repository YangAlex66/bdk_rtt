/*
 * File: player_utiles.h
 * 
 * COPYRIGHT (C) 2012-2018, Shanghai Real-Thread Technology Co., Ltd
 */

#ifndef __PLAYER_UTILES_H__
#define __PLAYER_UTILES_H__

#include "player_config.h" 
#include "player_system.h" 
#include "stdint.h"

typedef void (*parse_key_val_cb)(void *context, const char *key, int key_len, char **dest, int *dest_len);

int player_strstart(const char *str, const char *pfx, const char **ptr); 
void player_parse_key_value(const char *str, parse_key_val_cb callback_get_buf, void *context); 
size_t player_strlcpy(char *dst, const char *src, size_t size); 
size_t player_strlcat(char *dst, const char *src, size_t size); 
void player_make_absolute_url(char *buf, int size, const char *base, const char *rel); 
void player_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem); 

#endif 
