#ifndef PG_HEADER
#define PG_HEADER


void verify_db(PGconn *conn, char* db_name);
void create_db(PGconn *conn, char* db_name);
void close_and_finish(PGconn *conn);
PGconn* connect_to_pg(char *db);

//table
void create_table(PGconn *conn, char* sql);

// CRUD OPS
PGresult* exec_read_list(PGconn *conn,char *cols, char *table);
PGresult* exec_insert(PGconn *conn, const char *table, const char *cols, int n_params, const char * const *param_values);
PGresult* exec_single_read(PGconn *conn,char *cols, char *table, char *where);
PGresult* exec_update(PGconn *conn, const char *table, const char *set_clause, const char *where_clause, int n_params, const char * const *param_values);
PGresult* exec_delete(PGconn *conn, const char *table, const char *where_clause, int n_params, const char * const *param_values);
#endif