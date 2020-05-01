#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#define SPACES_TO_CMDLINE 20
#define MAX_PID_NAME_LENGTH 6 // 32768 (plus slash)
#define PROC_LENGTH 6 // /proc + space for null terminator
#define CMDLINE_LENGTH 8
#define STAT_LENGTH 5
#define MAX_CMDLINE_LENGTH 70 // Just the max I want to print so it looks good :D
#define MAX_PROC_NAME 17 // comm[16] in kernel + null terminator

struct ProcessInfo {
	int pid;
	char name[MAX_PROC_NAME]; 
	char cmd_line[MAX_CMDLINE_LENGTH];
} ;

int main(int argc, char *argv[]){
	struct dirent *de;
	DIR *dr = opendir("/proc");
	int pid;
	
	printf("PID\tNAME%-20sCMDLINE\n", " ");

	if (dr == NULL){
		perror("Couldn't open dir");
		return 1;
	}

	while ((de = readdir(dr)) != NULL){
		pid = atoi(de->d_name);
		if(pid != 0){
			print_line(pid);
		}
	}
}

int print_line(int pid){
	struct ProcessInfo p;
	p.pid = pid;
	get_name(&p);
	get_cmdline(&p);

	if (strcmp(p.cmd_line, "") == 0){
		printf("%d\t[%s]\n", pid, p.name); // supposedly a kernel thread...
	} else {
		printf("%d\t%-20s\t%s\n", pid, p.name, p.cmd_line);
	}
}

int get_cmdline(struct ProcessInfo *p){
	int fd, len;
	char path[PROC_LENGTH + MAX_PID_NAME_LENGTH + CMDLINE_LENGTH];

	sprintf(path, "/proc/%d/cmdline\0", p->pid);

	fd = open(path, O_RDONLY);
	if (fd == -1){
		perror("Couldn't open file");
		return 1;
	}

	len = read(fd, p->cmd_line, MAX_CMDLINE_LENGTH - 1);
	if (len == -1){
		perror("Coudln't open cmdline");
		return 1;
	}
	
	p->cmd_line[len] = 0;

	return 0;	
}

int get_name(struct ProcessInfo *p){
	int fd;
	char buffer[255];
	char path[PROC_LENGTH + MAX_PID_NAME_LENGTH + STAT_LENGTH];

	sprintf(path, "/proc/%d/stat\0", p->pid);

	fd = open(path, O_RDONLY);
	if (fd == -1){
		perror("Couldn't open stat");
		return 1;
	}

	if (read(fd, buffer, 255) == -1){
		perror("Coudln't read from stat");
		return 1;
	}

	char *start_index = strchr(buffer, '(');
	char *end_index = strrchr(buffer, ')');

	if (start_index == NULL || end_index == NULL){
		printf("Couldn't parse stat correctly '%s'\n", buffer);
		return;
	}

	memset(end_index, 0, 1);

	strncpy(p->name, start_index+1, MAX_PROC_NAME);
	
	return 0;
}