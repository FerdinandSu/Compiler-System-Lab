# 实验三-中间代码生成报告

## 实现了那些功能？

我的中间代码生成模块是在实验二分析生成的全局符号表基础上完成的，无需再次遍历语法树。

### 基础数据结构

在实验二相关数据结构(哈希表(`lib/hashtable`)、链表(`list.h`)和栈(`stack.h`))的基础上，我为链表额外编写了一系列工具函数，并实现了反向枚举器(reverse enumerator)。

```c
list new_list();
list new_list_singleton(object e);
list new_list_of(int count, ...);
list concat_list(list l1,list l2);
list append_list(list l,object e);
list prepend_list(object e,list l);
#define list_first(l) ((l)->first->element)
#define list_second(l) ((l)->first->next->element)
#define list_last(l) ((l)->last->element)
```

### 包含各种重要核心概念的库

在实验二核心库(类型(`corelib.h`)、表达式(`expressions.h`)、语句(`statements.h`)、符号和符号表(`symbols.h`))基础上，我又扩展了中间代码运行时(`irruntime.h`)、中间代码(`intercode.h`)等核心概念，我完成了一组结构体定义，并为其制定(封装)了一系列密切相关的操作方法，最终整个程序采取面向对象的思路完成。

其中，实验二的表达式模块有一些变动。

#### 中间代码运行时

中间代码运行时提供了将内置函数read和write的符号在加入全局符号表的方法，会在语义分析前被调用。

```c
void builtin_symbol_init();
```

核心库中的控制位也加入了相关控制信息(`builtin`)；另一个新加入的`ir_inline`则用于修饰表达式内联信息，这会在后文中提到。

```c
typedef struct control_flags_t{...,int builtin : 2;int ir_inline : 1;int padding : 26;} control_flags;
```

#### 表达式的改动

表达式(expression)的改动在于新增加的`ref`字段，它将会引用本表达式的值对应的操作数。此外，`control_flags`中的`ir_inline`也主要作用于表达式。

```c
typedef struct expression_t{...,code_oprand ref;} * expression;
```

#### 中间代码

中间代码(intercode)的定义如下，其中`code_oprand_type`描述操作数的种类，包含标签、函数、常量、内置常量、地址、关系运算符几种；`code_type`表示一条中间代码的类型，除去书上给定的类型还包含空代码(`IR_EMPTY`)。

```c
typedef union code_oprand_reference_t{int int_const;char* name;int op;float flt_const;} code_oprand_reference;
typedef struct code_oprand_t{code_oprand_type type;code_oprand_reference ref;} code_oprand_obj;
typedef code_oprand_obj* code_oprand;
typedef struct code_t{code_type type;code_oprand oprands[4];} *code;
```

### 中间代码生成模块

中间代码生成模块(`irgen.h`)为主要的符号、语句和表达式分别建立了若干处理函数，调用关系与符号表中它们的引用关系一致；因此，中间代码生成起始于$ir\_from\_symbol\_table(global\_symbols)$的调用。

```c
list ir_from_var_declaration(symbol var);
list ir_from_function(symbol def);
list ir_from_symbol_list(list symbols);
list ir_from_symbol_table(symbol_table st);
list ir_from_expression_single(expression exp);//常变量表达式
list ir_from_expression_tuple(expression exp);//单目运算
list ir_from_expression_triple_lassoc(expression exp);
list ir_from_expression_call_builtin(expression exp, builtin_symbol_type type);
list ir_from_expression_call(expression exp);
list ir_from_expression_array(expression exp);
list ir_from_expression_struct(expression exp);
list ir_from_expression_assign(expression exp);
list ir_from_expression(expression exp);//从右值表达式生成代码
list ir_from_expression_lv(expression exp);//从左值表达式生成代码
list ir_expression_lv_to_rv(list prepend, expression exp);//左值改为右值
list ir_from_statement_comp(statement_comp block);
list ir_from_statement(statement s);
```

表达式部分的翻译有以下需要特别注意的关键点：操作数传递，左右值区分，代码优化。

#### 操作数传递

表达式的值对应的操作数会被存放至表达式的`ref`字段。表达式的`control_flags`中的`ir_inline`字段也会按照该操作数参与的上一条指令而被设置

#### 左右值区分

数组和结构体成员表达式在作为左右值时的处理是不同的。作为左值时，本模块只会计算它的地址；否则，还会从地址读取值。

#### 代码优化

主要对赋值语句进行了优化，我称之为“消去临时变量的赋值内联”。分为以下两种情况：

1. 消去左值的赋值内联：如果一个常量或用户变量参与了赋值等语句，它生成的语句将不会引入新的临时变量来表示该常量或用户变量；
2. 消去右值的赋值内联：如果一个临时变量作为右值出现，且它的上一句被视为可内联的(`ir_inline==-1`，上一句可以被接到赋值语句上)，那么可以通过将该临时变量替换为左值操作数引用的方式来完成消去。

## 程序应该如何被编译？

程序使用makefile进行编译。makefile主要提供以下目标：

- all(默认): clean，然后编译生成可执行文件parser
- parser: 编译生成可执行文件parser
- test3：编译并运行所有位于`examples/lab2`中的测试案例，生成的代码位于`out`文件夹
- irsim：编译并运行所有位于`examples/lab2`中的测试案例，生成的代码位于`out`文件夹；然后启动虚拟机小程序(qt5版)

## 特性

### 基于全局符号表完成

这意味着未来对多文件翻译支持的可能性。

### 内置指令(read,write)支持

见[中间代码运行时](#中间代码运行时)部分。未来可以通过此种方法，扩展更多的内置指令。

### 赋值语句的优化

见[代码优化](#代码优化)部分。

### 反向枚举器封装

对线性表的反向遍历采用反向枚举器(reverse enumerator)进行封装，可以方便在未来替换其底层实现来进行优化。枚举器(reverse_enumerator)的遍历方法如下。

```c
enumerator e;
for (e = create_reverse_enumerator(<list>); 
    has_next_enumerator(e);
     move_next_reverse_enumerator(e))
{
    <type-name> cur = (<type-name>)get_current_enumerator(e);
    // Do Something...
}
destroy_enumerator(e);
```