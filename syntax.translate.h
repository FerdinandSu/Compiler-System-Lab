#pragma once
#include "utils/mylib.h"

symbol_table global_symbols;
/**
 * @brief 默认为null，由调用者为其初始化和回收
 * 
 */
symbol_table local_symbols;
stack scoped_symbols;
void translate_system_init();