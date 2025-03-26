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
bool pars_is_there_template_in_string(uint8_t *str, const uint8_t *temp) {
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
/// @retval     tokens_ptrs
/// @retval     actual_tokens_qty
/// @return     none
/// @note       Symbol '*' in template is any symbol.
//  ***************************************************************************
void pars_get_tokens_from_string(uint8_t *str, uint8_t *delimiters, const uint8_t **tokens_ptrs, uint32_t max_tokens_qty, uint32_t *actual_tokens_qty) {
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
/// @brief      Get answer from string ("answer \n prompt" -> "answer")
/// @param      str - pointer, can't be NULL
/// @param      prompt - pointer, can't be NULL
/// @param      cmd_answer - pointer, can't be NULL
/// @retval     cmd_answer
/// @return     bool - false if string does not contain prompt
//  ***************************************************************************
bool pars_get_cmd_answer_from_string(uint8_t *str, const uint8_t *prompt, uint8_t **cmd_answer) {
    uint8_t *tokens_ptrs[10];
    uint32_t tokens_qty;
    int32_t i;


    pars_get_tokens_from_string(str, "\n", tokens_ptrs, (sizeof(tokens_ptrs) / sizeof(uint8_t*)), &tokens_qty);
    for (i = tokens_qty; i > 0; i--) {
        if (pars_is_there_template_in_string(tokens_ptrs[i], prompt)) break;
    }
    i--;
    if (i >= 0) {
        *cmd_answer = tokens_ptrs[i];
        return true;
    }

    return false;
}


//  ***************************************************************************
/// @brief      Get line from file
/// @param      file_pos - pointer, can't be NULL
/// @param      line - pointer, can't be NULL
/// @param      max_line_size
/// @retval     file_pos - new file position
/// @return     bool - false if end of file
//  ***************************************************************************
bool pars_get_line_from_file(const uint8_t **file_pos, uint8_t *line, uint32_t max_line_size) {
    if (**file_pos == 0x1A) {  // EOF
        return false;
    }
    while ((**file_pos == '\r') || (**file_pos == '\n')) (*file_pos)++;
    while ((**file_pos != 0x1A) && (**file_pos != '\r') && (**file_pos != '\n')) {
        if (max_line_size > 1) {
            *line = **file_pos;
            line++;
            max_line_size--;
        }
        (*file_pos)++;
    }
    *line = '\0';
    return true;
}
