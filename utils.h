#define PRINT_ERROR() printf("Error at %s:%d\n", __FILE__, __LINE__)

#define CHECK(x) do { \
				   if(!(x)){ \
				     PRINT_ERROR(); \
					 return ERROR; \
				   }\
				 } while(0)

#define CHECK_GOTO_CLEANUP(x) do { \
							    if(!(x)){ \
								  PRINT_ERROR(); \
								  err = ERROR; \
								  goto cleanup; \
								}\
							  } while(0)

#define WARN(x) do { \
						     if(!(x)){ \
							   PRINT_ERROR();\
							   err = ERROR;\
						     }\
						   } while(0)

typedef enum {   // Not a lot of options so far :D
	NO_ERROR,
	ERROR
} ErrorCode;