# 实验二-语义分析报告

## 实现了那些功能？

### 基础数据结构

我引用了一个开源的简单哈希表(`lib/hashtable`)，并自己实现了链表(`list.h`)和栈(`stack.h`)，然后在每种数据结构上实现了对应的枚举器(enumerator)。

### 包含各种重要核心概念的库

针对类型(`corelib.h`)、表达式(`expressions.h`)、语句(`statements.h`)、符号和符号表(`symbols.h`)等核心概念，我完成了一组结构体定义，并为其制定(封装)了一系列密切相关的操作方法，最终整个程序采取面向对象的思路完成。

#### 类型系统

类型(class)的定义如下，其中`basic_type`取`INT`和`FLOAT`，`class_schema`描述类型的种类（函数、基本、数组或结构体）。

```c
typedef struct array_def_t{struct class_t *type;size_t size;} array_def;
typedef struct function_def_t{struct class_t *type;struct list_t* params;} function_def;
typedef union class_def_t{basic_type type;array_def arr_def;function_def func_def;symbol_table stu_def;} class_def;
typedef struct class_t{class_schema schema;class_def def;} * class;
class new_class(class_schema schema, class_def type_def);
class clscpy(class origin);//克隆
int clseql(class t1, class t2);//判断相等，支持函数返回值与一般类型的判断
```

#### 表达式

表达式(expression)的定义如下，其中`operator_type`描述表达式的种类，除了操作符指定的`ADD`,`SUB`等外，还包含数组`ARR`、调用`CALL`、变量`VAR`、常量`CONST`、结构体成员`DOT`等。`control_flags`包含一系列控制位，记录只读、已初始化(不作用于表达式)这种信息。

```c
typedef struct expression_t{operator_type op;class type;control_flags flags;list sub_expressions;} * expression;
expression new_expression(operator_type op,class type,int readonly,list sub_expressions);
```

#### 语句

语句(statement)的定义如下，其中`statement_type`描述语句的种类，包含简单、复合(comp)、返回、if、if-else、while六种。`statement_comp`继承了`statement`类型，其`statement_type`锁定为复合(comp)，并额外包含了一个局部符号表。

```c
typedef struct statement_t{statement_type type;list content;} * statement;
typedef struct statement_comp_t{statement_type type;list content;symbol_table symbols;} * statement_comp;
statement new_statement(statement_type type,list content);
statement_comp new_statement_comp(list content,symbol_table symbols);
```

#### 符号系统

符号(symbol)的相关定义如下，其中`symbol_table_enumerator`是哈希表上的枚举器。`symbol_ref`为函数指定函数体位置，为变量指定初值表达式，为其它定义指定原始节点。

```c
typedef union symbol_ref_t{statement_comp func;expression var;nodeptr origin;} symbol_ref;
typedef struct symbol_t{class type;string name;control_flags flags;symbol_ref reference;} * symbol;
symbol new_symbol(class type, string name, int readonly, int implemented);
symbol_table new_symbol_table();
symbol_table new_symbol_table_from_list(list l);
symbol get_symbol(symbol_table slist, string name);
int has_symbol(symbol_table slist, string name);
void add_symbol(symbol_table slist, symbol sym);
typedef struct symbol_table_enumerator_t{...} * symbol_table_enumerator;
symbol_table_enumerator create_symbol_table_enumerator(symbol_table t);
void destroy_symbol_table_enumerator(symbol_table_enumerator e);
int has_next_symbol_table_enumerator(symbol_table_enumerator e);
symbol get_current_symbol_table_enumerator(symbol_table_enumerator e);
void move_next_symbol_table_enumerator(symbol_table_enumerator e);
```

### 翻译模块

翻译模块(`syntax.translate.h`)为几乎每个语法单元(Program,Specifier,StmtList,CompSt,FunDec,VarList,ExtDefList,ExtDef,ExtDecList,ExtDec,ParamDec,Stmt,Def,DefList,Dec,DecList,VarDec,StructSpecifier,ID,CONST,Exp,Args)分别建立了一个处理函数，调用关系与语法树上节点间的联系方式一致；因此，语义分析起始于$Program()$的调用。

除了这些语法处理函数，本模块还包含以下辅助函数。

```c
symbol_table global_symbols;//全局符号表
symbol_table local_symbols;//结构体成员符号，由结构体分析的调用者维护
stack scoped_symbols;//包含一系列局部符号表的栈，栈底是全局符号表
class return_type;//函数返回值类型，由函数体分析的调用者维护
void translate_system_init();//初始化翻译系统
typedef symbol (*symbol_table_accessor)(string);//访问符号表的函数指针类型
expression parse_exp_single(nodeptr node);//解析单语法单元表达式(CONST,ID)
expression parse_exp_tuple(nodeptr node);//解析双语法单元表达式(单目运算符)
expression parse_exp_triple(nodeptr node);//解析三语法单元表达式(双目运算符、无参数调用、结构体访问、括号)
expression parse_exp_quadruple(nodeptr node);//解析四语法单元表达式(数组和有参数调用)
class merge_type(operator_type op, class t1, class t2);//判断用op连接，左右式类型分别为t1,t2的双目运算符表达式的类型
operator_type parse_operator(string name);//从字符串解析操作符类型
int func_param_eql(list parameters, list arguments);//比对形参和实参列表
void global_symbol_check();//全局符号表检测(查看是否有声明了但未实现的函数)
```

## 程序应该如何被编译？

程序使用makefile进行编译。makefile主要提供以下目标：

- all(默认): clean，然后编译生成可执行文件parser
- parser: 编译生成可执行文件parser
- test2：编译并运行所有位于`examples/lab2`中的测试案例

## 特性

### 预定义的错误类型

独立的主文件`main.c`中提供一系列预定义的错误类型(常量字符串数组)，因此语义分析报错时只需要传递错误号，无需传递描述。

### 局部符号(Scoped Symbol)支持

专门设置了存放局部符号表的栈，在进入一个块(CompStatement)时，它的局部符号表入栈，离开时会被弹出，因此我的语义分析器允许在不同的域中定义相同名称的符号。

### 函数实现和声明分开放置的支持

`control_flags`中设置了一个标记位，用于描述函数是否被实现。如果函数被实现，则对应符号的`reference`指向它的函数体，否则指向定义位置(方便全局符号表检测阶段报错)。

### 枚举器封装

对栈、线性表和符号表的遍历采用枚举器(enumerator)进行封装，可以方便在未来替换其底层实现来进行优化。枚举器(enumerator)的遍历方法如下。

```c
enumerator e;
for (e = create_enumerator(<list/stack/...>); 
    has_next_enumerator(e);
     move_next_enumerator(e))
{
    <type-name> cur = (<type-name>)get_current_enumerator(e);
    // Do Something...
}
destroy_enumerator(e);
```