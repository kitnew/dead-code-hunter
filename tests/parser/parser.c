#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

#define MAX_PATH_LENGTH 1000
#define MAX_ELEMENTS_COUNT 1000

typedef enum {
    ELEMENT_INCLUDE,
    ELEMENT_FUNCTION,
    ELEMENT_MACRO,
    ELEMENT_UNKNOWN,
    ELEMENT_VARIABLE
} CodeElementsType;

typedef struct {
    CodeElementsType type;
    char *filepath;
    char *name;
    int line;
    char **calls;
    int calls_count;
} CodeElement;

static CodeElementsType parse_line(const char *line) {
    if (line == NULL) {
        return ELEMENT_UNKNOWN;
    }

    const char *pattern_function = "^(\\w+)\\s+(\\w+)\\s+(\\w+)\\s*\\(([^)]*)\\)\\s*\\{";
    const char *pattern_function2 = "^(\\w+)*\\s*(\\w+)\\s+(\\w+)\\s*\\(([^)]*)\\)\\s*\\{";
    const char *pattern_macro = "(#\\w+)\\s(\\w+)\\s(\\d+|\\w+)";
    const char *pattern_include = "(#\\w+)\\s([<\"][a-zA-Z_\\./]*[\">])";
    const char *pattern_variable = "(\\w*)\\s*([a-zA-Z_\\*\\[\\]]+)\\s+([a-zA-Z_\\*\\[\\]]+)\\s+\\=\\s+([a-zA-Z\"]*)\\;";

    regex_t regex;
    int ret;
    
    // Проверяем include
    ret = regcomp(&regex, pattern_include, REG_EXTENDED);
    if (ret == 0) {
        ret = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (ret == 0) {
            return ELEMENT_INCLUDE;
        }
    }

    // Проверяем первый паттерн функции
    ret = regcomp(&regex, pattern_function, REG_EXTENDED);
    if (ret == 0) {
        ret = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (ret == 0) {
            return ELEMENT_FUNCTION;
        }
    }

    // Проверяем второй паттерн функции
    ret = regcomp(&regex, pattern_function2, REG_EXTENDED);
    if (ret == 0) {
        ret = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (ret == 0) {
            return ELEMENT_FUNCTION;
        }
    }

    // Проверяем макрос
    ret = regcomp(&regex, pattern_macro, REG_EXTENDED);
    if (ret == 0) {
        ret = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (ret == 0) {
            return ELEMENT_MACRO;
        }
    }

    // Проверяем переменную
    ret = regcomp(&regex, pattern_variable, REG_EXTENDED);
    if (ret == 0) {
        ret = regexec(&regex, line, 0, NULL, 0);
        regfree(&regex);
        if (ret == 0) {
            return ELEMENT_VARIABLE;
        }
    }

    return ELEMENT_UNKNOWN;
}

static CodeElementsType parse_element_type(const char *line) {
    if (line == NULL) {
        return ELEMENT_UNKNOWN;
    }

    return parse_line(line);

    if (strncasecmp(line, "#include", 8) == 0) {
        return ELEMENT_INCLUDE;
    } else if (strncasecmp(line, "#define", 7) == 0) {
        return ELEMENT_MACRO;
    }
    return ELEMENT_UNKNOWN;
}

static char* parse_element_name(const char *line) {
    if (line == NULL) {
        return NULL;
    }
    
    char *ptr = strpbrk(line, " ");
    if (ptr != NULL) {
        return strdup(ptr + 1);
    }
    
    return NULL;
}

CodeElement *parse_file(const char *filepath, int *element_count) {
    if (filepath == NULL || element_count == NULL) {
        return NULL;
    }

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return NULL;
    }

    CodeElement *elements = malloc(sizeof(CodeElement) * MAX_ELEMENTS_COUNT);
    if (elements == NULL) {
        return NULL;
    }

    int line_count = 1;
    int iter = 0;
    while(!feof(file) && iter < MAX_ELEMENTS_COUNT) {
        //printf("%d\n", iter);
        CodeElement *element = malloc(sizeof(CodeElement));
        if (element == NULL) {
            return NULL;
        }
        char *line = malloc(sizeof(char) * MAX_PATH_LENGTH);
        if (line == NULL) {
            return NULL;
        }
        line = fgets(line, MAX_PATH_LENGTH, file);
        if (line == NULL || strcmp(line, "\n") == 0) {
            free(line);
            line_count++;
            continue;
        }
        element->type = parse_element_type(line);
        element->filepath = strdup(filepath);
        element->name = parse_element_name(line);
        element->line = line_count;
        element->calls = NULL;
        element->calls_count = 0;
        elements[iter] = *element;
        iter++;
        line_count++;
        free(line);
    }

    *element_count = iter;
    fclose(file);
    return elements;
}

int main() {
    char filepath[MAX_PATH_LENGTH] = "../test_folder/file_scanner.c";
    int element_count = 0;
    CodeElement *elements = parse_file(filepath, &element_count);
    if (elements == NULL) {
        return 1;
    }
    for (int i = 0; i < element_count; i++) {
        printf("[%s] %s at line %d\n", elements[i].type == ELEMENT_INCLUDE ? "include" : elements[i].type == ELEMENT_FUNCTION ? "function" : elements[i].type == ELEMENT_MACRO ? "macro" : elements[i].type == ELEMENT_UNKNOWN ? "unknown" : elements[i].type == ELEMENT_VARIABLE ? "variable" : "unknown", elements[i].name, elements[i].line);
    }
    free(elements);
    return 0;
}