#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include "utils.h"

#define MAX_CMDLINE_LENGTH 70 // Just the max I want to print so it looks good :D
#define MAX_PROC_NAME 17 // comm[16] in kernel + null terminator

struct ProcessInfo {
	int pid;
	char name[MAX_PROC_NAME]; 
	char cmd_line[MAX_CMDLINE_LENGTH];
} ;


ErrorCode print_line(int pid);
ErrorCode get_cmdline(struct ProcessInfo *p);
ErrorCode get_name(struct ProcessInfo *p);

int main(){
	ErrorCode err = NO_ERROR;
	struct dirent *de = {0};
	DIR *dr = {0};
	int pid = -1;

	dr = opendir("/proc");
	CHECK_GOTO_CLEANUP(dr != NULL);
	
	printf("PID\tNAME%-20sCMDLINE\n", " ");

	while ((de = readdir(dr)) != NULL){
		pid = atoi(de->d_name); 
		if(pid != 0){ // Make sure this is a process folder and not unrelated file in /proc
			CHECK_GOTO_CLEANUP(print_line(pid) == NO_ERROR);
		}
	}

cleanup:
	WARN(closedir(dr) == 0);
	return err;
}

ErrorCode print_line(int pid){
	ErrorCode err = NO_ERROR;
	struct ProcessInfo p = {.pid = pid};

	CHECK_GOTO_CLEANUP(get_name(&p) == NO_ERROR);
	CHECK_GOTO_CLEANUP(get_cmdline(&p) == NO_ERROR);

	if (p.cmd_line[0] == 0){
		printf("%d\t[%s]\n", pid, p.name); // supposedly a kernel thread...
	} else {
		printf("%d\t%-20s\t%s\n", pid, p.name, p.cmd_line);
	}

cleanup:
	return err;
}

ErrorCode get_cmdline(struct ProcessInfo *p){
	ErrorCode err = NO_ERROR;
	int fd = -1;
	int len = -1;
	char path[PATH_MAX] = {0};

	snprintf(path, sizeof(path), "/proc/%d/cmdline", p->pid);

	fd = open(path, O_RDONLY);
	CHECK_GOTO_CLEANUP(fd != -1);

	len = read(fd, p->cmd_line, sizeof(p->cmd_line) - 1);
	CHECK_GOTO_CLEANUP(len != -1);

cleanup:
	WARN(close(fd) == 0);
	
	return err;	
}

ErrorCode get_name(struct ProcessInfo *p){
	ErrorCode err = NO_ERROR;
	int fd = -1;
	char buffer[255] = {0};
	char path[PATH_MAX] = {0};

	snprintf(path, sizeof(path), "/proc/%d/stat", p->pid);

	fd = open(path, O_RDONLY);
	CHECK_GOTO_CLEANUP(fd != -1);

	CHECK_GOTO_CLEANUP(read(fd, buffer, sizeof(buffer)) != -1);

	char *start_index = strchr(buffer, '(');
	char *end_index = strrchr(buffer, ')');

	CHECK_GOTO_CLEANUP(start_index != NULL);
	CHECK_GOTO_CLEANUP(end_index != NULL);

	end_index[0] = '\0';

	strncpy(p->name, start_index+1, sizeof(p->name));

cleanup:
	WARN(close(fd) == 0);

	return err;
}