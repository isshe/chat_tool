#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>				/*sockaddr_i*/
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>					/*memset*/


#define PORT 8889		/*端口号*/
#define MSGSIZE 1024	/*消息的大小,不知道需不需要*/
#define SIZE	16		/*ID大小,name大小,*/
//#define STR_SIZE sizeof(struct USER)

/*一个用户结构*/
struct USER
{	
	int	    flag ;					/*0.退出,-1为注册,-2登陆,-3获取在线列表,-4获取个人信息,*/
    long    user_ID ;
	char    secret[SIZE];
	char    name[SIZE];
	int     user_sockfd ;
	struct  sockaddr_in user_addr;		/*用户地址信息,用于两客户端间的通信*/
	struct  USER *next;			    /*用于创建链表*/
//	char    send_recv[MSGSIZE];

};

struct ONLINE
{
	char name[SIZE];
	long ID;
};

/*****************************************************************
 * name:		pthread_fun()
 * call:		user_land(); user_enroll(); get_list();	
 * call by:		main()
 * parameter:	void *arg		未确定
 * return:			
 * date:		2014.10.14
 * **************************************************************/
void *pthread_fun(struct USER *arg);			/*传了accept返回的套接字!*/


/****************************************************************
 * name:user_land()
 * call:
 * call by: pthread_fun
 * parameter:
 * return :
 * funtion:		注册用户
 * date:
 ****************************************************************/
void user_land(struct USER *recv_msg);	

/***************************************************************
 * name:		user_enroll
 * call:
 * call by:		pthread_fun
 * parameter:
 * return:
 * funtion:		用户登陆
 * date:
 * *************************************************************/
void user_enroll(struct USER *recv_msg);

/**************************************************************
 * name:		get_list()
 * call:
 * call by:		pthread_fun
 * parameter:	
 * return:
 * funtion:		获取在线列表
 * date:
 * J************************************************************/

void get_list(int sockfd);

/**************************************************************
 * name:		get_information
 * call:
 * call by:		pthread_fun
 * parameter:
 * return:
 * funtion:		获取单个用户的资料(主要是sockfd,addr)
 * date:		2014-10-18
 * ************************************************************/
void get_information(int sockfd, long ID);


/****************************************************************
 * ***************************************************************/

/*链表的头指针和头节点*/
struct USER *first;		/*附加头节点*/

/*插入元素到链表(在表头插入)*/
void user_insert(struct USER *inse_user);

/*删除元素,当用户下线时(参数应该用用户ID)*/
void user_remove(struct USER *rm_user);

/*输出在线用户*/
void user_output();

#endif
