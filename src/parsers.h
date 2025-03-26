//  ***************************************************************************
/// @file    parsers.h
/// @brief   Parsers collection
//  ***************************************************************************
#ifndef PARSERS_H
#define PARSERS_H

#include <stdint.h>
#include <stdbool.h>


extern bool pars_is_there_template_in_string(uint8_t *str, const uint8_t *temp);
extern void pars_get_tokens_from_string(uint8_t *str, uint8_t *delimiters, const uint8_t **tokens_ptrs, uint32_t max_tokens_qty, uint32_t *actual_tokens_qty);
extern bool pars_get_cmd_answer_from_string(uint8_t *str, const uint8_t *prompt, uint8_t **cmd_answer);
extern bool pars_get_line_from_file(const uint8_t **file_pos, uint8_t *line, uint32_t max_line_size);


#endif   // PARSERS_H
