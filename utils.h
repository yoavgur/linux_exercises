#define ERROR_OUTPUT stderr

#define PRINT_ERROR(msg) fprintf(ERROR_OUTPUT, "%s - %s:%d\n", msg, __FILE__, __LINE__)

#define CHECK(x) do { \
				   if(!(x)){ \
				     PRINT_ERROR("An error occurred"); \
					 return ERROR; \
				   }\
				 } while(0)

#define CHECK_MSG(x, msg) do { \
							if(!(x)){ \
							  PRINT_ERROR(msg); \
							  return ERROR; \
						    }\
						  } while(0)

#define CHECK_GOTO_CLEANUP(x) do { \
							    if(!(x)){ \
								  PRINT_ERROR("An error occurred"); \
								  err = ERROR; \
								  goto cleanup; \
								}\
							  } while(0)

#define CHECK_GOTO_CLEANUP_MSG(x, msg) do { \
							         if(!(x)){ \
								       PRINT_ERROR(msg); \
								       err = ERROR; \
								       goto cleanup; \
								     }\
							       } while(0)

#define WARN(x) do { \
				  if(!(x)){ \
					PRINT_ERROR("An error occurred"); \
					err = ERROR;\
				  }\
				} while(0)

#define WARN_MSG(x, msg) do { \
				  if(!(x)){ \
					PRINT_ERROR(msg); \
					err = ERROR;\
				  }\
				} while(0)

#define PRINT_IF_BIT_IN_MASK(mask, bit, s) do { \
										     if(mask & bit){ \
											   printf("%s", s); \
											 } \
										   } while(0)

typedef enum {   // Not a lot of options so far :D
	NO_ERROR,
	ERROR
} ErrorCode;