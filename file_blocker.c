#include <sys/fanotify.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

ErrorCode block_file(const char *file);
ErrorCode send_response(int fd, int fan_fd);
ErrorCode handle_events(int fan_fd);
void print_mask_bits(uint64_t mask);

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("Usage: file_blocker <FILE_TO_BLOCK>\n");
        return 1;
    }

    CHECK(block_file(argv[1]) == NO_ERROR);
    
    return 0;
}

ErrorCode block_file(const char *file){
    ErrorCode err = NO_ERROR;
    int fan_fd = -1;
    
    printf("Blocking access to '%s'\n", file);

    fan_fd = fanotify_init(FAN_CLASS_PRE_CONTENT, O_RDONLY);
    CHECK_GOTO_CLEANUP_MSG(fan_fd != -1, "Failed to initialize a fanotify group");
    
    CHECK_GOTO_CLEANUP_MSG(fanotify_mark(fan_fd, FAN_MARK_ADD, FAN_ALL_EVENTS | FAN_ALL_PERM_EVENTS, 0, file) == 0, "Couldn't add fanotify mark");

    CHECK_GOTO_CLEANUP_MSG(handle_events(fan_fd) == NO_ERROR, "Handle events failed");

cleanup:
    WARN_MSG(close(fan_fd) != -1, "Couldn't close fanotify file descriptor");

    return err;
}

ErrorCode handle_events(int fan_fd){
    ErrorCode err = NO_ERROR;
    struct fanotify_event_metadata event = {0};

    while (1) { 
        CHECK_GOTO_CLEANUP_MSG(read(fan_fd, &event, sizeof(event)) == sizeof(event), "Couldn't read from fanotify file descriptor");
        
        CHECK_GOTO_CLEANUP_MSG(event.vers == FANOTIFY_METADATA_VERSION, "Program compiled for different fanotify version");
        CHECK_GOTO_CLEANUP_MSG(event.fd != -1, "Received bad fd from fanotify event");

        print_mask_bits(event.mask);
        
        if (event.mask & FAN_ALL_PERM_EVENTS){
            CHECK_GOTO_CLEANUP_MSG(send_response(event.fd, fan_fd) == NO_ERROR, "Sending response failed");
        }

        WARN_MSG(close(event.fd) != -1, "Couldn't close event file descriptor");
    }

cleanup:
    return err;
}

ErrorCode send_response(int fd, int fan_fd){
    ErrorCode err = NO_ERROR;
    struct fanotify_response response = {0};

    response.fd = fd;
    response.response = FAN_DENY;

    CHECK_GOTO_CLEANUP_MSG(write(fan_fd, &response, sizeof(response)) == sizeof(response), "Couldn't write whole response to fanotify fd");
    printf("Blocked file action\n");

cleanup:
    return err;
}

void print_mask_bits(uint64_t mask){
    PRINT_IF_BIT_IN_MASK(mask, FAN_ACCESS_PERM, "Attempted access to file\n");
    PRINT_IF_BIT_IN_MASK(mask, FAN_OPEN_EXEC_PERM, "Attempted execution of file\n");
    PRINT_IF_BIT_IN_MASK(mask, FAN_OPEN_PERM, "Attempted opening file\n");
}
