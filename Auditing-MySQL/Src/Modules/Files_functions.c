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

int find_file(char *path, char *filename){
	int fileFounds=0;
	find_file_rec(path, filename, &fileFounds);
	return fileFounds;
}

int find_file_rec(char *path, char *filename, int *fileFounds){
	DIR *dir = opendir(path);
	char cmd[128]="";
	if (dir){
		struct dirent *dp;
		while ((dp = readdir(dir)) != NULL){
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
				char *fullpath = malloc (strlen (path) + strlen (dp->d_name) + 2);
				strcpy (fullpath, path);
				strcat (fullpath, "/");
				strcat (fullpath, dp->d_name);
				if (dp->d_type == DT_DIR){
					find_file_rec(fullpath, filename, fileFounds);
				}else{
					if(strstr(fullpath, filename)!=NULL){
						printf("%s\nFile found: %s\n", C_RED, fullpath);
						snprintf(cmd, sizeof(cmd), "ls -la %s", fullpath);
						system_call(cmd);
						(*fileFounds)++;
					}
				}
				free (fullpath);
			}
		}
		closedir(dir);
	}
	else
		printf("Error %d (%s)\n", errno, strerror (errno));
	return *fileFounds;
}

int open_file(char *fileName, FILE **f){
	char file[256]="";
	snprintf(file,sizeof(file),"%s%s", PATH_TO_RESOURCES,fileName);
	if((*f=fopen(file,"r"))==NULL){
		printf("%s",C_RED);
		printf("fopen(%s) error: Error: %d (%s)\n", fileName, errno, strerror(errno));
		printf("%s",C_DEFAULT);
		return -1;
	}
	int entries=0;
	char buffer[256]="";
	while(fscanf(*f, "%s ", buffer)!=EOF) entries++;
	rewind(*f);
	return entries;
}

