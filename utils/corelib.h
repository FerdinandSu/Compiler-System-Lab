#include<memory.h>

#define new(T) calloc(sizeof(T),1);
#define ASSERT_NOT_NULL(o) if (o==NULL) return NULL
typedef struct control_flags_t
{
	int implemented : 1;
	int readonly : 1;
	int padding : 30;
} control_flags;
typedef void *object;
typedef hash_table *symbol_table;

typedef string string;

typedef enum basic_type_t
{
	TYPE_INT,
	TYPE_FLOAT
} basic_type;


typedef enum class_schema_t
{
	CLASS_STANDARD,
	CLASS_FUNCTION,
	CLASS_ARRAY,
	CLASS_STRUCT
} class_schema;
typedef struct array_def_t
{
	struct class_t *type;
	size_t size;
} array_def;
extern struct list_t;
typedef struct function_def_t
{
	struct class_t *type;
	/**
	 * @brief list<symbol>
	 * 
	 */
	struct list_t* params
} function_def;
typedef union class_def_t
{
	basic_type type;
	array_def arr_def;
	function_def func_def;
	symbol_table stu_def;
} class_def;
/**
 * @brief 符号的类型定义
 *
 */
typedef struct class_t
{
	class_schema schema;
	class_def def;
} * class;
class new_class();

class class_int();

class clscpy(class origin);
int clseql(class t1, class t2);

class class_float();
control_flags readonly_flag();