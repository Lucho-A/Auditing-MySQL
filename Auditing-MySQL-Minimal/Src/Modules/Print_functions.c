/*
 ============================================================================
 Name        : Files_functions.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
 */

#include "Auditing-MySQL-Minimal.h"

void show_intro(void){
	if(getuid()!=0){
		printf("\n%sYou must be root for running the program.\n\n", C_RED);
		exit(EXIT_FAILURE);
	}
	system("clear");
	printf("%s",C_CYAN);
	printf("\n***********************************************************************************************************************************************************");
	printf("\n*");
	printf("\n* Auditing MYSQL by L.");
	printf("\n*");
	printf("\n* v0.0.1");
	printf("\n*");
	printf("\n* This is just a minimal/example version. ");
	printf("\n*");
	printf("\n* For in-deep version, as well, for others systems/plattforms (Cybersecurity, Oracle, AIX, SAP HANA, among others), pls, contact me!");
	printf("\n*");
	printf("\n* Email: luis.alfie@gmail.com");
	printf("\n*");
	printf("\n***********************************************************************************************************************************************************");
	printf("%s",C_WHITE);
	time_t timestamp = time(NULL);
	struct tm tm = *localtime(&timestamp);
	printf("\n\nStarting at: %d/%02d/%02d %02d:%02d:%02d\n\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("%s",C_DEFAULT);
	return;
}

void show_error(char *errMsg, int errnum){
	printf("%s",C_HRED);
	(errnum==0)?(printf("%s\n", errMsg)):(printf("%s Error %d (%s)\n", errMsg, errnum, strerror(errnum)));
	printf("%s",C_DEFAULT);
	return;
}
