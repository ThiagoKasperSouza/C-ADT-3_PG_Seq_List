#ifndef USERS_H
#define USERS_H

struct user {
    unsigned long int id;
    char name[40];
    int age;
    float height;
};

typedef struct user User;
struct list {
    int qty;
    User *data; 
};

typedef struct list UserList;

// LISTS
UserList* convert_to_list(PGresult* res);
UserList* create_list(int n);
int list_size(UserList *l );
int is_empty_list(UserList *L);
void delete_list(UserList* l);


User* get_user(UserList *l, int index);

//CRUD
void insert_user(PGconn *conn, User user);
UserList* read_list(PGconn *conn);
UserList* read_single_user(PGconn *conn, char *where);
void update_user(PGconn *conn, User user);
void delete_user(PGconn *conn, User user);
#endif
