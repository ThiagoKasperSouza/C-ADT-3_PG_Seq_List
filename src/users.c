#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "pg_orm.h"
#include "users.h"

// -- LISTAS --

UserList* create_list(int n) {
    UserList *l = (UserList*) malloc(sizeof(UserList)); // Aloca a estrutura da lista
    if (l == NULL) return NULL;

    l->qty = n;
    l->data = (User*) malloc(sizeof(User) * n); // Aloca o array de dados
    
    return l;
}


UserList* convert_to_list(PGresult *res) {
    int ntuples = PQntuples(res);

    if (ntuples == 0) {
        return NULL;
    }
    UserList *user_list = create_list(ntuples);

    // 3. Mapear cada linha do PGresult para uma struct
    // TODO: DEIXAR DINAMICO A ATRIBUICAO DAS VARS Q COMPOEM A STRUCT
    for (int i = 0; i < ntuples; i++) {
        user_list->data[i].id = atoi(PQgetvalue(res, i, 0));
    
        strncpy(user_list->data[i].name, PQgetvalue(res, i, 1), sizeof(user_list->data[i].name) - 1);
        user_list->data[i].name[sizeof(user_list->data[i].name) - 1] = '\0'; // Garante o caractere nulo
        
        user_list->data[i].age = atoi(PQgetvalue(res, i, 2));
        user_list->data[i].height = atof(PQgetvalue(res, i, 3));
    }
    return user_list;
}

int list_size(UserList *l ) {
    if(l == NULL) return -1;
    return l->qty;
}
int is_empty_list(UserList *l) {
    if(l == NULL) return 1;
    if(l->qty > 0) return 0;
}

void delete_list(UserList *l) {
    free(l->data);
    free(l);
    l = NULL;
}

// CRUD
void insert_user(PGconn *conn, User u) {
    char *cols = "name,age,height";
    
    // A libpq espera que CADA parâmetro seja uma string individual separada.
    char age_str[12], height_str[16];
    
    snprintf(age_str, sizeof(age_str), "%d", u.age);
    snprintf(height_str, sizeof(height_str), "%.2f", u.height);

    // Deve ser 'const char * const pv[]', sem o '*' extra no tipo da variável.
    const char * const pv[] = {
        u.name,
        age_str,
        height_str
    };

    PGresult *res = exec_insert(conn, "users",cols, 3, pv); 
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Erro de Execução no Banco: %s\n", PQerrorMessage(conn));
        PQclear(res);
    }

}
UserList* read_list(PGconn *conn) {
    char *cols = "id,name,age,height";
    PGresult *res = exec_read_list(conn,cols, "users");
    return convert_to_list(res);
}

// UserList* read_single_user(PGconn *conn, char *where) {
//     char *cols = "id,name,age,height";
//     PGresult *res = exec_single_read(conn,cols, "users",where);
//     PQclear(res);
//     return convert_to_list(res);
// }

void update_user(PGconn *conn, User u) {
    char *sql = "UPDATE users SET name = $1, age = $2, height = $3 WHERE id = $4;";

    char id_str[21];       // Garante espaço para longs de até 64 bits + sinal + '\0'
    char age_str[12];      // Suficiente para inteiros
    char height_str[16];   // Suficiente para números de ponto flutuante

    // Converta usando os especificadores de formato adequados
    snprintf(id_str, sizeof(id_str), "%ld", u.id);
    snprintf(age_str, sizeof(age_str), "%d", u.age);
    snprintf(height_str, sizeof(height_str), "%.2f", u.height);

    // Exemplo com 4 parâmetros: name ($1), age ($2), height ($3), id ($4)
    const char * const pv[] = { u.name, age_str, height_str, id_str };


    
    PGresult *res = exec_update(conn,sql, 4, pv);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Erro de Execução no Banco: %s\n", PQerrorMessage(conn));
        PQclear(res);
    }
}
void delete_user(PGconn *conn, User u) {
    
    char *sql = "DELETE FROM users WHERE id = $1;";
    char id_str[21];       // Garante espaço para longs de até 64 bits + sinal + '\0'
    // Converta usando os especificadores de formato adequados
    snprintf(id_str, sizeof(id_str), "%ld", u.id);

    const char * const pv[] = { id_str };
    PGresult *res = exec_delete(conn,sql, 1, pv);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Erro de Execução no Banco: %s\n", PQerrorMessage(conn));
        PQclear(res);
    }
}
