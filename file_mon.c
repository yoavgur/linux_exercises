#include <string.h>
#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "utils.h"

ErrorCode monitor_file(const char *file_name);
void print_mask_bits(uint32_t mask);

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: file_mon <FILE_TO_MONITOR>\n");
        return 1;
    }

    CHECK(monitor_file(argv[1]) == NO_ERROR);
    
    return 0;
}

ErrorCode monitor_file(const char *file_name){
    ErrorCode err = NO_ERROR;
    int fd = -1;
    int wd = -1;
    struct inotify_event event = {0};

    fd = inotify_init();
    CHECK_GOTO_CLEANUP(fd != -1);

    wd = inotify_add_watch(fd, file_name, IN_ALL_EVENTS);
    CHECK_GOTO_CLEANUP(wd != -1);

    printf("Monitoring file %s\n", file_name);
    do {
        CHECK_GOTO_CLEANUP(read(fd, &event, sizeof(event)) == sizeof(event));

        print_mask_bits(event.mask);
        
        memset(&event, 0, sizeof(event));
    } while (!(event.mask & IN_IGNORED));
    
    printf("File is no longer available for monitoring\n");

cleanup:
    WARN(close(fd) != -1);
    WARN(close(wd) != -1);

    return err;
}

void print_mask_bits(uint32_t mask){
    PRINT_IF_BIT_IN_MASK(mask, IN_ACCESS, "File has been accessed\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_ATTRIB, "File metadata has been changed\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_CLOSE_WRITE, "File has been closed after write\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_CLOSE_NOWRITE, "File has been closed after read\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_DELETE_SELF, "File has been deleted\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_MODIFY, "File has been modified\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_MOVE_SELF, "File has been moved\n");
    PRINT_IF_BIT_IN_MASK(mask, IN_OPEN, "File has been opened\n");
}