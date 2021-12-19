/*
 ============================================================================
 Name        : Files_functions.c
 Author      : L.
 Version     : 0.0.1
 Copyright   : GPL 3.0
 Description :
 ============================================================================
*/

#include "Auditing-MySQL.h"

void find_file(char *path, char *filename){
	DIR *dir = opendir (path);
	char cmd[128]="";
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
						printf("%s\nFile found: %s\n", C_RED, fullpath);
						snprintf(cmd, sizeof(cmd), "ls -la %s", fullpath);
						system_call(cmd);
					}
				free (fullpath);
			}
		}
		closedir(dir);
	}
	else
		printf("Error %d (%s)\n", errno, strerror (errno));
	return;
}
