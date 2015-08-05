#include "server.h"


int main(int argc, char *argv[])
{
	int		sockfd = 0;
	int		newfd = 0;//[MSGSIZE] = {0};			/*暂不知道有没用*/
	int		err = 0;						/*用来存pthread_create的返回值*/
	struct	sockaddr_in servaddr;
	struct	sockaddr_in	clieaddr;
	struct  USER user;
	struct  USER *arg;						/*这个参数还不确定*/
	pthread_attr_t *attr;					/*用于设置线程属性*/	
	pthread_t thread_id ;
	socklen_t addrlen;						/*accept第三个参数*/
	
	/*头节点, first在server.h中定义*/	
	first = (struct USER*)malloc(sizeof(struct USER));
	first->next = NULL;
	 
	/*socket*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "socket error:%s\n", strerror(errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "sockfd succeed!\n");
	}

	/*填充结构信息*/
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/*bind*/
	if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) //
	{
		fprintf(stderr, "bind error:%s\n", strerror(errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "bind succeed!\n");
	}

	/*listen*/
	if (listen(sockfd, MSGSIZE) < 0)			/*MSGSIZE=1024,最大连接数*/
	{
		fprintf(stderr, "listen error:%s\n", strerror(errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "listen succeed!\n");
	}

	while(1)
	{
		/*accept*/
		addrlen = sizeof(clieaddr);
		if ((newfd = accept(sockfd, (struct sockaddr *)&clieaddr, &addrlen)) < 0)
		{
			fprintf(stderr, "accept error:%s\n", strerror(errno));
			exit(1);
		}
		else
		{
			fprintf(stdout, "accept succeed!\n");
		}

		/*初始化,并设置线程属性*/
		if ((err = pthread_attr_init(attr)) != 0 )											/*初始化参数*/
		{
			fprintf(stderr, "pthread_attr_init error:%s\n", strerror(err));
		}
		else
		{
			fprintf(stdout, "pthread_attr_init succeed!\n");
		}
		
		if ((err = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED)) != 0)		/*设置线程属性*/
		{
			fprintf(stderr, "pthread_attr_setdetachstate error:%s\n", strerror(err));
		}
		else
		{
			fprintf(stdout, "pthread_attr_setdeatstate succeed!\n");
		}
		
		arg = &user;							/*下面pthread_fun的参数*/
		user.user_sockfd = newfd;
		user.user_addr = clieaddr;					

        printf("xiancheng97hang\n");
		/*创建线程:pthread_create*/
		if ((err =pthread_create(&thread_id, NULL, (void*)pthread_fun, arg)) != 0)				/*用的是!=0*/
		{
			fprintf(stderr, "pthread_create error:%s\n", strerror(err));
		}
		else
		{
			fprintf(stdout, "pthread_create succeed!\n");
		}

	/*分离线程,释放内存*/
/*		if ((err = pthread_join(pthread_id, NULL)) != 0)					
		{
			fprintf(stderr, "pthread_join error:%s\n", strerror(err));
		}
		else
		{
			fprintf(stdout, "pthread_join succeed!\n");
		}
*/
	}
	
	return 0;
}
