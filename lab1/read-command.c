// UCLA CS 111 Lab 1 command reading

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "command.h"
#include "command-internals.h"

#include <stdio.h>
#include <stdlib.h>  // for malloc/free
#include <error.h>
#include <ctype.h>   // for isalnum
#include <string.h>  // for strcpy

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */
   
/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

#define EXECUTION_STATUS -1
#define WORD_BUF_SIZE 100

enum Elements
{
    COMMAND,
    PIPE,   // pipe or semicolon
    SEMICOLON,
    NEWLINE,
    SPECIAL,    // close parenthesis and special words like "if"
    REDIRECT
};

typedef struct command_node   *command_node_t;

struct command_node
{
    command_t      command; //lots of stuff is named command
    command_node_t next;
};

void init_node(struct command_node * node) {
    node->command = (command_t) malloc(sizeof(struct command));
    node->next = NULL;
    //node->next = (command_node_t) malloc(sizeof(struct command_node));
}

struct command_stream
{
    command_node_t head;
};

// push node to top and increment size
// returns void
void command_stream_push(struct command_stream *stream, struct command_node node) {
    if(stream->head == NULL) {
        //stack->top = (struct command_node*)malloc(sizeof(struct command_node));
        stream->head = (struct command_node*)malloc(sizeof(node));
        stream->head->next = NULL;
        stream->head->command = node.command;
    } else {
        //struct command_node* temp = (struct command_node*)malloc(sizeof(struct command_node));
        struct command_node* temp = (struct command_node*)malloc(sizeof(struct command_node));
        temp->command = node.command;
        temp->next = stream->head;
        stream->head = temp;
    }
}

struct command_stack {
    struct command_node *top;
    int size;
};

void init_command_stack(struct command_stack *stack) {
    stack->top = NULL;
    stack->size = 0;
}

// push node to top and increment size
// returns void
void command_stack_push(struct command_stack *stack, struct command_node node) {
    if(stack->size == 0) {
        //stack->top = (struct command_node*)malloc(sizeof(struct command_node));
        stack->top = (struct command_node*)malloc(sizeof(node));
        stack->top->next = NULL;
        stack->top->command = node.command;
        stack->size++;
    } else {
        //struct command_node* temp = (struct command_node*)malloc(sizeof(struct command_node));
        struct command_node* temp = (struct command_node*)malloc(sizeof(struct command_node));
        temp->command = node.command;
        temp->next = stack->top;
        stack->top = temp;
        stack->size++;
    }
}

// pop the top and decrement size
// returns the popped top
struct command_node* command_stack_pop(struct command_stack *stack) {
    struct command_node* retval; 

    if (!stack->size)
        return NULL;
    
    retval = stack->top; 
    stack->top = stack->top->next;
    stack->size--;
    return retval;
}

// peeks the top
// returns a pointer to the top node
struct command_node *command_stack_top(struct command_stack *stack) {
    return stack->top;
}

// checks to see if stack is empty or not
// if empty, returns true else false
int command_stack_empty(struct command_stack *stack) {
    return stack->size == 0;
}

// returns size of command stack
int command_stack_size(struct command_stack *stack) {
    return stack->size;
}

struct operator_node {
    int value;
    struct operator_node* next;
};

struct operator_stack {
    struct operator_node* top;
    int size;
};

void init_operator_node(struct operator_node* node) {
    node->next = NULL;
}

void init_operator_stack (struct operator_stack *stack) {
    stack->top = NULL;
    stack->size = 0;
}

void operator_stack_push( struct operator_stack *stack, struct operator_node node) {
    if(stack->size == 0) {
        //stack->top = (struct operator_node*)malloc(sizeof(struct operator_node));
        stack->top = (struct operator_node*)malloc(sizeof(node));
        stack->top->value = node.value;
        stack->top->next = NULL;
        stack->size++;
    } else {
        //struct operator_node* temp = (struct operator_node*)malloc(sizeof(struct operator_node));
        struct operator_node* temp = (struct operator_node*)malloc(sizeof(node));
        temp->value = node.value;
        temp->next = stack->top;
        stack->top = temp;
        stack->size++;
    }
}

struct operator_node* operator_stack_pop(struct operator_stack *stack) {
    struct operator_node* retval;
    
    if (!stack->size)
        return NULL;
    
    retval = stack->top;
    stack->top = stack->top->next;
    stack->size--;
    return retval;
}

struct operator_node* operator_stack_top(struct operator_stack *stack) {
    return stack->top;
}

int operator_stack_empty(struct operator_stack *stack) {
    return stack->size == 0;
}

int operator_stack_size(struct operator_stack * stack) {
    return stack->size;
}

// checks if c is an 'other' token as specified in spec
int is_other_character(char c) {
    if(c == '!')
        return 1;
    else if(c == '+')
        return 1;
    else if(c == ',')
        return 1;
    else if(c == '-')
        return 1;
    else if(c == '.')
        return 1;
    else if(c == '/')
        return 1;
    else if(c == ':')
        return 1;
    else if(c == '@')
        return 1;
    else if(c == '^')
        return 1;
    else if(c == '_')
        return 1;
    else 
        return 0;
}

// checks if c is a 'special' token
int is_special_token(char c){
    if(c == ';')
        return 1;
    else if(c == '|')
        return 1;
    else if(c == '(')
        return 1;
    else if(c == ')')
        return 1;
    else if(c == '<')
        return 1;
    else if(c == '>')
        return 1;
    else
        return 0;
}

int is_valid_character(char c) {
    return isalnum(c) || isspace(c) || is_other_character(c) || is_special_token(c);
}

int get_operator_type(char *buf) {
    if(!strcmp(buf, "if")) {
        return IF_OP;
    } else if(!strcmp(buf, "then")) {
        return THEN_OP;
    } else if(!strcmp(buf, "else")) {
        return ELSE_OP;
    } else if(!strcmp(buf, "while")) {
        return WHILE_OP;
    } else if(!strcmp(buf, "until")) {
        return UNTIL_OP;
    } else if(!strcmp(buf, "do")) {
        return DO_OP;
    } else if(!strcmp(buf, "done")) {
        return DONE_OP;
    } else if(!strcmp(buf, "fi")) {
        return FI_OP;
    } else if(!strcmp(buf, ";")) {
        return SEMICOLON_OP;
    } else if(!strcmp(buf, "\n")) {
        return NEWLINE_OP;
    } else if(!strcmp(buf, "|")) {
        return PIPE_OP;
    } else if(!strcmp(buf, "(")) {
        return OPEN_PAREN_OP;
    } else if(!strcmp(buf, ")")) {
        return CLOSE_PAREN_OP;
    } else
        return -1; // error: not an operator
}

// based on operator, return appropriate command type
int get_command_type(int op) {
    switch(op) {
        case IF_OP:
        case THEN_OP:
        case ELSE_OP:
        case FI_OP:
            return IF_COMMAND;
            break;
        case OPEN_PAREN_OP:
        case CLOSE_PAREN_OP:
            return SUBSHELL_COMMAND;
            break;
        case SEMICOLON_OP:
        case NEWLINE_OP:
            return SEQUENCE_COMMAND;
            break;
        case PIPE_OP:
            return PIPE_COMMAND;
            break;
        case WHILE_OP:
            return WHILE_COMMAND;
            break;
        case UNTIL_OP:
            return UNTIL_COMMAND;
            break;
        default:
            //error
            return -1;
    }
}

// used to combine 2 command nodes into a single command node
struct command_node *combine_two_commands(struct command_node *first_command, struct command_node *second_command, int op) {
    // declarations
    struct command_node *new_command_node; 
    int command_type;

    // get command type from the operator type
    command_type = get_command_type(op);
    
    // allocate memory for node and command
    new_command_node = (command_node_t)malloc(sizeof(struct command_node));
    new_command_node->command = (command_t)malloc(sizeof(struct command));
    
    new_command_node->command->type    = command_type;
    new_command_node->command->status  = EXECUTION_STATUS;
    new_command_node->command->input   = NULL;
    new_command_node->command->output  = NULL;
    
    // place objects in u.command
    new_command_node->command->u.command[0] = first_command->command;
    new_command_node->command->u.command[1] = second_command->command;
    
    return new_command_node;
}

// overload of combine_commands that takes 3 command nodes
// for use with if-then-lse commands
struct command_node *combine_three_commands(struct command_node *first_command, struct command_node *second_command, struct command_node *third_command, int op) {
    // declarations
    struct command_node *new_command_node; 
    int command_type;
    
    // get command type from the operator type
    command_type = get_command_type(op); // op == IF_OP
    
    // dynamically allocate node and command
    new_command_node = (command_node_t)malloc(sizeof(struct command_node));
    new_command_node->command = (command_t)malloc(sizeof(struct command));    
    
    // fill in command attributes
    new_command_node->command->type    = command_type;
    new_command_node->command->status  = EXECUTION_STATUS;
    new_command_node->command->input   = NULL;
    new_command_node->command->output  = NULL;
    
    // place objects in u.command
    new_command_node->command->u.command[0] = first_command->command;
    new_command_node->command->u.command[1] = second_command->command;
    new_command_node->command->u.command[2] = third_command->command;
    
    return new_command_node;
}

void
print_error_message(int line_number, char *error_string) {
    fprintf(stderr, "%d: Syntax: %s\n", line_number, error_string);
    free(error_string);
    exit(1);  //magic number
}


command_stream_t
make_command_stream (int (*get_next_byte) (void *),
                 void *get_next_byte_argument)
{
    /* FIXME: Replace this with your implementation.  You may need to
       add auxiliary functions and otherwise modify the source code.
       You can also use external functions defined in the GNU C Library.  */

    // int j;
    size_t i;  // index for appending character to word

    char c = '\0';  // current byte
    char r = '\0';  // redirect type
    int is_operator = 0;  // if set to true, run algorithm from class
    int is_special_word = 0;
    int is_redirect = 0;
    int last_byte = 0;
    int number_of_words = 0;  // index for appending word
    int line_number_ref = 0;  // keep track of line number in case of error. 
    
    //char word[100];
    //char* words[100];
    char * word;
    char ** words;
    char * special_word;
    char * error_description; // to pass description of error to printing function
    
    enum Elements follows = NEWLINE;
    
    command_node_t node;  // pointer
    // command_t my_command;  // pointer
    
    struct operator_stack opstack;  // not pointer
    struct command_stack comstack;  // not pointer
    
    //struct command_stream comstream;  // not pointer
    struct command_stream *comstream;
    
    struct operator_stack spec_op_stack;
    
    // init error_description
    error_description = (char*)malloc(WORD_BUF_SIZE*sizeof(char)+1);
    memset(error_description, 0, WORD_BUF_SIZE*sizeof(char)+1);
    
    // init comstream. Dynamically allocated because needs to be returned
    comstream = (struct command_stream*)malloc(sizeof(struct command_stream));
    comstream->head = NULL;
    
    init_operator_stack(&opstack);
    init_command_stack(&comstack);
    init_operator_stack(&spec_op_stack);
    
    // init word (set all bytes to \0)
    word = (char*)malloc(WORD_BUF_SIZE*sizeof(char)+1);
    memset(word, 0, WORD_BUF_SIZE*sizeof(word));
    
    special_word = (char*)malloc(WORD_BUF_SIZE*sizeof(char)+1);
    memset(special_word, 0, WORD_BUF_SIZE*sizeof(special_word));
    
    // init words (allocate and set items to null)
    words = (char**)malloc(WORD_BUF_SIZE*sizeof(char*)+1);
    memset(words, 0, WORD_BUF_SIZE*sizeof(words)); // init words elements to NULL
    
    // init node (allocate node and command)
    node = (command_node_t)malloc(sizeof(struct command_node));
    //node->command = NULL;
    //node->next = NULL;
    init_node(node);
    
    for (;;)
    {
        // Reset
        is_operator = 0;
        //memset(special_word, 0, sizeof(*special_word));
        memset(special_word, 0, WORD_BUF_SIZE*sizeof(char));
        
        // get next byte
        if (!is_special_word && !is_redirect) {
            c = get_next_byte(get_next_byte_argument);
            if(!is_valid_character(c)){
                strcpy(error_description, "invalid character");
                print_error_message(line_number_ref, error_description);
            }
        }
        // if(c == EOF)
        //     break;
        
        // reset is_special_word
        is_special_word = 0;
        is_redirect = 0;
        
        // if word char
        if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' ||
              c == '.' || c == '/' || c == ':' || c == '@' || c == '^' || c == '_' )
        {
            // if first char of first word and follows command
            //if (strlen(word) == 0 && sizeof(words) == 0 && follows == COMMAND)
            if(strlen(word) == 0 && words[0] == NULL && follows == COMMAND)
            {
                // error
                //exit(12);
                strcpy(error_description, "Word can't follow command if is first word");
                print_error_message(line_number_ref, error_description);
            }
            // append (could have huge problems with segfault)
            i = strlen(word);
            word[i] = c;
            word[++i] = '\0';
        }
        else
        {
            if (strlen(word) > 0)
            {
                // if first word
                //if (sizeof(words) == 0)
                if(words[0] == NULL &&
                   (!strcmp(word, "if") || !strcmp(word, "while") || !strcmp(word, "until") ||
                    !strcmp(word, "then") || !strcmp(word, "else") || !strcmp(word, "fi") ||
                    !strcmp(word, "do") || !strcmp(word, "done")))
                {
                    is_special_word = 1;
                    if (follows == COMMAND && (!strcmp(word, "if") || !strcmp(word, "while") || !strcmp(word, "until"))) {
                        //exit(234);
                        strcpy(error_description, "first compound command keyword can't follow command");
                        print_error_message(line_number_ref, error_description);
                    }
                    if ((!strcmp(word, "then") || !strcmp(word, "else") || !strcmp(word, "fi") || !strcmp(word, "do") || !strcmp(word, "done")) &&
                        follows != COMMAND && follows != SEMICOLON && follows != NEWLINE) {
                            // exit(89);
                            strcpy(error_description, "dependent compound command keyword doesn't follow command, semicolon, or newline");
                            print_error_message(line_number_ref, error_description);
                        }
                    if ((!strcmp(word, "then") || !strcmp(word, "else") || !strcmp(word, "fi") || !strcmp(word, "do") || !strcmp(word, "done")) && follows == SEMICOLON)
                        operator_stack_pop(&opstack);
                    special_word = word;
                    is_operator = 1;
                    if (strcmp(word, "fi") && strcmp(word, "done"))
                        follows = SPECIAL;
                    else
                        follows = COMMAND;
                    
                    
                    // if (strcmp(word, "if") || strcmp(word, "while") || strcmp(word, "until"))
                    // {
                    //     // push onto operator stack
                    // }
                    // else if (strcmp(word, "then") || strcmp(word, "else") || strcmp(word, "if") ||
                    //          strcmp(word, "do") || strcmp(word, "done"))
                    // {
                    //     // validate (scope stack?)
                    //     // process operator stack until reach preceding special word
                    // }
                } else {
                
                // save word in words then reset
                words[number_of_words] = (char*)malloc(sizeof(word)+1);
                memcpy(words[number_of_words], word, sizeof(word)+1);
                number_of_words++;
                //words[number_of_words] = word;
                // word = (char*) malloc(sizeof(word));
                //word[0] = '\0';
                //words[number_of_words] = (char*)malloc(sizeof(word));
                //strcpy(words[number_of_words++], word);  // copy word into words[pos]            
                }
                
                // reset word
                word = (char*)malloc(WORD_BUF_SIZE*sizeof(char)+1);
                memset(word, 0, WORD_BUF_SIZE*sizeof(char));
            }
            if (c == '#')
            {
                // ignore remaining characters until reach newline
                // for (;;) {
                //     c = get_next_byte(get_next_byte_argument);
                //     if (c == '\n' || c == EOF)
                //         break;
                // }
                do {
                    c = get_next_byte(get_next_byte_argument);
                    line_number_ref++;
                } while (c != '\n' && c != EOF);
            }
            if (words[0] != NULL && c != ' ' && c != '\t') // then simple command
            {
                follows = COMMAND;
                // push simple command
                //node->command = (command_t)malloc(sizeof(struct command));
                node->command->type = SIMPLE_COMMAND;
                node->command->status = EXECUTION_STATUS;
                node->command->input = NULL;
                node->command->output = NULL;
                node->command->u.word = words;
                // node->command->u.command[0] = NULL;
                command_stack_push(&comstack, *node);
                
                // reset
                // node = (command_node_t) malloc(sizeof(struct command_node));  // Does this work?
                // my_command = (command_t)malloc(sizeof(struct command));
                // node->command = my_command;
                init_node(node);
                words = (char**)malloc(WORD_BUF_SIZE*sizeof(char*)+1); // No good solution yet. 
                words[0] = NULL;  //used to check if words is empty in future if statements
                number_of_words = 0;
            }
            if (c == '(') {
                if (follows == COMMAND) {
                    // exit(234);
                    strcpy(error_description, "( can't directly follow command");
                    print_error_message(line_number_ref, error_description);
                }
                //special_word = word;
                follows = SPECIAL;
                is_operator = 1;
            }
            else if (c == ')') {
                if (follows != COMMAND && follows != SEMICOLON && follows != NEWLINE) {
                    // exit(78);
                    strcpy(error_description, ") must follow command, semicolon, or newline");
                    print_error_message(line_number_ref, error_description);
                }
                // special_word = word;
                is_operator = 1;
            }
            else if (c == ';' || c == '|')
            {
                if (follows == COMMAND)
                {
                    is_operator = 1;
                    if (c == ';')
                        follows = SEMICOLON;
                    else
                        follows = PIPE;
                }
                else {
                    // error
                    //printf("operator %c does not follow command", c);
                    strcpy(error_description, "operator does not follow command");
                    print_error_message(line_number_ref, error_description);
                    //exit(23);
                }
            }
            else if (c == '\n')
            {
                line_number_ref++;
                if (follows == COMMAND)
                {
                    c = ';';
                    is_operator = 1;
                    follows = SEMICOLON;
                }
                else if (last_byte == ';')
                {
                    operator_stack_pop(&opstack);
                    follows = NEWLINE;
                }
            }
            else if (c == '<' || c == '>') {
                is_redirect = 1;
                r = c;
                if (follows != COMMAND) {
                    //exit(45); // error
                    strcpy(error_description, "redirect doesn't follow command");
                    print_error_message(line_number_ref, error_description);
                }
                for (;;) {
                    c = get_next_byte(get_next_byte_argument);
                    
                    if (isalnum(c) || c == '!' || c == '%' || c == '+' || c == ',' || c == '-' ||
                          c == '.' || c == '/' || c == ':' || c == '@' || c == '^' || c == '_' ) {
                        // append to word
                        i = strlen(word);
                        word[i] = c;
                        word[++i] = '\0';
                    } else {
                        if (strlen(word) > 0) {
                            // end word
                            words[number_of_words++] = word;
                            word = (char*)malloc(WORD_BUF_SIZE*sizeof(char)+1);
                            memset(word, 0, WORD_BUF_SIZE*sizeof(word));    
                        }
                        if (c != ' ' && c != '\t')
                            break;
                    }
                }
                // if more than one word  //or zero words?
                if (number_of_words != 1) {
                    //exit(56);
                    strcpy(error_description, "more than one word follows redirect");
                    print_error_message(line_number_ref, error_description);
                }
                node = command_stack_top(&comstack);
                if (r == '<') {
                    if (node->command->input) {
                        //exit (67);
                        strcpy(error_description, "command already contains an input");
                        print_error_message(line_number_ref, error_description);
                    } else
                        node->command->input = words[0];
                } else {
                    if (node->command->output) {
                        strcpy(error_description, "command already contains an output");
                        print_error_message(line_number_ref, error_description);
                        //exit (78);
                    } else
                        node->command->output = words[0];
                }
                node = (struct command_node *) malloc(sizeof(struct command_node));
                init_node(node);
                // words = (char**)malloc(WORD_BUF_SIZE*sizeof(char*)+1); // No good solution yet. 
                words[0] = NULL;
                number_of_words = 0;
            }
            else if (c == EOF) {
                is_operator = 1;
                if (follows == SEMICOLON)
                    operator_stack_pop(&opstack); //pop the automatically added semicolon
                else if (follows != COMMAND && follows != NEWLINE) {
                    //exit(90);
                    strcpy(error_description, "EOF must follow either semicolon, command, or newline");
                    print_error_message(line_number_ref, error_description);
                } 
                if (operator_stack_size(&spec_op_stack)) {
                    //exit(91);
                    strcpy(error_description, "compound command not properly closed");
                    print_error_message(line_number_ref, error_description);
                }
            }
            if (is_operator)
            {
                // follows = OPERATOR;
                // get operator type from operator string
                // char *operator_string = (char*)malloc(sizeof(c)+1);
                // strcpy(operator_string, &c);
                //int operator_type = get_operator_type(operator_string);  //operator_string is made up
                int operator_type = -1; // default
                struct operator_node op_node;
                if (is_special_word)
                    operator_type = get_operator_type(special_word);
                else if (c == ';')
                    operator_type = get_operator_type(";");
                else if (c == '\n')
                    operator_type = get_operator_type("\n");
                else if (c == '|')
                    operator_type = get_operator_type("|");
                else if(c == '(')
                    operator_type = OPEN_PAREN_OP;
                else if(c == ')')
                    operator_type = CLOSE_PAREN_OP;
                else if (c == EOF)
                    operator_type = EOF_OP;
                
                op_node.value = operator_type;
                
                if(operator_type == IF_OP || operator_type == WHILE_OP
                        || operator_type == UNTIL_OP || operator_type == OPEN_PAREN_OP) {
                    operator_stack_push(&spec_op_stack, op_node);
                } else if(operator_type == CLOSE_PAREN_OP && spec_op_stack.top != NULL && spec_op_stack.top->value == OPEN_PAREN_OP) {
                    operator_stack_push(&spec_op_stack, op_node);
                } else if (is_special_word) {
                    if (spec_op_stack.top != NULL) {
                        int top_operator_value = spec_op_stack.top->value;
                        if((operator_type == THEN_OP && top_operator_value == IF_OP)  ||
                          (operator_type == ELSE_OP && top_operator_value == THEN_OP)  ||
                          (operator_type == FI_OP && top_operator_value == ELSE_OP)  ||
                          (operator_type == FI_OP && top_operator_value == THEN_OP)  ||
                          (operator_type == DO_OP && top_operator_value == WHILE_OP) ||
                          (operator_type == DO_OP && top_operator_value == UNTIL_OP) ||
                          (operator_type == DONE_OP && top_operator_value == DO_OP)  ||
                          (operator_type == CLOSE_PAREN_OP && top_operator_value == OPEN_PAREN_OP)) {
                                operator_stack_push(&spec_op_stack, op_node);
                        } else {
                                //exit(312);
                            strcpy(error_description, "compound command key word order is incorrect");
                            print_error_message(line_number_ref, error_description);
                        }
                    } else {
                       // exit(313);
                       strcpy(error_description, "compound command key word doesn't follow compound starter word");
                       print_error_message(line_number_ref, error_description);
                    }
                }
                
                if(operator_stack_empty(&opstack)) {
                    operator_stack_push(&opstack, op_node);
                } else if(operator_type > operator_stack_top(&opstack)->value 
                            || operator_type == IF_OP || operator_type == WHILE_OP || operator_type == UNTIL_OP || operator_type == OPEN_PAREN_OP) {
                    operator_stack_push(&opstack, op_node);
                } else {
                    struct operator_node *opstack_top = operator_stack_top(&opstack);
                    while ( (opstack_top->value != OPEN_PAREN_OP && opstack_top->value != THEN_OP
                            && opstack_top->value != ELSE_OP && opstack_top->value != IF_OP 
                            && opstack_top->value != DO_OP && opstack_top->value != WHILE_OP
                            && opstack_top->value != UNTIL_OP)
                            && operator_type <= opstack_top->value) {
                                struct operator_node *popped_operator = operator_stack_pop(&opstack);
                                struct command_node *second_command   = command_stack_pop(&comstack);
                                struct command_node *first_command    = command_stack_pop(&comstack);
                                struct command_node *combined_command = combine_two_commands(first_command, second_command, popped_operator->value);
                                
                                command_stack_push(&comstack, *combined_command);
                                
                                opstack_top = operator_stack_top(&opstack);
                                if(opstack_top == NULL)
                                    break;
                    }
                    operator_stack_push(&opstack, op_node);
                    // if (is_special_word) {
                    //     if (spec_op_stack.top != NULL) {
                    //         int top_operator_value = spec_op_stack.top->value;
                    //         if((operator_type == THEN_OP && top_operator_value == IF_OP)  ||
                    //           (operator_type == ELSE_OP && top_operator_value == IF_OP)  ||
                    //           (operator_type == FI_OP && top_operator_value == ELSE_OP)  ||
                    //           (operator_type == FI_OP && top_operator_value == THEN_OP)  ||
                    //           (operator_type == DO_OP && top_operator_value == WHILE_OP) ||
                    //           (operator_type == DO_OP && top_operator_value == UNTIL_OP) ||
                    //           (operator_type == DONE_OP && top_operator_value == DO_OP)  ||
                    //           (operator_type == CLOSE_PAREN_OP && top_operator_value == OPEN_PAREN_OP)) {
                    //                 operator_stack_push(&spec_op_stack, op_node);
                    //         } else 
                    //                 exit(312);
                    //     } else {
                    //         exit(313);
                    //     }
                    // }
                    
                }
                if(operator_stack_top(&opstack)->value == FI_OP || operator_stack_top(&opstack)->value == DONE_OP
                        || operator_stack_top(&opstack)->value == CLOSE_PAREN_OP) 
                {
                    follows = COMMAND;
                    free(operator_stack_pop(&opstack));  // don't need FI_OP or DONE_OP or CLOSE_PAREN_OP
                    free(operator_stack_pop(&spec_op_stack));
                    struct operator_node *popped_operator = operator_stack_pop(&opstack);
                    free(operator_stack_pop(&spec_op_stack));
                    if(popped_operator->value == ELSE_OP) {
                        struct command_node* third_command  = command_stack_pop(&comstack);
                        struct command_node* second_command = command_stack_pop(&comstack);
                        struct command_node* first_command  = command_stack_pop(&comstack);
                        struct command_node* combined_command    = combine_three_commands(first_command, second_command, third_command, IF_OP);
                        command_stack_push(&comstack, *combined_command);
                        free(operator_stack_pop(&opstack)); //should be popping THEN_OP
                        free(operator_stack_pop(&opstack)); //should be popping IF_OP
                        free(operator_stack_pop(&spec_op_stack));
                        free(operator_stack_pop(&spec_op_stack));
                    } else if(popped_operator->value == THEN_OP) {
                        struct command_node* second_command = command_stack_pop(&comstack);
                        struct command_node* first_command  = command_stack_pop(&comstack);
                        struct command_node* combined_command    = combine_two_commands(first_command, second_command, IF_OP);
                        command_stack_push(&comstack, *combined_command);
                        free(operator_stack_pop(&opstack)); //should be popping IF_OP
                        free(operator_stack_pop(&spec_op_stack));
                    } else if(popped_operator->value == DO_OP) {
                        struct command_node* second_command = command_stack_pop(&comstack);
                        struct command_node* first_command  = command_stack_pop(&comstack);
                        struct command_node* combined_command;
                        popped_operator = operator_stack_pop(&opstack);  // should be WHILE_OP or UNTIL_OP nodes
                        free(operator_stack_pop(&spec_op_stack));
                        if(popped_operator->value == WHILE_OP)
                            combined_command = combine_two_commands(first_command, second_command, WHILE_OP);
                        else if(popped_operator->value == UNTIL_OP)
                            combined_command = combine_two_commands(first_command, second_command, UNTIL_OP);
                        else {
                            //exit(180);
                            strcpy(error_description, "improper compound command keyword follows 'do' keyword");
                            print_error_message(line_number_ref, error_description);
                        }
                        command_stack_push(&comstack, *combined_command);
                    } else if(popped_operator->value == OPEN_PAREN_OP){ 
                        struct command_node* popped_command_node = command_stack_pop(&comstack);
                        struct command_node* subshell_command_node = (struct command_node*)malloc(sizeof(struct command_node));
                        
                        subshell_command_node->command = (struct command*)malloc(sizeof(struct command));
                        
                        subshell_command_node->command->type         = SUBSHELL_COMMAND;
                        subshell_command_node->command->status       = -1;
                        subshell_command_node->command->input        = NULL;
                        subshell_command_node->command->output       = NULL;
                        subshell_command_node->command->u.command[0] = popped_command_node->command;
                        
                        command_stack_push(&comstack, *subshell_command_node);
                    } else {
                        // error
                        //printf("error with compound command");
                        //exit(34);
                        strcpy(error_description, "compound command order inconsistency");
                        print_error_message(line_number_ref, error_description);
                    }    
                }
                
                
                
                // if encounter new commands (simple command)
                //     put them on command stack
                // if encounter new operator
                //     if operator_stack == NULL
                //         Add new operator to operator stack
                //     else if precedence(new operator) > precedence(opstack.top) || new operator == 'if' 
                //         Add new oparator to operator stack
                //     else
                //         while top.operator != (OPEN.PARENTHESIS || 'then' || 'else' || 'if') && precedence(new operator) <= precedence(top operator)
                //           {
                //             operator = operator-stack.pop()
                //             second-command = command-stack.pop()
                //             first-command = command-stack.pop()
                //             new-command = combine(first-command, second-command, operator)
                //             command-stack.push(new-command)
                //             top-operator = operator-stack.peek()
                //             if top-operator == NULL
                //                 break;
                //           }
                //         operator-stack.push(new_operator)
                //      if(operator-stack.top == "fi")
                //             operator-stack.pop()
                //             if(operator-stack.top == "then")
                //                 pop and process twice
                //             else if(operator-stack.top == "else")
                //                 pop and process 3 times
            }
        }

        if(c == EOF)
            break;
            
        if (!is_special_word && !is_redirect) {
            last_byte = c;
        }
    }
    
    
    
    // commented out to not create infinite loop
    // while(!operator_stack_empty(&opstack))
    // {
    //     // Do some processing if necessary. 
    // }

    while (!command_stack_empty(&comstack)) {
        node = command_stack_pop(&comstack);
        command_stream_push(comstream, *node);
        //command_stream_push(&comstream, *node);   
    }
    
    return comstream;  // We are C hackers
}

command_t
read_command_stream (command_stream_t s)
{
    /* FIXME: Replace this with your implementation too.  */
    
    command_t c = NULL;
    if (s->head != NULL) {
        c = s->head->command;
        s->head = s->head->next;
    }
    
    return c;
}
