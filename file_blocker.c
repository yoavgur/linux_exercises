#include <sys/fanotify.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

ErrorCode block_file(char *file);

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: file_blocker <FILE_TO_MONITOR>\n");
        return 1;
    }

    CHECK(block_file(argv[1]) == NO_ERROR);
    
    return 0;
}

ErrorCode block_file(char *file){
    ErrorCode err = NO_ERROR;
    int fan_fd = -1;
    struct fanotify_event_metadata event = {0};
    struct fanotify_response response = {0};

    printf("Blocking access to '%s'\n", file);

    fan_fd = fanotify_init(FAN_CLASS_PRE_CONTENT, O_RDONLY);
    CHECK_GOTO_CLEANUP_MSG(fan_fd != -1, "Failed to initialize a fanotify group");
    
    CHECK_GOTO_CLEANUP_MSG(fanotify_mark(fan_fd, FAN_MARK_ADD, FAN_ALL_EVENTS | FAN_ALL_PERM_EVENTS, 0, file) == 0, "Couldn't add fanotify mark");

    while (1) { 
        CHECK_GOTO_CLEANUP_MSG(read(fan_fd, &event, sizeof(event)) == sizeof(event), "Couldn't read from fanotify file descriptor");
        
        CHECK_GOTO_CLEANUP_MSG(event.vers == FANOTIFY_METADATA_VERSION, "Program compiled for different fanotify version");
        CHECK_GOTO_CLEANUP_MSG(event.fd != -1, "Received bad fd from fanotify event");

        PRINT_IF_BIT_IN_MASK(event.mask, FAN_ACCESS_PERM, "Attempted access to file\n");
        PRINT_IF_BIT_IN_MASK(event.mask, FAN_OPEN_EXEC_PERM, "Attempted execution of file\n");
        PRINT_IF_BIT_IN_MASK(event.mask, FAN_OPEN_PERM, "Attempted opening file\n");
        
        if (event.mask & FAN_ALL_PERM_EVENTS){
            response.fd = event.fd;
            response.response = FAN_DENY;

            CHECK_GOTO_CLEANUP_MSG(write(fan_fd, &response, sizeof(response)) == sizeof(response), "Couldn't write whole response to fanotify fd");
            printf("Blocked file action\n");
        }

        WARN_MSG(close(event.fd) != -1, "Couldn't close event file descriptor");
    }

cleanup:
    WARN_MSG(close(fan_fd) != -1, "Couldn't close fanotify file descriptor");

    return err;
}