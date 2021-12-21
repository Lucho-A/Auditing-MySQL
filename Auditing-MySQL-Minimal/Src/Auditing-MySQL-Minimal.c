/*
 ============================================================================
 Name        : Auditing-MySql-Minimal.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
 */

#include <Auditing-MySQL-Minimal.h>

int main(void) {
	show_intro();
	MYSQL *conn=NULL;
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	char cmd[128]="", query[1024]="";
	int numRows =0, fileFounds=0;
	printf("%sConnecting to MYSQL: ", C_WHITE);
	(perform_MySQL_connection(&conn))?(printf("%sOK\n",C_GREEN)):(exit(EXIT_FAILURE));
	printf("%s\n*) MySQL client version: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_client_info());
	printf("%s\n*) MySQL host info: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_host_info(conn));
	printf("%s\n*) MySQL server info: %s%s\n\n",C_WHITE, C_RESULTS, mysql_get_server_info(conn));
	//*****************************************************
	printf("%s\n*) Server process: \n",C_WHITE);
	system_call("ps aux | grep -i [m]ysqld");
	//*****************************************************
	printf("%s\n*) MYSQL directory: \n",C_WHITE);
	snprintf(cmd, sizeof(cmd), "ls -la %s", MYSQL_DIR);
	system_call(cmd);
	//*****************************************************
	printf("%s\n*) 'my.cnf' files in /home directory: \n",C_WHITE);
	fileFounds=find_file(HOME_DIR,"my.cnf");
	printf("\n%sFile founds: %d\n\n",C_RESULTS,fileFounds);
	//*****************************************************
	printf("%s\n*) Users with empty password: \n",C_WHITE);
	snprintf(query, sizeof(query),
			"SELECT user, host FROM mysql.user "
			"WHERE authentication_string ='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("\n%sUsers found: %d\n\n", C_RESULTS, numRows);
	while ((row = mysql_fetch_row(result))) printf("%s (%s)\n", (row[0])?(row[0]):("NULL"), (row[1])?(row[1]):("NULL"));
	//*****************************************************
	printf("%s\n*) Existence of anonymous/empty users: \n",C_WHITE);
	snprintf(query, sizeof(query),
			"SELECT user, host "
			"FROM mysql.user "
			"WHERE user='' and account_locked = 'N';");
	numRows=perform_mySQL_query(conn, &result, query);
	(numRows>0)?(printf("\n%sAnonymous users found\n", C_RESULTS)):(printf("\n%sNo Anonymous users found\n", C_RESULTS));
	//*****************************************************
	printf("%s\n*) Login hack: \n\n",C_WHITE);
	int contUsersFound=	perform_mySQL_brute_force();
	(contUsersFound==0)?(printf("\n\n%sNo users/passwords hacked\n", C_RESULTS)):(printf("\n%sUsers & passwords hacked: \n", C_RESULTS));
	//*****************************************************
	printf("%s\n*) 'Max_user_connections' by global and by user: \n",C_WHITE);
	snprintf(query, sizeof(query),
			"SELECT VARIABLE_NAME, VARIABLE_VALUE "
			"FROM performance_schema.global_variables "
			"WHERE VARIABLE_NAME LIKE 'max_user_connections' ;");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RESULTS);
	while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	//*****************************************************
	printf("%s\n*) 'Audit Log' plugin status: \n",C_WHITE);
	snprintf(query, sizeof(query),
			"SELECT PLUGIN_NAME, plugin_status "
			"FROM INFORMATION_SCHEMA.PLUGINS "
			"WHERE PLUGIN_NAME LIKE 'audit_log';");
	numRows=perform_mySQL_query(conn, &result, query);
	printf("%s\n", C_RESULTS);
	while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	if(numRows==0){
		printf("%sPlugin not installed.\n",C_YEL);
	}else{
		printf("%s\n*) 'Audit Log' encrypted?: \n",C_WHITE);
		snprintf(query, sizeof(query),
				"SELECT VARIABLE_NAME, VARIABLE_VALUE "
				"FROM performance_schema.global_variables "
				"WHERE VARIABLE_NAME LIKE 'audit_log_encryption';");
		numRows=perform_mySQL_query(conn, &result, query);
		printf("%s\n", C_RESULTS);
		while ((row = mysql_fetch_row(result))) printf("(Global) %s: %s\n\n", (row[0])?(row[0]):("NULL"),(row[1])?(row[1]):("NULL"));
	}
	mysql_free_result(result);
	mysql_close(conn);
	printf("%s\n\n", C_DEFAULT);
	return EXIT_SUCCESS;
}

