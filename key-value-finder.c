/*
    key_value_finder
    date: july 2022.
    author: Taras Vytvytskyi.
    written as an assignment for Poznan University of Tech. (PUT) courses for ukrainian students.

    The aim of the project is to be able to read files of key-value pairs
    that are organized as 'key = value' and output values of input keys
    and also execute simple arithmetic operations.
    Beware, that large files can crush the program, because the whole file is fetched into memory
    and only then the search commences.
    The file is stored as sections and key-value pairs as in a one-linked list:
        section 1
            key-value1
            key-value2
        section 2
            key-value3
        and so on
    The code is tested with valgrind for errors and warnings.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// into the buffer, each new line is fed
#define BUFFER_SIZE 256
struct key_value
{
    char *key;
    char *value;
    struct key_value *next;
};
struct section
{
    char *id;
    struct section *next_section;
    struct key_value *start;
};

// checking for correctness of the string
int is_cor_str(char *in)
{
    char *pt = in;
    char *end = in + strlen(in) - 1; // "-1" for the null terminating character
    while (pt++ < end)
    {
        if (!isalnum(*pt) && (*pt != '-'))
        {
            return 0;
        }
    }
    return 1;
}

// function is responsible for creating a section struct out of the fetched line
struct section *handle_section(char *line)
{
    int len = strlen(line);
    if (line[len - 2] != ']')
    { //"-2" to check if the last char is ']' but not the '\n' char
        printf("section is declared incorrectly:%s\n", line);
        return NULL;
    }

    char *cur_section = malloc(len - 2); //"-2" for '[' and ']' characters, last '\n' char will be replaced by '\0'
    int ndx = 0;
    for (int i = 1; i < len - 2; i++)
    {
        cur_section[ndx++] = line[i];
    }

    cur_section[len - 3] = '\0';
    if (!is_cor_str(cur_section))
    {
        printf("Section name is not alphanumeric: %s\n", cur_section);
        free(cur_section); // if program exits at this point, the section name will not be written in the linked list and thus won't be cleaned
        return NULL;
    }

    struct section *sn = malloc(sizeof(struct section));
    sn->id = cur_section;
    sn->start = NULL;
    sn->next_section = NULL;
    return sn;
}

struct key_value *handle_line(char *line)
{
    char *key = strtok(line, " =");
    char *value = strtok(NULL, " =\n"); //'\n' is added so that the last char('\n') is simply ignored
    if (value == NULL)
    {
        printf("Error creating key-value pair\n");
        return NULL;
    }
    if (!is_cor_str(key) || !is_cor_str(value))
    {
        printf("Key or value is not alphanumeric: %s=%s\n", key, value);
        return NULL;
    }
    struct key_value *result = malloc(sizeof(struct key_value));
    result->key = strdup(key);
    result->value = strdup(value);
    result->next = NULL;
    return result;
}

struct key_value *search(struct section *begin, char *line)
{
    struct section *sect;
    char *section_to_find = strtok(line, ".");
    char *key_to_find = strtok(NULL, ".");
    if (!is_cor_str(section_to_find) || !is_cor_str(key_to_find))
    {
        printf("input data or key strings are not alphanumeric");
        return NULL;
    }
    else
    {
        sect = begin;
        while (sect != NULL)
        { // begin searching for input section
            if (!strcmp(sect->id, section_to_find))
            {
                struct key_value *kv = sect->start;
                while (kv != NULL)
                {
                    if (strcmp(kv->key, key_to_find) == 0)
                    {
                        printf("Correspondning key-value pair is found: %s = %s\n", kv->key, kv->value);
                        return kv;
                    }
                    kv = kv->next;
                }
                printf("Such key doesn\'t exist in said section\n");
                return NULL;
            }
            sect = sect->next_section;
        }
        printf("Failed to find such section!\n");
        return NULL;
    }
}

void *execute_expression(char *expression, struct section *begin)
{
    struct key_value *arg1;
    struct key_value *arg2;
    char *exprs = strtok(expression, "\""); // clear the last and the begging " character
    char *argument1 = strtok(exprs, " ");   //"sect.key + sect.key"
    char *operator= strtok(NULL, " ");
    char *argument2 = strtok(NULL, " ");
    arg1 = search(begin, argument1);
    arg2 = search(begin, argument2);
    if ((arg1 == NULL) || (arg2 == NULL))
    {
        printf("Error searching for input keys, aborting...");
        return NULL;
    }
    char *arg_str1 = arg1->value;
    char *arg_str2 = arg2->value;
    int arg_int1 = atoi(arg_str1);
    int arg_int2 = atoi(arg_str2);
    if (!arg_int1 != !arg_int2)
    { // logical XOR operation
        printf("Values are not of the same type!\n");
        return NULL;
    }
    else
    {
        if (arg_int1 == 0)
        { // this means the values are in str
            if (strcmp(operator, "+") != 0)
            {
                printf("Invalid operator for strings");
                return NULL;
            }
            char *res = malloc(strlen(arg_str1) + strlen(arg_str2)); //*sizeof(char) (=1)
            res = strdup(arg_str1);
            strcat(res, arg_str2);
            printf("The result is: %s\n", res);
            return res;
        }
        else
        { // this means values are int
            int *result = malloc(sizeof(int));
            switch (operator[0])
            {
            case '*': // multiplication
                *result = arg_int1 * arg_int2;
                break;
            case '+':
                *result = arg_int1 + arg_int2;
                break;
            case '-':
                *result = arg_int1 - arg_int2;
                break;
            case '/':
                *result = arg_int1 / arg_int2;
                break;
            default:
                printf("Unknown operator, aborting...\n");
                return NULL;
                break;
            }
            printf("The result is: %d\n", *result);
            return result;
        }
    }
}

// program begins here
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("One or more of the input parameters wasn\'t specified");
        return 1;
    }
    // reading part
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Trying to open a non-existing file");
        return 1;
    }
    char buffer[BUFFER_SIZE];
    struct section *sect;
    struct section *begin;
    struct key_value *prev_kv;
    struct key_value *kv;

    while (fgets(buffer, BUFFER_SIZE, f))
    {
        if (buffer[0] == '[')
        {
            struct section *sn = handle_section(buffer);
            if (sn == NULL)
            {
                printf("Error creating first section, skipping untill a correct one is found\n");
                continue;
            }
            begin = sn;
            sect = sn;
            break;
        }
    }

    while (fgets(buffer, BUFFER_SIZE, f))
    {
        if ((buffer[0] == '\n') || (buffer[0] == ';'))
        {
            continue;
        }
        if (buffer[0] == '[') // constructing one linked list of sections
        {
            struct section *sn = handle_section(buffer);
            if (sn == NULL)
            {
                printf("Error creating a section!\n");
                continue;
            }
            sect->next_section = sn;
            sect = sn;
        }
        else // constructing one-linked list of key-value pairs
        {
            struct key_value *kv = handle_line(buffer);
            if (kv == NULL)
            {
                printf("Error creating a key-value pair!\n");
                continue;
            }
            if (sect->start == NULL)
            {
                sect->start = kv;
            }
            else
            {
                prev_kv->next = kv;
            }
            prev_kv = kv;
        }
    }
    // searching part
    void *result = NULL;
    if (strcmp(argv[2], "expression") == 0)
    {
        result = execute_expression(argv[3], begin);
    }
    else
    {
        kv = search(begin, argv[2]);
    }
    // clearing up part
    fclose(f);
    free(result);
    struct section *prev_sect;
    sect = begin;
    while (sect != NULL)
    {
        kv = sect->start;
        while (kv != NULL)
        {
            prev_kv = kv;
            kv = kv->next;
            free(prev_kv->key);
            free(prev_kv->value);
            free(prev_kv);
        }
        prev_sect = sect;
        sect = sect->next_section;
        free(prev_sect->id);
        free(prev_sect);
    }
    return 0;
}
