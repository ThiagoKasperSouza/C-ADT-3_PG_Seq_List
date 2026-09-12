#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "env_loader.h"

// Remove espaços em branco do início e do fim de uma string
char *trim(char *str) {
    char *end;

    // Remove espaços do início
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return str;

    // Remove espaços do fim
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}

int load_env(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo .env");
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char *trimmed = trim(line);

        // Ignora linhas vazias ou comentários
        if (trimmed[0] == '\0' || trimmed[0] == '#') {
            continue;
        }

        // Procura pelo primeiro sinal de '='
        char *delimiter = strchr(trimmed, '=');
        if (!delimiter) {
            continue; // Linha inválida (sem '=')
        }

        *delimiter = '\0'; // Divide a string em duas
        char *key = trim(trimmed);
        char *value = trim(delimiter + 1);

        // Remove aspas da string se houver (ex: "valor" ou 'valor')
        size_t len = strlen(value);
        if (len >= 2 && ((value[0] == '"' && value[len - 1] == '"') || 
                         (value[0] == '\'' && value[len - 1] == '\''))) {
            value[len - 1] = '\0';
            value++;
        }

        // Define a variável de ambiente (1 = sobrescreve se já existir)
#if defined(_WIN32)
        // No Windows usa-se _putenv_s
        _putenv_s(key, value);
#else
        // POSIX (Linux/macOS)
        setenv(key, value, 1);
#endif
    }

    fclose(file);
    return 0;
}