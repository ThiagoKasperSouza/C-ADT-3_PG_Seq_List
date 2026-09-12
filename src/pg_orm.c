#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "pg_orm.h"

void close_and_finish(PGconn *conn) {
    PQfinish(conn);
}

PGconn* connect_to_pg(char* conninfo) {
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erro ao conectar: %s", PQerrorMessage(conn));
        close_and_finish(conn);
    }
    return conn;
}

void verify_db(PGconn *conn, char* db_name) {
    const char *params[1] = { db_name };
    PGresult *res = PQexecParams(
        conn,
        "SELECT 1 FROM pg_database WHERE datname = $1;",
        1,          /* Número de parâmetros */
        NULL,       /* Tipos dos parâmetros (deixa o Postgres inferir) */
        params,     /* Valores dos parâmetros */
        NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Erro ao verificar existência: %s", PQerrorMessage(conn));
        PQclear(res);
        close_and_finish(conn);
    }

    int exists = PQntuples(res);
    PQclear(res);

    if(!exists) create_db(conn,db_name);
}

void create_db(PGconn *conn, char* db_name) {
    // Nomes de banco não podem ser passados via parametros $1 no CREATE DATABASE.
    // Monte a string com atencao (evite SQL Injection se a variavel vier de input externo).
    char query[128];
    snprintf(query, sizeof(query), "CREATE DATABASE %s;", db_name);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Erro ao criar banco de dados: %s", PQerrorMessage(conn));
        PQclear(res);
        close_and_finish(conn);
    }

    printf("Banco de dados '%s' criado com sucesso!\n", db_name);
    PQclear(res);
}

PGresult* exec_insert(PGconn *conn, const char *table, const char *cols, int n_params, const char * const *param_values) {
    // Monta dynamicamente: INSERT INTO tabela (id, nome) VALUES ($1, $2)
    char query[128];
    char placeholders[128] = "";

    // Cria a lista de $1, $2, $3...
    for (int i = 1; i <= n_params; i++) {
        char buf[10];
        snprintf(buf, sizeof(buf), "%s$%d", (i > 1 ? ", " : ""), i);
        strcat(placeholders, buf);
    }

    snprintf(query, sizeof(query), "INSERT INTO %s (%s) VALUES (%s);", table, cols, placeholders);

    // PQexecParams executa de forma segura sanitizando os valores automaticamente
    PGresult *res = PQexecParams(
        conn,
        query,
        n_params,      // Número de parâmetros
        NULL,          // Deixa o Postgres inferir os tipos
        param_values,  // Array com as strings dos valores
        NULL,          // Tamanho dos parâmetros (só necessário para dados binários)
        NULL,          // Formato dos parâmetros (text)
        0              // Retorno em formato texto
    );

    return res;
}

PGresult* exec_read_list(PGconn *conn, char *cols, char *table){
    char query[128];
    snprintf(query, sizeof(query), "SELECT %s FROM %s;", cols, table);
    return PQexec(conn, query);
};

PGresult* exec_single_read(PGconn *conn,char *cols, char *table, char *where) {
    char query[128];
    snprintf(query, sizeof(query), "SELECT %s FROM %s WHERE %s;", cols, table,where);
    return PQexec(conn, query);
}

PGresult* exec_update(PGconn *conn, char *sql, int n_params, const char * const *param_values) {
    PGresult *res = PQexecParams(
        conn, 
        sql, 
        n_params,             // Quantidade exata de marcadores ($1 a $4)
        NULL, 
        param_values,            // Array contendo os 4 valores
        NULL, 
        NULL, 
        0
    );
    return res;
}
PGresult* exec_delete(PGconn *conn, char *sql, int n_params, const char * const *param_values) {
    PGresult *res = PQexecParams(
        conn, 
        sql, 
        n_params,             // Quantidade exata de marcadores ($1 a $4)
        NULL, 
        param_values,            // Array contendo os 4 valores
        NULL, 
        NULL, 
        0
    );
    return res;
}

void create_table(PGconn *conn, char *sql) {

    // 4. Executar o comando SQL
    PGresult *res = PQexec(conn, sql);

    // 5. Verificar o resultado da execução
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Falha ao criar a tabela: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    printf("Tabela 'usuarios' verificada/criada com sucesso!\n");

    // 6. Limpar memória do resultado e fechar conexão
    PQclear(res);
}
