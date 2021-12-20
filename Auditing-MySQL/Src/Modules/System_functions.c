/*
 ============================================================================
 Name        : System_functions.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
*/

#include "Auditing-MySQL.h"

void system_call(char *cmd){
	printf("%s\n",C_RESULTS);
	system(cmd);
	printf("%s\n", C_WHITE);
	return;
}
