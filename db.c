#include <stdio.h>
#include <libpq-fe.h>
#include <stdlib.h>
#include <sodium.h>
#include <string.h>
#include "db.h"



PGconn* connect_db(const char *db_uri) {
    PGconn *conn = PQconnectdb("dbname=blackjack user=admin password=Yoloboyz89# host=localhost port=5432");
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    printf("Connected to the database\n");
    return conn;
}

void hash_password(const char *password, char *hashed_password, size_t hashed_size) {
    if (sodium_init() < 0) {
        fprintf(stderr, "Libsodium initialization failed\n");
        exit(1);
    }
    
    if (crypto_pwhash_str(hashed_password, password,
        strlen(password), crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Password hashing failed\n");
        exit(1);
    }
}

void close_db(PGconn *conn) {
    PQfinish(conn);
}

void register_player(PGconn *conn, const char *username, const char *password, const int *balance, const int *total_winnings) {
    if(sodium_init() < 0) {
        fprintf(stderr, "Libsodium initialization failed\n");
        return;
    }
    
    char hashed_password[crypto_pwhash_STRBYTES];
    hash_password(password, hashed_password, sizeof(hashed_password));
    
    const char *paramValues[4];
    char balance_str[16], winnings_str[16];
    
    snprintf(balance_str, sizeof(balance_str), "%d", *balance);
    snprintf(winnings_str, sizeof(winnings_str), "%d", *total_winnings);
    
    paramValues[0] = username;
    paramValues[1] = hashed_password;
    paramValues[2] = balance_str;
    paramValues[3] = winnings_str;
    
    PGresult *res = PQexecParams(conn, 
        "INSERT INTO Players (username, password, balance, total_winnings) VALUES ($1, $2, $3, $4)",
        4, NULL, paramValues, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {  
        fprintf(stderr, "Failed to add player: %s\n", PQerrorMessage(conn));
    } else {
        printf("Registered successfully ...\n");
    }
    
    PQclear(res);
}

User authenticate_player(PGconn *conn, const char *username, const char *password) {
    User user = {0};
    const char *paramValues[1];
    paramValues[0] = username;
    
    PGresult *res = PQexecParams(conn, 
        "SELECT id, username, password, balance, total_winnings FROM Players WHERE username = $1",
        1, NULL, paramValues, NULL, NULL, 0);
    
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Authentication query failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return user;
    }
    
    int rows = PQntuples(res);
    if(rows == 1) {

        const char *stored_hash = PQgetvalue(res, 0, 2);
    
        if(sodium_init() < 0) {
            fprintf(stderr, "Libsodium initialization failed\n");
            PQclear(res);
            return user;
        }
        
        if(crypto_pwhash_str_verify(stored_hash, password, strlen(password)) == 0) {
            user.id = atoi(PQgetvalue(res, 0, 0));
            strncpy(user.username, PQgetvalue(res, 0, 1), sizeof(user.username) - 1);
            user.username[sizeof(user.username) - 1] = '\0';
            strncpy(user.password, stored_hash, sizeof(user.password) - 1);
            user.password[sizeof(user.password) - 1] = '\0';
            user.balance = atoi(PQgetvalue(res, 0, 3));
            user.total_winnings = atoi(PQgetvalue(res, 0, 4));
		
	    printf("==========================\n");
            printf("Welcome %s\n", username);
        } else {
            printf("Incorrect password\n");
        }
    }
    
    PQclear(res);
    return user;
}

void update_balance(PGconn *conn, const char *username, const int balance) {
    const char *paramValues[2];
    char balance_str[16];
    
    snprintf(balance_str, sizeof(balance_str), "%d", balance);
    
    paramValues[0] = balance_str;
    paramValues[1] = username;
    
    PGresult *res = PQexecParams(conn, 
        "UPDATE Players SET balance = $1 WHERE username = $2",
        2, NULL, paramValues, NULL, NULL, 0);
    
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Failed to update balance: %s\n", PQerrorMessage(conn));
    } else {
        printf("Saved balance successfully...\n");
    }
    
    PQclear(res);
}

void check_leaderboards(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT username, balance, total_winnings FROM Players ORDER BY total_winnings DESC LIMIT 10");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to retrieve leaderboard: %s\n", PQerrorMessage(conn));
    } else {
        int rows = PQntuples(res);
        printf("\n===== LEADERBOARD =====\n");
        printf("%-20s %-15s %-15s\n", "Username", "Balance", "Total Winnings");
        printf("----------------------------------------\n");
        
        for (int i = 0; i < rows; i++) {
            printf("%-20s $%-14s $%-14s\n", 
                   PQgetvalue(res, i, 0),
                   PQgetvalue(res, i, 1), 
                   PQgetvalue(res, i, 2));
        }
        printf("========================\n");
    }
    
    PQclear(res);
}

void save_user_data(PGconn *conn, User user) {
    update_balance(conn, user.username, user.balance);
    
    const char *paramValues[2];
    char winnings_str[16];
    
    snprintf(winnings_str, sizeof(winnings_str), "%d", user.total_winnings);
    
    paramValues[0] = winnings_str;
    paramValues[1] = user.username;
    
    PGresult *res = PQexecParams(conn, 
        "UPDATE Players SET total_winnings = $1 WHERE username = $2",
        2, NULL, paramValues, NULL, NULL, 0);
    
    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Failed to update user data: %s\n", PQerrorMessage(conn));
    } else {
        printf("User data saved successfully...\n");
    }
    
    PQclear(res);
}

User load_user_data(PGconn *conn, const char *username) {
    User user = {0};
    
    const char *paramValues[1];
    paramValues[0] = username;
    
    PGresult *res = PQexecParams(conn, 
        "SELECT id, username, password, balance, total_winnings FROM Players WHERE username = $1",
        1, NULL, paramValues, NULL, NULL, 0);
    
    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to load user data: %s\n", PQerrorMessage(conn));
    } else {
        int rows = PQntuples(res);
        if(rows == 1) {
            user.id = atoi(PQgetvalue(res, 0, 0));
            strncpy(user.username, PQgetvalue(res, 0, 1), sizeof(user.username) - 1);
            user.username[sizeof(user.username) - 1] = '\0';
            strncpy(user.password, PQgetvalue(res, 0, 2), sizeof(user.password) - 1);
            user.password[sizeof(user.password) - 1] = '\0';
            user.balance = atoi(PQgetvalue(res, 0, 3));
            user.total_winnings = atoi(PQgetvalue(res, 0, 4));
        }
    }
    
    PQclear(res);
    return user;
}
