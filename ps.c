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

#define MAX_CMDLINE_LENGTH 70 // Just the max I want to print so it looks good :D
#define MAX_PROC_NAME 17 // comm[16] in kernel + null terminator

#define PRINT_ERROR do { printf("Error at %s:%d\n", __FILE__, __LINE__);return 1; } while (0)
#define CHECK_VALUE(x,y) do { if(x != y){PRINT_ERROR;} } while(0)
#define CHECK_NOT_VALUE(x,y) do { if(x == y){PRINT_ERROR;} } while(0)
#define CHECK(x) CHECK_VALUE(x,0)

struct ProcessInfo {
	int pid;
	char name[MAX_PROC_NAME]; 
	char cmd_line[MAX_CMDLINE_LENGTH];
} ;

int print_line(int pid);
int get_cmdline(struct ProcessInfo *p);
int get_name(struct ProcessInfo *p);

int main(){
	struct dirent *de = {0};
	DIR *dr;
	int pid;

	dr = opendir("/proc");
	CHECK_NOT_VALUE(dr, NULL);
	
	printf("PID\tNAME%-20sCMDLINE\n", " ");

	while ((de = readdir(dr)) != NULL){
		pid = atoi(de->d_name); 
		if(pid != 0){ // Make sure this is a process folder and not unrelated file in /proc
			print_line(pid);
		}
	}

	return 0;
}

int print_line(int pid){
	struct ProcessInfo p = {.pid = pid, .name = {0}, .cmd_line = {0}};

	CHECK(get_name(&p));
	CHECK(get_cmdline(&p));

	if (p.cmd_line[0] == 0){
		printf("%d\t[%s]\n", pid, p.name); // supposedly a kernel thread...
	} else {
		printf("%d\t%-20s\t%s\n", pid, p.name, p.cmd_line);
	}

	return 0;
}

int get_cmdline(struct ProcessInfo *p){
	int fd;
	int len;
	char path[PATH_MAX] = {0};

	snprintf(path, PATH_MAX, "/proc/%d/cmdline", p->pid);

	fd = open(path, O_RDONLY);
	CHECK_NOT_VALUE(fd, -1);

	len = read(fd, p->cmd_line, MAX_CMDLINE_LENGTH - 1);
	CHECK_NOT_VALUE(len, -1);
	
	return 0;	
}

int get_name(struct ProcessInfo *p){
	int fd;
	char buffer[255] = {0};
	char path[PATH_MAX] = {0};

	snprintf(path, PATH_MAX, "/proc/%d/stat", p->pid);

	fd = open(path, O_RDONLY);
	CHECK_NOT_VALUE(fd, -1);

	CHECK_NOT_VALUE(read(fd, buffer, sizeof(buffer)), -1);

	char *start_index = strchr(buffer, '(');
	char *end_index = strrchr(buffer, ')');

	CHECK_NOT_VALUE(start_index, NULL);
	CHECK_NOT_VALUE(end_index, NULL);

	end_index[0] = 0;

	strncpy(p->name, start_index+1, MAX_PROC_NAME);
	
	return 0;
}