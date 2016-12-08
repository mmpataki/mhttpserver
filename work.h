
#define COMMAND_MAX_LEN	(1024)

struct work
{
    #pragma pack(1)
    /*
     * type of the work & command
     */
	union {
		char	command[5];
		char	data[COMMAND_MAX_LEN];
	};
    
	int	    out;
	uint32_t    ip;
};

struct work *new_empty_work();
void serve_user(struct work *wk);
