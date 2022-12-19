#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

	        if(argc != 2){
			        fprintf(2, "usage: sleep <time>\n");
				        exit(1);
					    }
		    int sleepTime;
		        sleepTime = atoi(argv[1]);
			    if (sleep(sleepTime) != 0) {
				            exit(1);
					        }
			        exit(0);
}
