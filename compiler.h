#ifndef PEACHCOMPLIER_H
#define PEACHCOMPLIER_H
/*it is very easily to undertand.#ifndef 
means if has call the define, then this 
define dont call two times*/

/*the .h file usually call the function 
array and so on*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define S_EQ(str,str2) \
    (str&&str2&&(strcmp(str,str2)==0))

struct pos //this struct is the information of token position include the line and conlome and the filename which is the head file lib function,lib function is used to parse analysis
{
    int line;
    int col;
    const char* filename;
};
struct lex_process;
typedef char (*LEX_PROCESS_NEXT_CHAR)(struct lex_process* process);
typedef char (*LEX_PROCESS_PEEK_CHAR)(struct lex_process* process);
typedef void (*LEX_PROCESS_PUSH_CHAR)(struct lex_process* process, char c);
// lex fuction
struct lex_process_functions
{
    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;



};
// the lex analysis process
struct lex_process
{
    struct pos pos;
    struct vector* token_vec;
    struct compile_process* compiler;
// store the tampere data

    /*
    the record how many braket such as:((50)),this lex has double braket,so we need record the braket the number,becuse double brakets is stand one token
    */
    int current_expression_count;
    // tampere data buffer
    struct buffer* parentheses_buffer;
    // to record this lex where is lex location function
    struct lex_process_functions* function;
    // this private data, which is used to some low requestion,so it is no matter
    void* private; 
    
    
};
struct parsed_switch_case
{
    // Index of the parsed case
    int index;
};
#define SYMBOL_CASE \
    case '{':       \
    case '}':       \
    case ':':       \
    case ';':       \
    case '#':       \
    case '\\':      \
    case ')':       \
    case ']'
    
#define NUMERIC_CASE \
    case '0':       \
    case '1':       \
    case '2':       \
    case '3':       \
    case '4':       \
    case '5':       \
    case '6':       \
    case '7':       \
    case '8':       \
    case '9'       

#define OPERATOR_CASE_EXCLUDING_DIVISON \
    case '+':                           \
    case '-':                           \
    case '*':                           \
    case '>':                           \
    case '<':                           \
    case '^':                           \
    case '%':                           \
    case '!':                           \
    case '=':                           \
    case '~':                           \
    case '|':                           \
    case '&':                           \
    case '(':                           \
    case '[':                           \
    case ',':                           \
    case '.':                           \
    case '?'

#define WHITESPACE  \
    case ' ':       \
    case '\t'
enum{
    LEXICAL_ANALYSIS_ALL_OK,
    LEXICAL_ANALYSIS_INPUT_ERROR
};
enum{
    /*the type of token*/

    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_KEYWORD,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_SYMBOL,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_NEWLINE
};
enum
{
    NUMBER_TYPE_NORMAL,
    NUMBER_TYPE_LONG,
    NUMBER_TYPE_FLOAT,
    NUMBER_TYPE_DOUBLE
};

struct token
{
    int type;
    int flags;
    struct pos pos;
    union{
        char cval;
        const char* sval;//string
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;//pionter
    };
    //this parameter meet one situation, like int i = 123L ,which means 123 is long data type
    struct token_number
    {
       int type;
    }num;
    
    // true if the * a must store the space in token
    bool whitespace;
    //if (holle world) this data point to the "holle world",which use to dbug to identify the bracket inside value 
    const char* between_brackets;
};


enum
{
    DATA_SIZE_BYTE = 1,
    DATA_SIZE_WORD = 2,
    DATA_SIZE_DWORD = 4,
    DATA_SIZE_DDWORD = 8
};


enum
{
    PARSE_ALL_OK,
    PARSE_GENERAL_ERROR
};
// AST the type of node, someone maybe never use, it is not matter
enum
{
    NODE_TYPE_EXPRESSION,
    NODE_TYPE_EXPRESSION_PARENTHESIS,
    NODE_TYPE_NUMBER,
    NODE_TYPE_IDENTIFIER,
    NODE_TYPE_STRING,
    NODE_TYPE_VARIABLE,
    NODE_TYPE_VARIABLE_LIST,
    NODE_TYPE_FUNCTION,
    NODE_TYPE_BODY,
    NODE_TYPE_STATEMENT_RETURN,
    NODE_TYPE_STATEMENT_IF,    // Used for both IF and ELSE IF statements
    NODE_TYPE_STATEMENT_ELSE,  // This is an ELSE statement of an IF
    NODE_TYPE_STATEMENT_WHILE, // While statements i.e while(1) { }
    NODE_TYPE_STATEMENT_DO_WHILE,
    NODE_TYPE_STATEMENT_FOR,
    NODE_TYPE_STATEMENT_BREAK,
    NODE_TYPE_STATEMENT_CONTINUE,
    NODE_TYPE_STATEMENT_SWITCH,
    NODE_TYPE_STATEMENT_CASE,
    NODE_TYPE_STATEMENT_DEFAULT,
    NODE_TYPE_STATEMENT_GOTO,
    NODE_TYPE_TENARY,
    NODE_TYPE_LABEL,
    NODE_TYPE_UNARY,
    NODE_TYPE_STRUCT,
    NODE_TYPE_UNION,
    NODE_TYPE_BRACKET, 
    NODE_TYPE_CAST,
    NODE_TYPE_BLANK,
};
enum
{   
    //which means this node is part of expression
    NODE_FLAG_INSIDE_EXPRESSION = 0b00000001,
    NODE_FLAG_IS_FORWARD_DECLARATION = 0b00000010,
    NODE_FLAG_HAS_VARIABLE_COMBINED = 0b00000100

};
//which struct store the number of dimensions array like: a[10][2]
struct array_brackets
{      
    //vector of struct node*
    struct vector* n_brackets;
};
struct node;
struct datatype
{
    int flags;
    // type this is i.e float, double, long
    int type;

    // like datatype long long double datatype
    struct datatype *secondary;
    // the string of datatype
    const char *type_str;
    // The size in bytes of this datatype.
    size_t size;
    // The pointer depth of this datatype,such as ***a the deepth is 1
    int pointer_depth;

    // If this is a data type of structure or union then you can access one of the modifiers here
    union
    {
        struct node *struct_node;
        struct node *union_node;
    };

    struct array
    {
        struct array_brackets *brackets;
        // This datatype size for the full array its self.
        // Calculation is datatype.size * EACH_INDEX
        // Note the array size of a datatype can be changed depending on the expression it was involved in.
        // for example int abc[50]; and then you go int x = abc[4]; the size will become 4 bytes because
        // we have now resolved an integer. The original "abc" variable will maintain a datatype with a size
        // of 50 * 4 
        size_t size;
    } array;
   
};

struct node 
{
    int type;
    int flags;

    struct pos pos;

    struct node_binded
    {
        // Pointer to our body node
        struct node* owner; 

        // Pointer to the function this node is in.
        struct node* function;
    } binded;
    
    union
    {
        struct exp
        {
            struct node* left;
            struct node* right;
            const char* op;
        } exp;

        struct parenthesis
        {
            // The expression inside the parenthesis node.
            struct node* exp;
        } parenthesis;

        struct var
        {
            struct datatype type;
            int padding;
            // Aligned offset
            int aoffset;
            const char* name;
            struct node* val;   
        } var;

        struct node_tenary
        {
            struct node* true_node;
            struct node* false_node;
        } tenary;
        
        struct varlist
        {
            // A list of struct node* variables.
            struct vector* list;
        } var_list;

        struct bracket
        {
            // int x[50]; [50] would be our bracket node. The inner would NODE_TYPE_NUMBER value of 50
            struct node* inner;
        } bracket;

        struct _struct
        {
            const char* name;
            struct node* body_n;

            /**
             * struct abc
             * {
             * 
             * } var_name;
             * 
             * NULL if no variable attached to structure.
             * 
             */
            struct node* var;
        } _struct;

        struct body
        {
            /**
             * struct node* vector of statements
             */
            struct vector* statements;

            // The size of combined variables inside this body.
            size_t size;

            // True if the variable size had to be increased due to padding in the body.
            bool padded;

            // A pointer to the largest variable node in the statements vector.
            struct node* largest_var_node;
        } body;

        struct function
        {
            // Special flags
            int flags;
            // Return type i.e void, int, long ect... 
            struct datatype rtype;

            // I.e function name "main"
            const char* name;

            struct function_arguments
            {
                // Vector of struct node* . Must be type NODE_TYPE_VARIABLE
                struct vector* vector;

                // How much to add to the EBP to find the first argument.
                size_t stack_addition;
            } args;

            // Pointer to the function body node, NULL if this is a function prototype
            struct node* body_n;
            
            // The stack size for all variables inside this function.
            size_t stack_size;
        } func;


        struct statement
        {

            struct return_stmt
            {
                // The expression of the return 
                struct node* exp;
            } return_stmt;

            struct if_stmt
            {
                // if(COND) {// body }
                struct node* cond_node;
                struct node* body_node;

                // if(COND) {} else {}
                struct node* next;
            } if_stmt;

            struct else_stmt
            {
                struct node* body_node;
            } else_stmt;

            struct for_stmt
            {
                struct node* init_node;
                struct node* cond_node;
                struct node* loop_node;
                struct node* body_node;
            } for_stmt;

            struct while_stmt
            {
                struct node* exp_node;
                struct node* body_node;
            } while_stmt;

            struct do_while_stmt
            {   
                struct node* exp_node;
                struct node* body_node;
            } do_while_stmt;

            struct switch_stmt
            {
                struct node* exp;
                struct node* body;
                struct vector* cases;
                bool has_default_case;
            } switch_stmt;

            struct _case_stmt
            {
                struct node* exp;
            } _case;

            struct _goto_stmt
            {
                struct node* label;
            } _goto;
        } stmt;


        struct node_label
        {
            struct node* name;
        } label;

        struct cast
        {
            struct datatype dtype;
            struct node* operand;
        } cast;

        
    }; 
    //some valueable node have value
    union{
        char cval;
        const char* sval;//string
        unsigned int inum;
        unsigned long lnum;
        unsigned long long llnum;
        void* any;//pionter
    };
   
};

enum
{
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAIL_WITH_ERRORS

};
//scope of variable


struct compile_process
{
    // how to complier 
    int flags;

    struct pos pos;
    struct compile_process_input_file{

        FILE* fp;
        const char* abs_path;

    }cfile;
    //this vectors is from lexer output vector which store the token
    struct vector* token_vec;
    //this vector store the node vec,which vector be used to some operation such as push and pop node vector
    struct vector* node_vec;
    //this vector in the right order of AST
    struct vector* node_tree_vec;
    FILE* ofile;


    struct{
        // like outside of main() val
        struct scope* root;
        //the main() inside val
        struct scope* current;
    }scope;


    struct{
        //current symbol table
        struct vector* table;
        // store the symblol table in the table 
        struct vector* tables;
    }symbols;
};
enum
{
    // The flag is set for native functions.
    FUNCTION_NODE_FLAG_IS_NATIVE = 0b00000001,
};
enum
{
    DATATYPE_FLAG_IS_SIGNED = 0b00000001,
    DATATYPE_FLAG_IS_STATIC = 0b00000010,
    DATATYPE_FLAG_IS_CONST = 0b00000100,
    DATATYPE_FLAG_IS_POINTER = 0b00001000,
    DATATYPE_FLAG_IS_ARRAY = 0b00010000,
    DATATYPE_FLAG_IS_EXTERN = 0b00100000,
    DATATYPE_FLAG_IS_RESTRICT = 0b01000000,
    DATATYPE_FLAG_IGNORE_TYPE_CHECKING = 0b10000000,
    DATATYPE_FLAG_SECONDARY = 0b100000000,
    DATATYPE_FLAG_STRUCT_UNION_NO_NAME = 0b1000000000,
    DATATYPE_FLAG_IS_LITERAL = 0b10000000000
};
enum
{
    DATA_TYPE_VOID,
    DATA_SIZE_ZERO,
    DATA_TYPE_CHAR,
    DATA_TYPE_SHORT,
    DATA_TYPE_INTEGER,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_LONG,
    DATA_TYPE_STRUCT,
    DATA_TYPE_UNION,
    DATA_TYPE_UNKNOWN,
};
enum
{
    DATA_TYPE_EXPECT_PRIMITIVE,
    DATA_TYPE_EXPECT_UNION,
    DATA_TYPE_EXPECT_STRUCT,
};

struct scope
{
    int flags;
    //we store the scope value by vector 
    struct vector* entities;
    //null if non parents
    struct scope* parent;
    size_t size;
};
enum
{
    SYMBOL_TYPE_NODE,
    SYMBOL_TYPE_NATIVE_FUNCTION,
    SYMBOL_TYPE_UNKNOWN
};
//struct function val all be as the symbol
struct symbol{
    const char* name;
    int type;
    void* data;
};
int compile_file(const char* filename, const char* out_filename, int flags);
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags);
char compile_process_next_char(struct lex_process* lex_process);
//this function is get the peek char,why need the ungtec,becuse call the getc this char will output from stream
char compile_process_peek_char(struct lex_process* lex_process);
void compile_process_push_char(struct lex_process* lex_process,char c);
struct lex_process* lex_process_create(struct compile_process* compiler,struct lex_process_functions* functions, void* private);
void lex_process_free(struct lex_process* process);
void* lex_process_private(struct lex_process* process);
struct vector* lex_process_tokens(struct lex_process* process);
int lex(struct lex_process* process);
int parse(struct compile_process* process);
//create a token from a string, this function is out of lexer
struct lex_process *tokens_build_for_string(struct compile_process *compiler, const char *str);
void compiler_error(struct compile_process* compiler, const char* msg,...);
void compiler_warning(struct compile_process* compiler, const char* msg,...);
bool token_is_keyword(struct token* token, const char* value);
bool token_is_symbol(struct token* token, char c);
bool token_is_nl_or_comment_or_newline_seperator(struct token* token);
bool keyword_is_datatype(const char *str);
bool datatype_is_struct_or_union_for_name(const char* name);
size_t datatype_size_for_array_access(struct datatype* dtype);
size_t datatype_element_size(struct datatype* dtype);
size_t datatype_size_no_ptr(struct datatype* dtype);
size_t datatype_size(struct datatype* dtype);
bool token_is_primitive_keyword(struct token* token);
bool token_is_operator(struct token *token, const char *op);
struct node *node_create(struct node *_node);
void make_exp_node(struct node *node_left, struct node *node_right, const char *op);
struct node *node_peek_or_null();
struct node *node_peek();
struct node *node_pop();
void node_push(struct node *node);
void node_set_vector(struct vector *vec, struct vector *root_vec);
bool node_is_expressionable(struct node *node);
struct node* node_peek_expressionable_or_null();
struct array_brackets* array_brackets_new();
void array_brackets_free(struct array_brackets* brackets);
void array_brackets_add(struct array_brackets* brackets, struct node* bracket_node);
struct vector* array_brackets_node_vector(struct array_brackets* brackets);
size_t array_brackets_calculate_size_from_index(struct datatype* type, struct array_brackets* brackets, int index);
size_t array_brackets_calculate_size(struct datatype* type, struct array_brackets* brackets);
int array_total_indexes(struct datatype* dtype);
void make_bracket_node(struct node* node);
bool datatype_is_struct_or_union(struct datatype* dtype);
struct scope *scope_alloc();
struct scope *scope_create_root(struct compile_process *process);
struct scope *scope_new(struct compile_process *process, int flags);
void scope_finish(struct compile_process *process);
struct scope *scope_create_root(struct compile_process *process);
void scope_free_root(struct compile_process *process);
void *scope_last_entity_at_scope(struct scope *scope);
void scope_push(struct compile_process *process, void *ptr, size_t elem_size);
void *scope_last_entity(struct compile_process *process);
void *scope_iterate_back(struct scope *scope);
void scope_iteration_start(struct scope *scope);
void scope_iteration_end(struct scope *scope);
struct scope *scope_current(struct compile_process *process);
void* scope_last_entity_stop_at(struct compile_process* process, struct scope* stop_scope);
void symresolver_initialize(struct compile_process* process);
void make_body_node(struct vector *body_vec, size_t size, bool padded, struct node *largest_var_node);
size_t variable_size(struct node* var_node);
size_t variable_size_for_list(struct node* var_list_node);
int padding(int val, int to);
int align_value(int val, int to);
int align_value_treat_positive(int val, int to);
int compute_sum_padding(struct vector* vec);
struct node* variable_struct_or_union_body_node(struct node* node);
bool node_is_struct_or_union_variable(struct node* node);
struct node* variable_node(struct node* node);
bool variable_node_is_primitive(struct node* node);
bool datatype_is_primitive(struct datatype* dtype);
struct node* variable_node_or_list(struct node* node);
void make_struct_node(const char* name, struct node* body_node);
void make_exp_parentheses_node(struct node* exp_node);
struct node* node_from_sym(struct symbol* sym);
struct node* node_from_symbol(struct compile_process* current_process, const char* name);
struct node* struct_node_for_name(struct compile_process* current_process, const char* name);
struct symbol* symresolver_get_symbol(struct compile_process* process, const char* name);
bool node_is_expression_or_parentheses(struct node* node);
bool node_is_value_type(struct node* node);
size_t function_node_argument_stack_addition(struct node* node);
void symresolver_new_table(struct compile_process* process);
void symresolver_end_table(struct compile_process* process);
bool token_is_identifier(struct token* token);
void make_function_node(struct datatype* ret_type, const char* name, struct vector* arguments, struct node* body_node);
struct symbol* symresolver_get_symbol_for_native_function(struct compile_process* process, const char* name);
void make_if_node(struct node* cond_node, struct node* body_node, struct node* next_node);
void make_else_node(struct node* body_node);
void make_return_node(struct node* exp_node);
void make_continue_node();
void make_break_node();
void make_goto_node(struct node* label_node);
void make_while_node(struct node* exp_node, struct node* body_node);
void make_do_while_node(struct node* body_node, struct node* exp_node);
void make_label_node(struct node* name_node);
void make_cast_node(struct datatype* dtype, struct node* operand_node);
void make_tenary_node(struct node* true_node, struct node* false_node);
void make_switch_node(struct node* exp_node, struct node* body_node, struct vector* cases, bool has_default_case);
void make_for_node(struct node* init_node, struct node* cond_node, struct node* loop_node, struct node* body_node);
#define TOTAL_OPERATOR_GROUPS 14
#define MAX_OPERATORS_IN_GROUP 12
//according to the link:https://en.cppreference.com/w/cpp/language/operator_precedence
//this is two different the operator associativity
enum
{
    ASSOCIATIVITY_LEFT_TO_RIGHT,
    ASSOCIATIVITY_RIGHT_TO_LEFT
};
struct expressionable_op_precedence_group
{
    char *operators[MAX_OPERATORS_IN_GROUP];
    int associativity;
};

struct fixup;

/**
 * Fixes the fixup.
 * Return true if the fixup was successful.
 * 
 */
typedef bool(*FIXUP_FIX)(struct fixup* fixup);
/**
 * @brief Signifies the fixup has been removed from memory. 
 * The implementor of this function pointer should free any memory related
 * to the fixup.
 */
typedef void(*FIXUP_END)(struct fixup* fixup);

struct fixup_config
{
    FIXUP_FIX fix;
    FIXUP_END end;
    void* private;
};

struct fixup_system
{
    // A vector of fixups.
    struct vector* fixups;
};

enum
{
    FIXUP_FLAG_RESOLVED = 0b00000001
};

struct fixup
{
    int flags;
    struct fixup_system* system;
    struct fixup_config config;
};

struct fixup_system* fixup_sys_new();
struct fixup_config* fixup_config(struct fixup* fixup);

void fixup_free(struct fixup* fixup);
void fixup_start_iteration(struct fixup_system* system);
struct fixup* fixup_next(struct fixup_system* system);
void fixup_sys_fixups_free(struct fixup_system* system);

void fixup_sys_free(struct fixup_system* system);
int fixup_sys_unresolved_fixups_count(struct fixup_system* system);
struct fixup* fixup_register(struct fixup_system* system, struct fixup_config* config);
bool fixup_resolve(struct fixup* fixup);
void* fixup_private(struct fixup* fixup);
bool fixups_resolve(struct fixup_system* system);

#endif