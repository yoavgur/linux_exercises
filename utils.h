#define PRINT_ERROR printf("Error at %s:%d\n", __FILE__, __LINE__)

#define CHECK(x) do { if(!(x)){ PRINT_ERROR; return ERROR; } } while(0)
#define CHECK_GOTO_CLEANUP(x) do { if(!(x)){ PRINT_ERROR; err = ERROR; goto cleanup; } } while(0)
#define CHECK_NO_RETURN(x) do { if(!(x)){ PRINT_ERROR; } } while(0)

typedef enum {   // Not a lot of options so far :D
	NO_ERROR,
	ERROR
} ErrorCode;