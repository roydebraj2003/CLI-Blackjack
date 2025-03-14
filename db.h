#ifndef DB_H
#define DB_H
#include <libpq-fe.h>
#include <string.h>

typedef struct {
    int id;
    char username[50];
    int balance;
    int total_winnings;
    char password[128];
} User;

PGconn* connect_db(const char *db_uri);
void close_db(PGconn *conn);
void register_player(PGconn *conn, const char *username, const char *password, const int *balance, const int *total_winnings);
User authenticate_player(PGconn *conn, const char *username, const char *password);
void update_balance(PGconn *conn, const char *username, const int balance);
void check_leaderboards(PGconn *conn);
void save_user_data(PGconn *conn, User user);
User load_user_data(PGconn *conn, const char *username);

#endif
