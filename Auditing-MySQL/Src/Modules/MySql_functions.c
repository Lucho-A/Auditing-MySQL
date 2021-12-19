/*
 ============================================================================
 Name        : MySQL_functions.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
 */

#include "Auditing-MySQL.h"

int perform_mySQL_query(MYSQL *conn, MYSQL_RES **result, char *query){
	if (mysql_query(conn, query)){
		printf("%d %s\n", errno, strerror(errno));
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(conn));
		mysql_close(conn);
		return 0;
	}else{
		*result = mysql_store_result(conn);
		return mysql_num_rows(*result);
	}
}

int perform_MySQL_connection(MYSQL **conn){
	*conn = mysql_init(NULL);
	if(*conn == NULL){
		printf("Error (%s)\n", mysql_error(*conn));
		return RETURN_ERROR;
	}
	if(mysql_real_connect(*conn, SERVER_IP, USER, PASS, DB, SERVER_PORT, NULL, 0) == NULL){
		fprintf(stderr, "\n%s%s\n\n",  C_RED, mysql_error(*conn));
		mysql_close(*conn);
		return RETURN_ERROR;
	}else{
		return RETURN_OK;
	}
}
