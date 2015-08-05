#include "server.h"

void *pthread_fun(struct USER *arg)
{
	int recv_byte = 0;
	long ID = arg->user_ID;
	int sockfd = arg->user_sockfd;					/*这里可能要改*/
//	struct sockaddr addr = arg->addr
	char msg[MSGSIZE];
	struct USER recv_buf;					/*注意是一个结构*/
	
	
//	printf("shifoudaozhelile");
	while (1)
	{

		memset(&recv_buf, 0, sizeof(recv_buf));
		if ((recv_byte =recv(sockfd, &recv_buf, sizeof(recv_buf), 0)) < 0)
		{
			fprintf(stderr, "pthread_fun recv error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stdout, "pthread_fun recv succeed, flag = %d!\n", recv_buf.flag);
		}
		
		recv_buf.user_sockfd = sockfd;				/*注册登陆要用*/
//		recv_buf.user_addr = arg->user_addr;		/*添加到在线列表要用*/
		switch (recv_buf.flag)
		{

			case -1:				/*注册*/
			    fprintf(stdout, "这是注册\n");
				user_enroll(&recv_buf);
//				break;						暂时不要看看.
			
			case -2:				/*登陆*/
			    fprintf(stdout, "这是登陆\n");
				user_land(&recv_buf);
				break;
			
			case -3:				/*获取在线列表*/
			    fprintf(stdout, "这是获取在线列表\n");
				get_list(sockfd);
			    break;

			case 0:				/*客户端退出,删除在线用户*/
			    fprintf(stdout, "这是退出\n");
//			    printf("47hang recv_buf.user_ID = %ld\n", recv_buf.user_ID);
				user_remove(&recv_buf);			/*注意客户端也要对应*/
				pthread_exit((void*)0);		/*不需要break*/	
				break;		
			    
			default:				/*获取单个用户详细信息*/
			{  
//			    setbuf(stdin, NULL);
			    setbuf(stdout, NULL);
			    fprintf(stdout, "这是获取个人信息\n");
				get_information(sockfd, recv_buf.flag);
				break;
            }
		}
/*
		if (recv_buf.flag == 0)
		{
			break;				
		}
*/
	}

	pthread_exit((void*)0);


}
