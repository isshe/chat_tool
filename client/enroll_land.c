#include "client.h"


int user_enroll(int ser_sockfd)
{
	
	struct USER user;
	char name[SIZE];
	char secret1[SIZE];
	char secret2[SIZE];
	int	 recv_num = 0;

	while (1)
	{
		fprintf(stdout, "请输入昵称:\n");
		fscanf(stdin, "%s", name);
		setbuf(stdin, NULL);
		if (strlen(name) > 15)
		{
			fprintf(stdout, "您输入的昵称太长!\n请重新输入!\n");
			continue;
		}

		fprintf(stdout, "请输入密码:\n");
		fscanf(stdin, "%s", secret1);
		setbuf(stdin, NULL);
		if (strlen(secret1) > 15)
		{
			fprintf(stdout, "您输入的密码太长!\n请重新输入!\n");
			continue;
		}		

		fprintf(stdout, "请再次输入密码:\n");
		fscanf(stdin, "%s", secret2);
        setbuf(stdin, NULL);
		if (strcmp(secret1, secret2) != 0)
		{
			fprintf(stdout, "您输入的密码不匹配!\n请重新输入!\n");
			continue;
		}
		else
		{
			fprintf(stdout, "注册中...\n");
			break;
		}
	}

	memset(&user, 0, sizeof(user));
	strcpy(user.user_name, name);
	strcpy(user.user_secret, secret1);
	user.flag = -1;							/*注意这里, -1注册*/

	if (send(ser_sockfd, &user, sizeof(user), 0) == -1)
	{
		fprintf(stderr, "main/main_fun/user_enroll error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "main/main_fun/user_enroll succeed!\n");
	}

	if (recv(ser_sockfd, &recv_num, sizeof(int), 0) == -1)
	{
		fprintf(stderr, "main/main_fun/user_enroll error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "main/main_fun/user_enroll succeed!\n");

		if (recv_num > 0)			/*返回一个ID*/
		{
			fprintf(stdout, "注册成功!\n您的ID为:%d!请牢记,登陆要用哦\n", recv_num);
		}
		else if (recv_num == -1)
		{
			fprintf(stdout, "注册失败,服务器淘气了!\n");
			return 0;
		}
		else
		{
			fprintf(stdout, "服务器不知道返回什么东西了!\n");
		}

	}/*else*/
}


int user_land(int ser_sockfd)
{
	int		maxfd = 0;				/*select*/
	fd_set	set ;				
	int		udp_serfd = 0;			/*udp服务器的sockfd*/
	int		recv_num = 0;			/*-1失败, 1成功*/
	long	ID = 0;
	char	secret[SIZE] = {0};
	struct	USER user;
	ssize_t read_num ;
	char	recv_msg[MSGSIZE];
	struct	USER udp_partner;          /*用来接收对方信息*/
	struct  sockaddr udp_cliaddr;
	socklen_t seraddr_size;

	fprintf(stdout, "----------登录----------\n");
	while(1)
	{
		fprintf(stdout, "请输入账号:\n");
		fscanf(stdin, "%ld[\t\n\r]", &ID);
		setbuf(stdin, NULL);
		if ( ID == 0)                   /*输入0,或者输入的是字符,字符串*/
		{
		    fprintf(stdout, "您的输入有误,请重新输入!%ld\n", ID);		    
		    break;
		}
		 
		fprintf(stdout, "请输入密码:\n");
		fscanf(stdin, "%s[\t\n\r]", secret);
        setbuf(stdin, NULL);
        
		if (strlen(secret) > 15)
		{
			fprintf(stdout, "密码太长,请检查,重新输入!\n");
			break;
		}
		user.flag = -2;					/*注意这里, -2登陆*/
		user.user_ID = ID;
		strcpy(user.user_secret, secret);

		if (send(ser_sockfd, &user, sizeof(user), 0) == -1)
		{
			fprintf(stdout, "main/main_fun/user_land/send error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stdout, "main/main_fun/user_land/send succeed!\n");
		}

		/*接收确认信息*/
		if (recv(ser_sockfd, &recv_num, sizeof(int), 0) == -1)
		{
			fprintf(stdout, "main/main_fun/user_land/recv error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stdout, "main/main_fun/user_land/recv succeed!\n");
			if (recv_num == 1)
			{
				fprintf(stdout, "登陆成功!\n");
				break;
			}
			else if (recv_num == -1)
			{
				fprintf(stdout, "登陆失败,密码错误!\n");
				break;
			}
			else if (recv_num == -2)
			{
			    fprintf(stdout, "登陆失败,还没注册吧逗比?\n");
			    return 0;
			}
			else
			{
				fprintf(stdout, "服务器返回:%d(user_land/recv)", recv_num);
				return 0;
			}
		}
	}/*while*/

	if (recv_num == 1)							/*登陆成功*/
	{
	    /*建立udp服务器*/
		udp_serfd = create_udpser(ser_sockfd);
		printf("udp_serfd = %d\n", udp_serfd);			
		
		/*下面应该用一个select?监听3个字符集,
		 * stdin, ser_sockfd, udp_serfd  */
		while (1)
		{
		    fprintf(stdout,"--------------------------------------\n");
		    fprintf(stdout, "1.聊天直接输入对方ID;\n"
				"2.获取在线列表输入-1\n3.退出输入0\n");
		    fprintf(stdout, "-------------------------------------\n");
			
			FD_ZERO(&set);
			FD_SET(fileno(stdin), &set);			/*输入*/
			FD_SET(udp_serfd, &set);		/*用户通信*/
//			FD_SET(ser_sockfd, &set);		/*接收在线列表*/

//			maxfd = udp_serfd > ser_sockfd ? udp_serfd : ser_sockfd;
            maxfd = udp_serfd+1;
			switch (select(maxfd, &set, NULL, NULL, NULL))
			{
				case -1:
					fprintf(stderr, "user_land/select error:%s\n", strerror(errno));
				case 0:
					continue;
				default:
					{
					    printf("select succeed!\n");
					    
						if (FD_ISSET(fileno(stdin), &set))
						{
						    fscanf(stdin, "%d", &read_num);
						    
						    printf("这是fscanf的到的  read_num = %d\n", read_num);

							if (read_num == -1)                 /*获取在线列表*/
							{
								get_list(ser_sockfd);			/*看服务器flag是否正确*/
								setbuf(stdin, NULL);
							} 			
							else if (read_num == 0)             /*退出*/
							{
							    user.flag = read_num;
//							    user.user_ID = ID;
							    printf("flag = %d, user_ID = %ld\n", user.flag, user.user_ID);
								if (send(ser_sockfd, &user, sizeof(user), 0)	== -1)		/*看下服务器flag是否正确*/
								{
									fprintf(stderr, "select/send error:%s\n", strerror(errno));
								}
								else
								{
									fprintf(stdout,"select/send 0 succeed!\n");
								}
								exit(-1);
							}
							else if (read_num > 0)              /*聊天*/
							{
					
								get_information(read_num, ser_sockfd, udp_serfd, ID); 
							}
							
						}/*if*/
						
						if (FD_ISSET(udp_serfd, &set))					
						{
						    seraddr_size = sizeof(udp_cliaddr);
                            
                            memset(&udp_partner, 0, sizeof(struct USER));
                            if (recvfrom(udp_serfd, &udp_partner, sizeof(struct USER), 0,
                                         (struct sockaddr *)&udp_cliaddr, &seraddr_size) == -1)
                            {
                                fprintf(stderr, "recvform ID,name error:%s\n", strerror(errno));
                            }
                            else
                            {
                                fprintf(stdout, "recvform ID, name succeed!\n");
                            }
                            
                            memset(recv_msg, 0, MSGSIZE);
							if (recvfrom(udp_serfd, recv_msg, MSGSIZE, 0, (struct sockaddr *)&udp_cliaddr, &seraddr_size) == -1)
							{
								fprintf(stderr, "select/recvfrom error:%s\n", strerror(errno));
							}
							else
							{
							    recv_msg[strlen(recv_msg)] = '\0';
								fprintf(stdout, "select/recvfrom succeed!\n");
								fprintf(stdout, "从%s(%ld)收到的信息:%s\n", 
								        udp_partner.user_name, udp_partner.user_ID, recv_msg);
//								memset(recv_msg, 0, sizeof(recv_msg));
							}
						}
/*						if (FD_ISSET(ser_sockfd, &set))
						{
							//
						}
*/					}
			}/*switch*/
		}/*while*/
	}/*if*/
}



