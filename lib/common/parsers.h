//  ***************************************************************************
/// @file    parsers.h
/// @brief   Parsers collection
//  ***************************************************************************
#ifndef PARSERS_H
#define PARSERS_H

#include <stdint.h>
#include <stdbool.h>


extern bool pars_is_there_template_in_string(const uint8_t *str, const uint8_t *temp);
extern void pars_get_tokens_from_string(uint8_t *str, const uint8_t *delimiters, const uint8_t **tokens_ptrs, uint32_t max_tokens_qty, uint32_t *actual_tokens_qty);
extern bool pars_string_to_s32(const uint8_t *str, int32_t *digit);
extern bool pars_string_to_u32(const uint8_t *str, uint32_t *digit);

#endif   // PARSERS_H
