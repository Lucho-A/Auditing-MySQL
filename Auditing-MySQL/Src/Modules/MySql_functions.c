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
		printf("%sError (%s)\n\n",  C_RED, mysql_error(conn));
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

int mysql_brute_force(void){
	double totalComb=0, cont=0, contUsersFound=0;
	int i=0;
	FILE *f=NULL;
	int totalUsernames=0;
	if((totalUsernames=open_file("usernames_MySQL.txt",&f))==-1) return RETURN_ERROR;
	char **usernames = (char**)malloc(totalUsernames * sizeof(char*));
	for(i=0;i<totalUsernames;i++) usernames[i] = (char*)malloc(50 * sizeof(char));
	i=0;
	while(fscanf(f,"%s", usernames[i])!=EOF) i++;
	int totalPasswords=0;
	if((totalPasswords=open_file("passwords_MySQL.txt",&f))==-1) return RETURN_ERROR;
	char **passwords = (char**)malloc(totalPasswords * sizeof(char*));
	for(i=0;i<totalPasswords;i++) passwords[i] = (char*)malloc(50 * sizeof(char));
	i=0;
	while(fscanf(f,"%s", passwords[i])!=EOF) i++;
	totalComb=totalUsernames*totalPasswords;
	MYSQL *conn=NULL;
	for(i=0;i<totalUsernames;i++){
		for(int j=0;j<totalPasswords;j++,cont++){
			if(conn==NULL) conn=mysql_init(NULL);
			if(conn==NULL){
				printf("%sError (%s)\n\n",  C_RED, strerror(errno));
				return RETURN_ERROR;
			}
			printf("\rPercentaje completed: %s%.4lf%% (%s/%s)%s               ",C_GREEN,(double)((cont/totalComb)*100.0),usernames[i], passwords[j], C_WHITE);
			fflush(stdout);
			usleep(BRUTE_FORCE_DELAY);
			if(mysql_real_connect(conn, SERVER_IP, usernames[i], passwords[j], DB, SERVER_PORT, NULL, 0) != NULL){
				printf("%sError (%s)\n\n",  C_RED, strerror(errno));
				printf("%s",C_RED);
				printf("\n\nLoging successfull with user: %s, password: %s. Service Vulnerable\n\n",usernames[i], passwords[j]);
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
