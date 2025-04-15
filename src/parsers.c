//  ***************************************************************************
/// @file    parsers.c
//  ***************************************************************************
#include "parsers.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


//  ***************************************************************************
/// @brief      Сheck for template match in string
/// @param      str - pointer, can't be NULL
/// @param      temp - pointer, can't be NULL
/// @return     bool
/// @note       Symbol '*' in template is any symbol.
//  ***************************************************************************
bool pars_is_there_template_in_string(const uint8_t *str, const uint8_t *temp) {
    uint32_t y;

    // skip first special symbols
    while(*temp == '*') temp++;

    y = 0;
    while (*str != '\0') {
        if ((temp[y] == '*') || (*str == temp[y])) {
            y++;
            if (temp[y] == '\0') return true;
        }
        else {
            y = 0;
        }
        str++;
    }
    return false;
}


//  ***************************************************************************
/// @brief      Get tokens from string
/// @param      str - pointer, can't be NULL
/// @param      delimiters - pointer, can't be NULL
/// @param      tokens_ptrs - pointer, can't be NULL
/// @param      max_tokens_qty
/// @param      actual_tokens_qty - pointer, can't be NULL
/// @retval     str
/// @retval     tokens_ptrs
/// @retval     actual_tokens_qty
/// @return     none
//  ***************************************************************************
void pars_get_tokens_from_string(uint8_t *str, const uint8_t *delimiters, const uint8_t **tokens_ptrs, uint32_t max_tokens_qty, uint32_t *actual_tokens_qty) {
    uint32_t i;


    tokens_ptrs[0] = (uint8_t*)strtok((char*)str, (char*)delimiters);
    i = 1;
    while ((tokens_ptrs[i - 1] != NULL) && (i < max_tokens_qty)) {
        tokens_ptrs[i] = (uint8_t*)strtok(NULL, (char*)delimiters);
        i++;
    }
    if (tokens_ptrs[i - 1] == NULL) i--;
    *actual_tokens_qty = i;
}


//  ***************************************************************************
/// @brief      Conver hex/dec string digit to S32
/// @param      str - pointer, can't be NULL
/// @param      digit - pointer, can't be NULL
/// @retval     digit
/// @return     bool
//  ***************************************************************************
bool pars_string_to_s32(const uint8_t *str, int32_t *digit) {
    char *end_ptr = NULL;


    // Hexadecimal format
    if ((str[0] == '0') && (str[1] == 'x')) {
        *digit = strtol((char*)str, &end_ptr, 16);
    }
    // Decimal format
    else {
        *digit = strtol((char*)str, &end_ptr, 10);
    }

    if (*end_ptr != '\0') return false;
    return true;
}


//  ***************************************************************************
/// @brief      Conver hex/dec string digit to U32
/// @param      str - pointer, can't be NULL
/// @param      digit - pointer, can't be NULL
/// @retval     digit
/// @return     bool
//  ***************************************************************************
bool pars_string_to_u32(const uint8_t *str, uint32_t *digit) {
    char *end_ptr = NULL;


    // Hexadecimal format
    if ((str[0] == '0') && (str[1] == 'x')) {
        *digit = strtoul((char*)str, &end_ptr, 16);
    }
    // Decimal format
    else {
        *digit = strtoul((char*)str, &end_ptr, 10);
    }

    if (*end_ptr != '\0') return false;
    return true;
}
