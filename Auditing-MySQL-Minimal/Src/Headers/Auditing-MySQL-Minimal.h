/*
 ============================================================================
 Name        : Auditing-MySql-Minimal.h
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
*/

#ifndef AUDITING_MYSQL_MINIMAL_H_
#define AUDITING_MYSQL_MINIMAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <mysql/mysql.h>
#include <dirent.h>

#define RETURN_OK 1
#define RETURN_ERROR 0

#define TRUE 1
#define FALSE 0

#define C_DEFAULT "\e[0m"
#define C_RED "\e[0;31m"
#define C_HRED "\e[0;91m"
#define C_GREEN "\e[0;32m"
#define C_YEL "\e[0;33m"
#define C_CYAN "\e[0;36m"
#define C_WHITE "\e[0;37m"
#define C_RESULTS C_YEL

#define SERVER_IP "localhost"
#define SERVER_PORT 3306
#define USER "Auditing-MySQL"
#define PASS "Auditing-MySQL-Pass"
#define DB ""
#define MYSQL_DIR "/var/lib/mysql"
#define HOME_DIR "/home"

#define USERNAMES 3
#define PASSWORDS 3

#define BRUTE_FORCE_DELAY 100000

int perform_MySQL_connection(MYSQL **conn);
int perform_mySQL_query(MYSQL *conn, MYSQL_RES **result, char *query);
int perform_mySQL_brute_force(void);
void system_call(char *cmd);
int find_file(char *path, char *filename);
int find_file_rec(char *basePath, char *filename, int *fileFounds);
int open_file(char *fileName, FILE **f);
void show_intro(void);
void show_error(char *errMsg, int errnum);

#endif /* AUDITING_MYSQL_MINIMAL_H_ */
