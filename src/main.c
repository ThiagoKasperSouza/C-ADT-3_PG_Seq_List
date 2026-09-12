#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "users.h"
#include "pg_orm.h"
#include "env_loader.h"


int main() {
    int status = load_env(".env");
    if (status != 0) {
        fprintf(stderr, "Erro ao carregar o arquivo .env!\n");
        return 1;
    }

    const char *host = getenv("HOST");
    const char *port = getenv("PORT");
    const char *db = getenv("DB_NAME");
    const char *user = getenv("USER");
    const char *pass = getenv("PASSWORD");

    char conninfo[256];

    snprintf(conninfo, sizeof(conninfo), "user=%s password=%s host=%s port=%s",user,pass,host,port);
    PGconn *conn = connect_to_pg(conninfo);


    verify_db(conn, getenv("DB_NAME"));

    char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id SERIAL PRIMARY KEY, "
                      "age INTEGER NOT NULL, "
                      "name VARCHAR(100) NOT NULL, "
                      "height REAL NOT NULL"
                      ");";
    create_table(conn,sql);

    User u;
    u.id = 0;
    strcpy(u.name, "Teste");
    u.age = 20;
    u.height = 1.75f;    

    insert_user(conn,u);

    UserList *l = read_list(conn);
    if (l != NULL && l->data != NULL) {
        for (int i = 0; i < l->qty; i++) {
            User u2 = l->data[i];
            printf("ID: %ld | Name: %s | Age: %d | Height: %.2f\n", u2.id, u2.name, u2.age, u2.height);
        }
        
        // Lembre-se de liberar a memória da lista retornada por read_list se tiver uma função para isso!
        // free_list(l);
    } else {
        printf("Nenhum usuário encontrado ou erro ao ler do banco.\n");
    }

    // 4. Libera conexao com o banco padrao
    close_and_finish(conn);
    return 0;
}