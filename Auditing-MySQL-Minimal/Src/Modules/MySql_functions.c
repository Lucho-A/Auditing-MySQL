/*
 ============================================================================
 Name        : MySQL_functions.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
 */

#include <Auditing-MySQL-Minimal.h>

int perform_MySQL_connection(MYSQL **conn){
	*conn = mysql_init(NULL);
	if(*conn == NULL){
		printf("%sMySQL error (%s)\n\n",  C_HRED, mysql_error(*conn));
		return RETURN_ERROR;
	}
	if(mysql_real_connect(*conn, SERVER_IP, USER, PASS, DB, SERVER_PORT, NULL, 0) == NULL){
		printf("%sMySQL error (%s)\n\n",  C_HRED, mysql_error(*conn));
		mysql_close(*conn);
		return RETURN_ERROR;
	}else{
		return RETURN_OK;
	}
}

int perform_mySQL_query(MYSQL *conn, MYSQL_RES **result, char *query){
	if (mysql_query(conn, query)){
		show_error("", errno);
		printf("%sMySQL error (%s)\n\n",  C_HRED, mysql_error(conn));
		mysql_close(conn);
		return RETURN_ERROR;
	}else{
		*result = mysql_store_result(conn);
		return mysql_num_rows(*result);
	}
}

int perform_mySQL_brute_force(void){
	double totalComb=0.0;
	int cont=1, contUsersFound=0;
	char usernames[USERNAMES][128]={"root",
							"mysql",
							"admin"};
	char passwords[PASSWORDS][128]={"pass1",
							"pass2",
							"pass3"};
	totalComb=USERNAMES*PASSWORDS;
	MYSQL *conn=NULL;
	for(int i=0;i<USERNAMES;i++){
		for(int j=0;j<PASSWORDS;j++,cont++){
			if(conn==NULL) conn=mysql_init(NULL);
			if(conn==NULL){
				show_error("", errno);
				return RETURN_ERROR;
			}
			printf("\r%sPercentage completed: %s%.4lf%% (%s/%s)               ",C_WHITE,C_GREEN, (double)((cont/totalComb)*100.0),usernames[i], passwords[j]);
			fflush(stdout);
			usleep(BRUTE_FORCE_DELAY);
			if(mysql_real_connect(conn, SERVER_IP, usernames[i], passwords[j], DB, SERVER_PORT, NULL, 0) != NULL){
				show_error("", errno);
				printf("%s",C_RED);
				printf("\n\nLoging successfull with user: %s, password: %s.\n\n",usernames[i], passwords[j]);
				mysql_close(conn);
				conn=NULL;
				contUsersFound++;
			}
		}
	}
	mysql_close(conn);
	printf("%s",C_DEFAULT);
	return contUsersFound;
}
