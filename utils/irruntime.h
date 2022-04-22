#pragma once

void builtin_symbol_init();
typedef enum builtin_symbol_type_t
{
    BUILT_IN_NONE = 0b00,
    BUILT_IN_READ = 0b01,
    BUILT_IN_WRITE = 0b10
} builtin_symbol_type;