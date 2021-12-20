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

int main(void) {
	show_intro();
	MYSQL *conn=NULL;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	char cmd[128]="", query[1024]="";
	int numRows =0, fileFounds=0;
	printf("%sConnecting to MYSQL: ", C_WHITE);
	(perform_MySQL_connection(&conn))?(printf("%sOK\n",C_GREEN)):(exit(EXIT_FAILURE));
//
	printf("%s\n*) MySQL client version: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_client_info());
//
	printf("%s\n*) MySQL host info: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_host_info(conn));
//
	printf("%s\n*) MySQL server info: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_server_info(conn));
//
	printf("%s\n*) Server process: \n",C_WHITE);
	system_call("ps aux | grep -i [m]ysqld");
//
	printf("%s\n*) MYSQL directory: \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_DIR);
	system_call(cmd);
//
	printf("%s\n*) MYSQL config directory: \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_CONFIG_DIR);
	system_call(cmd);
//
	printf("%s\n*) 'my.cnf' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"my.cnf");
	printf("\n%sFile founds: %d\n\n",C_RESULTS,fileFounds);
//
	printf("%s\n*) 'mysql_history' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"mysql_history");
	printf("\n%sFile founds: %d\n\n",C_RESULTS,fileFounds);
//
	printf("%s\n*) 'bash_history' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"bash_history");
	printf("\n%sFile founds: %d\n\n",C_RESULTS,fileFounds);
//
	printf("%s\n*) Users with empty password: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user "
			"WHERE authentication_string ='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\n%sUsers found: %d\n\n", C_RESULTS, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) Existence of 'root' user: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='root' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sRoot user found\n", C_RESULTS)):(printf("\n%sNo Root user found\n", C_RESULTS));
//
	printf("%s\n*) Existence of anonymous/empty users: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user WHERE user='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sAnonymous users found\n", C_RESULTS)):(printf("\n%sNo Anonymous users found\n", C_RESULTS));
//
	printf("%s\n*) Users with privileges: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT user, host FROM mysql.user "
			"WHERE (Select_priv = 'Y' OR Insert_priv = 'Y' OR Update_priv = 'Y' "
			"OR Delete_priv = 'Y' OR Create_priv = 'Y' OR Drop_priv = 'Y' OR Grant_priv = 'Y' "
			"OR Index_priv = 'Y' OR Alter_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\n%sUsers found: %d\n\n", C_RESULTS, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) Users with 'process', 'file', 'super' and 'shutdown' permissions: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT User, Process_priv, File_priv, Super_priv, Shutdown_priv "
			"FROM mysql.user "
			"WHERE (Process_priv = 'Y' OR File_priv = 'Y' OR Super_priv = 'Y' "
			"OR Shutdown_priv = 'Y') and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\n%sUsers found: %d\n\n", C_RESULTS, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s\n", (row[0])?(row[0]):("NULL"));
//
	printf("%s\n*) 'Validate_password' component: \n",C_WHITE);
	snprintf(query, sizeof(query), "SHOW VARIABLES LIKE 'validate_password.%%';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows==0)?(printf("\n%sValidate_password component not found\n", C_RESULTS)):(printf("\n%sValidate_password component found\n", C_RESULTS));
	while ((row = mysql_fetch_row(result))) printf("%s\n", (row[0])?(row[0]):("NULL"));
//
	printf("%s\n*) Relevant variables: \n",C_WHITE);
	snprintf(query, sizeof(query), "SHOW VARIABLES where Variable_name like 'default_authentication_plugin' "
			"or Variable_name like 'default_password_lifetime' "
			"or Variable_name like 'password_history' "
			"or Variable_name like 'admin_tls_version';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RESULTS);
	while ((row = mysql_fetch_row(result))) printf("%s: %s\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
//
	printf("%s\n*) BFA Login hack: \n\n",C_WHITE);
	int contUsersFound=	perform_mySQL_brute_force();
	(contUsersFound==0)?(printf("\n\n%sNo users/passwords hacked\n", C_RESULTS)):(printf("\n%sUsers & passwords hacked: \n", C_RESULTS));
//
	printf("%s\n*) 'Max_user_connections' by global and by user: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT VARIABLE_NAME, VARIABLE_VALUE "
			"FROM performance_schema.global_variables "
			"WHERE VARIABLE_NAME LIKE 'max_user_connections' ;");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RESULTS);
	while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	snprintf(query, sizeof(query), "SELECT user, host, max_user_connections "
			"FROM mysql.user;");
	numRows=perform_mySQL_query(conn, &result, query);
	while ((row = mysql_fetch_row(result))) printf("%s: %s: %s\n", (row[0])?(row[0]):("NULL")
			,(row[1])?(row[1]):("NULL")
			,(row[2])?(row[2]):("NULL"));
//
	printf("%s\n*) 'Audit Log' plugin status: \n",C_WHITE);
	snprintf(query, sizeof(query), "SELECT PLUGIN_NAME, plugin_status FROM INFORMATION_SCHEMA.PLUGINS "
			"WHERE PLUGIN_NAME LIKE 'audit_log';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RESULTS);
	while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	if(numRows==0){
		printf("%sPlugin not installed.\n",C_YEL);
	}else{
		printf("%s\n*) 'Audit Log' encrypted?: \n",C_WHITE);
		snprintf(query, sizeof(query), "SELECT VARIABLE_NAME, VARIABLE_VALUE "
				"FROM performance_schema.global_variables "
				"WHERE VARIABLE_NAME LIKE 'audit_log_encryption';");
		numRows=perform_mySQL_query(conn, &result, query);
		printf("%s\n", C_RESULTS);
		while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	}
//

	//TODO
	mysql_free_result(result);
	mysql_close(conn);
	printf("%s\n\n", C_DEFAULT);
	return EXIT_SUCCESS;
}

