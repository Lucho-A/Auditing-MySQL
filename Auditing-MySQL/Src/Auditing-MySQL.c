/*
 ============================================================================
 Name        : Auditing-MySql.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description : Script for auditing MySQL databases in C, Ansi-style
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <mysql/mysql.h>
#include <dirent.h>

#define SERVER_IP "localhost"
#define SERVER_PORT 3306
#define USER "lucho"
#define PASS ""
#define MYSQL_DIR "/var/lib/mysql"
#define MYSQL_CONFIG_DIR "/etc/mysql"
#define HOME_DIR "/home"
#define C_WHITE "\e[0;37m"
#define C_DEFAULT "\e[0m"
#define C_RED "\e[0;31m"
#define C_CYAN "\e[0;36m"
#define C_GREEN "\e[0;32m"

void find_file(char *basePath, char *filename);

int main(void) {
	if(getuid()!=0){
		printf("\n%sYou must be root for running the program.\n\n", C_RED);
		exit(EXIT_FAILURE);
	}
	system("clear");
	printf("%s",C_CYAN);
	printf("\n*******************************************************\n");
	printf("\nAuditing MYSQL Script by L.\n");
	printf("\nv0.0.1\n");
	printf("\n*******************************************************\n");
	printf("%s",C_WHITE);
	time_t timestamp = time(NULL);
	struct tm tm = *localtime(&timestamp);
	printf("\nStarting at: %d/%02d/%02d %02d:%02d:%02d\n\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("%s",C_DEFAULT);
	char cmd[128]="";
	char url[128]="";
	snprintf(url, sizeof(url), "%s", SERVER_IP);
	MYSQL *conn = mysql_init(NULL);
	if (conn == NULL){
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(EXIT_FAILURE);
	}
	printf("%sConnecting to MYSQL: \n", C_WHITE);
	if (mysql_real_connect(conn, url, USER, PASS, "sys", SERVER_PORT, NULL, 0) == NULL){
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
		exit(EXIT_FAILURE);
	} else{
		printf("\n%sOK\n\n", C_GREEN);
	}
	//1
	printf("%s1) Identify the MySQL client version: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_client_info());
	//2
	printf("%s2) Identify the MySQL host info: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_host_info(conn));
	//3
	printf("%s3) Identify the MySQL server info: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_server_info(conn));
	//4
	printf("%s4) Evaluate running the server as 'mysql' (or another unprivileged user): \n",C_WHITE);
	printf("%s\n",C_GREEN);
	system("ps aux | grep -i mysql");
	printf("%s\n", C_WHITE);
	//5
	printf("%s5) Evaluate the owner of the MYSQL directory (mysql.mysql): \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_DIR);
	printf("%s\n", C_GREEN);
	system(cmd);
	printf("%s\n", C_WHITE);
	//6
	printf("%s6) Evaluate the owner of the MYSQL config directory (root.root): \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_CONFIG_DIR);
	printf("%s\n", C_GREEN);
	system(cmd);
	printf("%s\n", C_WHITE);
	MYSQL_RES *result = NULL;
	char query[1024]="";
	int numRows =0;
	//7
	printf("%s7) Evaluate the existence of 'my.cnf' files in /home directory: \n",C_WHITE);
	find_file(HOME_DIR,"my.cnf");
	printf("\n");
	//8
	printf("%s8) Evaluate the existence of users with account not locked, and without requiring a password to connect to MySQL: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host from mysql.user where authentication_string ='' and account_locked = 'N';");
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
	}else{
		result = mysql_store_result(conn);
		numRows = mysql_num_rows(result);
		MYSQL_ROW row;
		printf("\nUsers found: %s%d\n", C_RED, numRows);
		printf("%s\n", C_RED);
		while ((row = mysql_fetch_row(result))){
			printf("%s (%s)", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
			printf("\n");
		}
	}
	printf("\n");
	//9
	printf("%s9) Evaluate the existence of user named 'root' with account not locked: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host from mysql.user where user='root' and account_locked = 'N';");
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
	}else{
		result = mysql_store_result(conn);
		numRows = mysql_num_rows(result);
		(numRows>0)?(printf("\n%sRoot user found\n", C_RED)):(printf("\n%sNo Root user found\n", C_GREEN));
	}
	printf("\n");
	//10
	printf("%s10) Evaluate the existence of anonymous/empty users with account not locked: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host from mysql.user where user='' and account_locked = 'N';");
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
	}else{
		result = mysql_store_result(conn);
		numRows = mysql_num_rows(result);
		(numRows>0)?(printf("\n%sAnonymous users found\n", C_RED)):(printf("\n%sNo Anonymous users found\n", C_GREEN));
	}
	printf("\n");
	//11
	printf("%s11) Evaluate the users with account not locked, and with privileges in order to analize their rational: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host from mysql.user where (Select_priv = 'Y' "
			"OR Insert_priv = 'Y' OR Update_priv = 'Y' OR Delete_priv = 'Y' OR Create_priv = 'Y' "
			"OR Drop_priv = 'Y' OR Grant_priv = 'Y' OR Index_priv = 'Y' OR Alter_priv = 'Y') and account_locked = 'N';");
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
	}else{
		result = mysql_store_result(conn);
		numRows = mysql_num_rows(result);
		MYSQL_ROW row;
		printf("\nUsers found: %s%d\n", C_RED, numRows);
		printf("%s\n", C_RED);
		while ((row = mysql_fetch_row(result))){
			printf("%s (%s)", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
			printf("\n");
		}
	}
	printf("\n");
	//12
	printf("%s12) Check the mysql.user tables to make sure the PROCESS, FILE, SUPER and SHUTDOWN permissions are only "
			"granted to the admin account: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT User, Process_priv, File_priv, Super_priv, Shutdown_priv "
			"from mysql.user "
			"where (Process_priv = 'Y' OR File_priv = 'Y' OR Super_priv = 'Y' OR Shutdown_priv = 'Y') and account_locked = 'N';");
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
	}else{
		result = mysql_store_result(conn);
		numRows = mysql_num_rows(result);
		MYSQL_ROW row;
		printf("\nUsers found: %s%d\n", C_RED, numRows);
		printf("%s\n", C_RED);
		while ((row = mysql_fetch_row(result))){
			printf("%s", (row[0])?(row[0]):("NULL"));
			printf("\n");
		}
	}
	mysql_free_result(result);
	mysql_close(conn);
	printf("%s\n\n", C_DEFAULT);
	return EXIT_SUCCESS;
}

void find_file(char *path, char *filename){
	DIR *dir = opendir (path);
	if (dir){
		struct dirent *dp;
		while ((dp = readdir(dir)) != NULL){
			if (dp->d_name[0] != '.'){
				char *fullpath = malloc (strlen (path) + strlen (dp->d_name) + 2);
				strcpy (fullpath, path);
				strcat (fullpath, "/");
				strcat (fullpath, dp->d_name);
				if (dp->d_type == DT_DIR){
					find_file(fullpath, filename);
				}
				else
					if(strstr(fullpath, filename)!=NULL){
						printf ("%s\nFile found: %s\n", C_RED, fullpath);
					}
				free (fullpath);
			}
		}
		closedir(dir);
	}
	else
		fprintf (stderr, "Can't open dir %s: %s", path, strerror (errno));
	return;
}
