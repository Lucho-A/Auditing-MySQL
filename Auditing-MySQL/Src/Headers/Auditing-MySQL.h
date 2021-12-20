/*
 ============================================================================
 Name        : Auditing-MySql.h
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
*/

#ifndef AUDITING_MYSQL_H_
#define AUDITING_MYSQL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <mysql/mysql.h>
#include <dirent.h>

#define RETURN_OK 1
#define RETURN_ERROR -1

#define TRUE 1
#define FALSE 0

#define C_WHITE "\e[0;37m"
#define C_DEFAULT "\e[0m"
#define C_RED "\e[0;31m"
#define C_CYAN "\e[0;36m"
#define C_GREEN "\e[0;32m"

#define SERVER_IP "localhost"
#define SERVER_PORT 3306
#define USER "lucho"
#define PASS ""
#define DB "sys"
#define MYSQL_DIR "/var/lib/mysql"
#define MYSQL_CONFIG_DIR "/etc/mysql"
#define HOME_DIR "/home"

#define BRUTE_FORCE_DELAY 100000
#define PATH_TO_RESOURCES "/home/lucho/git/Auditing-MySQL/Auditing-MySQL/Resources/"

int mysql_brute_force(void);
int perform_mySQL_query(MYSQL *conn, MYSQL_RES **result, char *query);
int perform_MySQL_connection(MYSQL **conn);
void system_call(char *cmd);
int find_file(char *path, char *filename);
int find_file_rec(char *basePath, char *filename, int *fileFounds);
int open_file(char *fileName, FILE **f);

#endif /* AUDITING_MYSQL_H_ */
