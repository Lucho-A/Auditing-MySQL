/*
 ============================================================================
 Name        : Auditing-MySql.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description : Script for auditing MySQL databases in C, Ansi-style
 ============================================================================
 */

#include "Auditing-MySQL.h"

void intro(void);

int main(void) {
	intro();
	MYSQL *conn=NULL;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	char cmd[128]="", query[1024]="";
	int numRows =0;
	printf("%sConnecting to MYSQL: ", C_WHITE);
	(perform_MySQL_connection(&conn))?(printf("%sOK\n",C_GREEN)):(exit(EXIT_FAILURE));
	printf("%s\n1) Identify the MySQL client version: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_client_info());
	printf("%s\n2) Identify the MySQL host info: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_host_info(conn));
	printf("%s\n3) Identify the MySQL server info: %s%s\n\n",C_WHITE, C_RED, mysql_get_server_info(conn));
	printf("%s\n4) Evaluate running the server as 'mysql' (or another unprivileged user): \n",C_WHITE);
	system_call("ps aux | grep -i mysql");
	printf("%s\n5) Evaluate the owner of the MYSQL directory (mysql.mysql): \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_DIR);
	system_call(cmd);
	printf("%s\n6) Evaluate the owner of the MYSQL config directory (root.root): \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_CONFIG_DIR);
	system_call(cmd);
	printf("%s\n7) Evaluate the existence of 'my.cnf' files in /home directory: \n",C_WHITE);
	find_file(HOME_DIR,"my.cnf");
//8
	printf("%s\n8) Evaluate the existence of users with account not locked, and without requiring a password to connect to MySQL: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE authentication_string ='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n\n", C_RED, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//9
	printf("%s\n9) Evaluate the existence of user named 'root' with account not locked: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='root' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sRoot user found\n", C_RED)):(printf("\n%sNo Root user found\n", C_GREEN));
//10
	printf("%s\n10) Evaluate the existence of anonymous/empty users with account not locked: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sAnonymous users found\n", C_RED)):(printf("\n%sNo Anonymous users found\n", C_GREEN));
//11
	printf("%s\n11) Evaluate the users with account not locked, and with privileges in order to analize their rational: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user "
			"WHERE (Select_priv = 'Y' OR Insert_priv = 'Y' OR Update_priv = 'Y' "
			"OR Delete_priv = 'Y' OR Create_priv = 'Y' OR Drop_priv = 'Y' OR Grant_priv = 'Y' "
			"OR Index_priv = 'Y' OR Alter_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n\n", C_RED, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//12
	printf("%s\n12) Check the mysql.user tables to make sure the PROCESS, FILE, SUPER and SHUTDOWN permissions are only "
			"granted to the admin account: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT User, Process_priv, File_priv, Super_priv, Shutdown_priv "
			"FROM mysql.user "
			"WHERE (Process_priv = 'Y' OR File_priv = 'Y' OR Super_priv = 'Y' "
			"OR Shutdown_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n", C_RED, numRows);
	printf("%s\n", C_RED);
	while ((row = mysql_fetch_row(result))){
		printf("%s", (row[0])?(row[0]):("NULL"));
		printf("\n");
	}
//TODO
	mysql_free_result(result);
	mysql_close(conn);
	printf("%s\n\n", C_DEFAULT);
	return EXIT_SUCCESS;
}

void intro(void){
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
	return;
}
