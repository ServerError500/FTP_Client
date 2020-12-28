#include <stdio.h>
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
#include "tools.h"

#define BUF_SIZE (4096) 
int cmd_fd;
char buf[BUF_SIZE];


typedef struct sockaddr *SAP;

int main(int argc, char *const argv[])
{
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

	bzero(buf, BUF_SIZE);
	recv(cmd_fd, buf, BUF_SIZE, 0);
	printf("recv:%s\n", buf);

	printf("user:");
	char name[20] = {};
	get_str(name, 20);
	sprintf(buf, "USER %s\n", name);
	send(cmd_fd, buf, strlen(buf), 0);
	bzero(buf, BUF_SIZE);
	recv(cmd_fd, buf, BUF_SIZE, 0);
	printf("%s\n", buf);
	
	printf("pass:");
	char pass[20] = {};
	get_str(pass, 20);
	sprintf(buf, "PASS %s\n", pass);
	send(cmd_fd, buf, strlen(buf), 0);
	bzero(buf, BUF_SIZE);
	recv(cmd_fd, buf, BUF_SIZE, 0);
	bzero(buf, BUF_SIZE);

	sprintf(buf, "OPTS UTF8 ON\n");
	send(cmd_fd, buf, strlen(buf), 0);
	bzero(buf, BUF_SIZE);
	recv(cmd_fd, buf, BUF_SIZE, 0);
	printf("recv:%s\n", buf);

	sprintf(buf, "PWD\n");
	send(cmd_fd, buf, strlen(buf), 0);
	bzero(buf, BUF_SIZE);
	recv(cmd_fd, buf, BUF_SIZE, 0);
	printf("recv:%s\n", buf);

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

	while (1)
	{
		printf("ftp> ");
		char cmd[20] = {};
		get_str(cmd, sizeof(cmd));
		if (0 == strcmp("quit", cmd))
		{
			return -1;
		}

		if (0 == strcmp("ls", cmd))
		{
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

			sprintf(buf, "LIST -al\n");
			send(cmd_fd, buf, strlen(buf), 0);
			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);

			bzero(buf, BUF_SIZE);
			while (0 < recv(data_fd, buf, BUF_SIZE, 0))
			{
				printf("%s", buf);
				bzero(buf, BUF_SIZE);
			}
			close(data_fd);

			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);
		}
		if (0 == strcmp("pwd", cmd))
		{
			sprintf(buf, "pwd\n");
			send(cmd_fd, buf, strlen(buf), 0);
			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);
		}
		if (0 == strncmp("cd ", cmd, 3))
		{
			char *path = cmd + 3;
			sprintf(buf, "CWD %s\n", path);
			send(cmd_fd, buf, strlen(buf), 0);
			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);
			continue;
		}
		if (0 == strncmp("mkdir ", cmd, 6))
		{
			char *path = cmd + 6;
			sprintf(buf, "MKD %s\n", path);
			send(cmd_fd, buf, strlen(buf), 0);
			bzero(buf, BUF_SIZE);
			recv(cmd_fd, buf, BUF_SIZE, 0);
			printf("recv:%s\n", buf);
			continue;
		}
		// if (0 == strncmp("rm ", cmd, 3))
		// {
		// 	sprintf(buf, "PASV\n");
		// 	send(cmd_fd, buf, strlen(buf), 0);
		// 	bzero(buf, BUF_SIZE);
		// 	recv(cmd_fd, buf, BUF_SIZE, 0);
		// 	printf("recv:%s\n", buf);

		// 	int ip1, ip2, ip3, ip4, port1, port2;
		// 	sscanf(buf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d",
		// 		   &ip1, &ip2, &ip3, &ip4, &port1, &port2);

		// 	char ip[16] = {};
		// 	sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
		// 	short port = port1 * 256 + port2;

		// 	int data_fd = socket(AF_INET, SOCK_STREAM, 0);
		// 	if (0 > data_fd)
		// 	{
		// 		perror("socket");
		// 		return -1;
		// 	}

		// 	addr.sin_port = htons(port);
		// 	addr.sin_addr.s_addr = inet_addr(ip);

		// 	if (connect(data_fd, (SAP)&addr, addrlen))
		// 	{
		// 		perror("connect");
		// 		return -1;
		// 	}

		// 	char *path = cmd + 3;
		// 	sprintf(buf, "DELE %s\n", path);
		// 	send(cmd_fd, buf, strlen(buf), 0);
		// 	bzero(buf, BUF_SIZE);
		// 	recv(cmd_fd, buf, BUF_SIZE, 0);
		// 	printf("recv:%s\n", buf);

		// 	sprintf(buf, "LIST\n");
		// 	send(cmd_fd, buf, strlen(buf), 0);
		// 	bzero(buf, BUF_SIZE);
		// 	recv(cmd_fd, buf, BUF_SIZE, 0);
		// 	printf("recv:%s\n", buf);
		// 	continue;
		// }
		if(0 == strncmp(cmd,"get ",4))
		{
			sprintf(buf,"PASV\n");
			send(cmd_fd,buf,strlen(buf),0);
			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);

			int ip1,ip2,ip3,ip4,port1,port2;
			sscanf(buf,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d",
				&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	
			char ip[16] = {};
			sprintf(ip,"%d.%d.%d.%d",ip1,ip2,ip3,ip4);
			short port = port1*256+port2;

			int data_fd = socket(AF_INET,SOCK_STREAM,0);
			if(0 > data_fd)
			{
				perror("socket");
				return -1;
			}

			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip);

			if(connect(data_fd,(SAP)&addr,addrlen))
			{
				perror("connect");
				return -1;
			}
			char* p = cmd+4;
			sprintf(buf,"RETR %s\n",p);
			send(cmd_fd,buf,strlen(buf),0);
			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);
			if(0==strncmp(buf,"550",3))
			{
				continue ;
			}

			int fd = open(p,O_WRONLY|O_CREAT,0644);
			if(0 > fd)
			{
				perror("open");
				return -1;
			}

			int ret_size = 0;
			while(ret_size = recv(data_fd,buf,BUF_SIZE,0))
			{
				write(fd,buf,ret_size);
			}

			close(data_fd);
			close(fd);

			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);
			continue ;
		}
		if(0 == strncmp(cmd,"put ",4))
		{
			sprintf(buf,"PASV\n");
			send(cmd_fd,buf,strlen(buf),0);
			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);

			int ip1,ip2,ip3,ip4,port1,port2;
			sscanf(buf,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d",
				&ip1,&ip2,&ip3,&ip4,&port1,&port2);
	
			char ip[16] = {};
			sprintf(ip,"%d.%d.%d.%d",ip1,ip2,ip3,ip4);
			short port = port1*256+port2;

			int data_fd = socket(AF_INET,SOCK_STREAM,0);
			if(0 > data_fd)
			{
				perror("socket");
				return -1;
			}

			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip);

			if(connect(data_fd,(SAP)&addr,addrlen))
			{
				perror("connect");
				return -1;
			}
			char* p = cmd+4;
			sprintf(buf,"STOR %s\n",p);
			send(cmd_fd,buf,strlen(buf),0);
			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);
			if(strncmp(buf,"150",3))
			{
				continue ;
			}
			int fd = open(p,O_RDONLY);
			if(0 > fd)
			{
				perror("open");
				continue;
			}

			int ret_size = 0;
			while(ret_size = read(fd,buf,BUF_SIZE))
			{
				send(data_fd,buf,ret_size,0);
			}
			close(fd);
			close(data_fd);

			bzero(buf,BUF_SIZE);
			recv(cmd_fd,buf,BUF_SIZE,0);
			printf("recv:%s\n",buf);
			continue ;
		}
		
	}
}
