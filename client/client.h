#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>		
#include <netdb.h>
#include <sys/select.h>
#include <errno.h>
#include <ctype.h>


#define MSGSIZE 1024
#define SIZE 16
#define PORT 8889
//#define UDP_PORT 4567
#define SERVER_ID "127.0.0.1"

struct USER 
{
	int		flag;
	long	user_ID;
	char	user_secret[SIZE];
	char	user_name[SIZE];
	int     sockfd;
	struct	sockaddr_in user_addr;
	struct	USER *next; 
//	char    send_recv[MSGSIZE];

};

struct ONLINE
{
	char name[SIZE];
	long ID;
};


/*主要的函数*/
void main_fun(int ser_sockfd);

/*注册*/
int user_enroll(int ser_sockfd);

/*登陆*/
int user_land(int ser_sockfd);

/*获取在线用户列表*/
void get_list(int ser_sockfd);

/*udp服务器创建*/
int create_udpser(int ser_sockfd);

/*udp客户端创建*/
void create_udpcli(long ID, int ser_sockfd);				/**/

/*聊天*/
void user_chat(int ID, int ser_sockfd);			/**/

/*获取聊天对象的信息!*/
void get_information(int read_num, int ser_sockfd, int udp_serfd, long ID);


#endif
