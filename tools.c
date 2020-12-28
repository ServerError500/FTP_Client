#include<stdio.h>
#include <stdint.h>
#include <getch.h>
#include "tools.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUF_SIZE (4096)
int cmd_fd;
char buf[BUF_SIZE];

// 清理输入缓冲区
static void clear_stdin(void)
{
    stdin->_IO_read_ptr = stdin->_IO_read_end;
}

// 从键盘获取字符串
char* get_str(char* str,size_t hope_len)
{
    if(NULL == str || 0 == hope_len) return NULL;

    size_t index = 0;
    while(index < hope_len-1)
    {
        int8_t key_val = getch();
        if(10 == key_val) break;
        if(127 == key_val)
        {
            if(index > 0)
            {
                printf("\b \b");
                index--;
            }
            continue;
        }
        printf("%c",key_val);
        str[index++] = key_val;
    }
    
    str[index] = '\0';
    
    printf("\n");

    clear_stdin();

    return str;
}

// 从键盘获取密码
char* get_passwd(char* pd,size_t hope_len,bool is_show)
{
    if(NULL == pd) return NULL;

    size_t index = 0;
    while(index < hope_len-1)
    {
        int32_t key_val = getch();
        if(127 == key_val)
        {
            if(index > 0)
            {
                index--;
                if(is_show) printf("\b \b");
            }
        }
        else if(10 == key_val)
        {
            break;
        }
        else
        {
            pd[index++] = key_val;
            if(is_show) printf("*");
        }
    }

    pd[index] = '\0';

    printf("\n");

    clear_stdin();

    return pd;
}

char pasv_client(void){
typedef struct sockaddr *SAP;
cmd_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > cmd_fd)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(21);
	//addr.sin_addr.s_addr = inet(argv[1]);
	addr.sin_addr.s_addr = inet_addr("192.168.126.129");
	socklen_t addrlen = sizeof(addr);

	if (connect(cmd_fd, (SAP)&addr, addrlen))
	{
		perror("connect");
		return -1;
	}


	sprintf(buf, "PASV\n");
			send(cmd_fd, buf, strlen(buf), 0);
			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);

			int ip1, ip2, ip3, ip4, port1, port2;
			sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d",
				   &ip1, &ip2, &ip3, &ip4, &port1, &port2);

			char ip[16] = {};
			sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
			short port = port1 * 256 + port2;

			int data_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (0 > data_fd)
			{
				perror("socket");
				return -1;
			}

			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip);

			if (connect(data_fd, (SAP)&addr, addrlen))
			{
				perror("connect");
				return -1;
			}
}


