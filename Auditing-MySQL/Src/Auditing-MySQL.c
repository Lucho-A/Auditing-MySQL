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
	int numRows =0, fileFounds=0;
	printf("%sConnecting to MYSQL: ", C_WHITE);
	(perform_MySQL_connection(&conn))?(printf("%sOK\n",C_GREEN)):(exit(EXIT_FAILURE));
//
	printf("%s\n*) Identify the MySQL client version: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_client_info());
//
	printf("%s\n*) Identify the MySQL host info: %s%s\n\n",C_WHITE, C_GREEN, mysql_get_host_info(conn));
//
	printf("%s\n*) Identify the MySQL server info: %s%s\n\n",C_WHITE, C_RED, mysql_get_server_info(conn));
//
	printf("%s\n*) Identify the user running the server: \n",C_WHITE);
	system_call("ps aux | grep -i [m]ysqld");
//
	printf("%s\n*) Identify the owner of the MYSQL directory: \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_DIR);
	system_call(cmd);
//
	printf("%s\n*) Identify the owner of the MYSQL config directory: \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_CONFIG_DIR);
	system_call(cmd);
//
	printf("%s\n*) Evaluate the existence of 'my.cnf' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"my.cnf");
	printf("\nFile founds: %s%d\n\n",(fileFounds==0)?(C_GREEN):(C_RED),fileFounds);
//
	printf("%s\n*) Evaluate the existence of 'mysql_history' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"mysql_history");
	printf("\nFile founds: %s%d\n\n",(fileFounds==0)?(C_GREEN):(C_RED),fileFounds);
//
	printf("%s\n*) Evaluate the existence of 'bash_history' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"bash_history");
	printf("\nFile founds: %s%d\n\n",(fileFounds==0)?(C_GREEN):(C_RED),fileFounds);
//
	printf("%s\n*) Evaluate the existence of users without requiring a password to connect to MySQL: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE authentication_string ='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n\n", C_RED, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) Evaluate the existence of 'root' user: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='root' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sRoot user found\n", C_RED)):(printf("\n%sNo Root user found\n", C_GREEN));
//
	printf("%s\n*) Evaluate the existence of anonymous/empty users: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sAnonymous users found\n", C_RED)):(printf("\n%sNo Anonymous users found\n", C_GREEN));
//
	printf("%s\n*) Evaluate the users with privileges: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user "
			"WHERE (Select_priv = 'Y' OR Insert_priv = 'Y' OR Update_priv = 'Y' "
			"OR Delete_priv = 'Y' OR Create_priv = 'Y' OR Drop_priv = 'Y' OR Grant_priv = 'Y' "
			"OR Index_priv = 'Y' OR Alter_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n\n", C_RED, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) Evaluate users with 'process', 'file', 'super' and 'shutdown' permissions: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT User, Process_priv, File_priv, Super_priv, Shutdown_priv "
			"FROM mysql.user "
			"WHERE (Process_priv = 'Y' OR File_priv = 'Y' OR Super_priv = 'Y' "
			"OR Shutdown_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\nUsers found: %s%d\n", C_RED, numRows);
	printf("%s\n", C_RED);
	while ((row = mysql_fetch_row(result))) printf("%s\n", (row[0])?(row[0]):("NULL"));
//
	printf("%s\n*) Evaluate validate_password component: \n",C_WHITE);
	snprintf(query, sizeof(query), "SHOW VARIABLES LIKE 'validate_password.%';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RED);
	(numRows==0)?(printf("\n%sValidate_password component not found\n", C_RED)):(printf("\n%sValidate_password component found\n", C_GREEN));
	while ((row = mysql_fetch_row(result))) printf("%s\n", (row[0])?(row[0]):("NULL"));
//
	printf("%s\n*) Evaluate relevant variables: \n",C_WHITE);
	snprintf(query, sizeof(query), "SHOW VARIABLES where Variable_name like 'default_authentication_plugin' "
			"or Variable_name like 'default_password_lifetime' "
			"or Variable_name like 'password_history' "
			"or Variable_name like 'admin_tls_version';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RED);
	while ((row = mysql_fetch_row(result))) printf("%s: %s\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) Attemping perform login by using BFA: \n\n",C_WHITE);
	int contUsersFound=	mysql_brute_force();
	(contUsersFound==0)?(printf("\n\n%sNo users/passwords hacked\n", C_GREEN)):(printf("\n%sUsers & passwords hacked: \n", C_RED));
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
