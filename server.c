#pragma region headers
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "main.c" 

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>		 //write
#include <pthread.h>	 //for threading , link with lpthread
#include <unistd.h>		 // foe sleep

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <stddef.h>

#pragma endregion

#define NULL ((void *)0)
#define PORT 8888


