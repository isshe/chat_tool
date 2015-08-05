#include "server.h"

/*注册*/
void user_enroll(struct USER *recv_msg)
{
	int		fd = recv_msg->user_sockfd;
	long	send_num = 0;					/*成功发送Id,失败发送-1*/
	long	total = 0;	/*用于计算所有用户数,其实也可以用最后一个用户+1*/
	FILE	*fp_r = NULL;
	FILE    *fp_w = NULL;
//	size_t	fread_retn;
	ssize_t send_retn;					/*send的返回值*/
	struct	USER *user = recv_msg;
	struct	USER read_user;

    if ((fp_r = fopen("user", "r")) == NULL)
	{
		fprintf(stderr, "user_enroll fopen error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "user_enroll fopen succeed!\n");
	    	
	    fseek(fp_r, 0, SEEK_SET);				/*重置文件指针*/		
	    while (fread(&read_user, sizeof(struct USER), 1, fp_r) == 1)
	    {
		    total++;
		    fprintf(stdout,"total = %ld\n", total);
	    }
	    
	    close(fp_r);
	    fp_r = NULL;
    }

	
	user->user_ID = total+1;
	/*写入文件*/
    if ((fp_w = fopen("user", "a+")) == NULL)
	{
		fprintf(stderr, "user_enroll fopen error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "user_enroll fopen succeed!\n");
	}  
    fseek(fp_w, 0, SEEK_CUR);
	if (fwrite(user, sizeof(struct USER), 1, fp_w) != 1) /*失败,发送-1*/
	{
		send_num = -1;
		
		if ((send_retn = send(fd, &send_num, sizeof(int), 0)) == -1)
		{
			fprintf(stderr, "user_enroll/fwrite/send error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stdout, "user_enroll/fwrite/send succeed:%d\n", send_retn);
		}
		close(fp_w);
	}
	else		/*写入成功,发送ID*/
	{   
	    fflush(fp_w);
	    fprintf(stdout, "写入成功!\n");
		send_num = total+1;				/*应该不用+1*/
		if ((send_retn = send(fd, &send_num, sizeof(int), 0)) == -1)
		{
			fprintf(stderr, "user_enroll else send error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stdout, "user_enroll else send succeed:send_num = %ld\n", send_num);
		}
		/*插入到在线列表*/
//		user->online = 1;				/*fopen可不要*/
//		user_insert(user);
	}
	
	close(fp_w);
}

/*登陆*/
void user_land(struct USER *recv_msg)
{
	int		fd = recv_msg->user_sockfd;		/*fd*/
	struct	USER *user = recv_msg;
	struct	USER read_user;
	struct  USER recv_fdaddr;                /*接收udp服务器发来的sockfd和addr*/
	struct	sockaddr_in recv_addr;			/*接收客户端发送来的udp_addr*/
	FILE	*fp;
	int		send_num = 0;					/*登陆成功发送1,否则发送-1*/
	
	fprintf(stdout, "fopen 之前!\n");
	if ((fp = fopen("user", "r")) == NULL)
	{
		fprintf(stderr, "land fopen error:%s\n", strerror(errno));
		
		send_num = -2;
		if (send(fd, &send_num, sizeof(int), 0) == -1)
		{
			fprintf(stderr, "land/fread/else/send error:%s\n", strerror(errno));
		}
		else
		{
			fprintf(stderr, "land/fread/else/send succeed!\n");
		}
		return (void)0;
	}
	else
	{
		fprintf(stdout, "land fopen succeed!\n");
	}

//	fprintf(stdout,"fseek zhiqian\n");
	fseek(fp, 0, SEEK_SET);

	while (1)
	{
		
		if (fread(&read_user, sizeof(struct USER), 1, fp)== 1)
		{
		    if (read_user.user_ID == user->user_ID)
		    {
			    if (strcmp(read_user.secret, user->secret) == 0)	/*密码正确,发送1为确认信息过去*/
			    {
				    send_num = 1;
				    if (send(fd, &send_num, sizeof(int), 0) == -1)
				    {
					    fprintf(stderr, "land/fread/send error:%s\n", strerror(errno));
				    }
				    else
				    {
					    fprintf(stdout, "land/fread/send succeed!\n");
					
					    /*接收一个udp结构,保存在struct USER的struct sockaddr中*/
					    if (recv(fd, (struct USER *)&recv_fdaddr, sizeof(recv_fdaddr), 0) == -1)
					    {
						    fprintf(stderr, "land/fread/recv error:%s\n", strerror(errno));
					    }
					    else
					    {
						    fprintf(stdout, "/land/fread/recv succeed!\n");
						    printf("接收到的结构为:sin_f = %d, sin_port = %d, s_addr = %d\n"
						            , recv_fdaddr.user_addr.sin_family, recv_fdaddr.user_addr.sin_port
						            , recv_fdaddr.user_addr.sin_addr.s_addr);
//						    user->user_addr = recv_addr;					/*结构体直接赋值,不知道可不可以*/
					    }
					    /*将用户插入到在线列表*/
					    read_user.user_sockfd = recv_fdaddr.user_sockfd;
					    read_user.user_addr = recv_fdaddr.user_addr;
					    user_insert(&read_user);                             /*用在文件中读到的插入*/
					    fprintf(stdout, "已将用户插入到在线列表!\n");
					                      
					    fclose(fp);
					    break;
				    }
			    }
		        else											/*密码错误*/
		        {
			        send_num = -1;
			        if (send(fd, &send_num, sizeof(int), 0) == -1)
			        {
				        fprintf(stderr, "land/fread/else/send error:%s\n", strerror(errno));
			        }
			        else
			        {
				        fprintf(stderr, "land/fread/else/send succeed!\n");
				        fclose(fp);
				        break;
			        }
		        }/*else*/
		    }/*if*/
		}
		else
		{
		     send_num = -2;
			 if (send(fd, &send_num, sizeof(int), 0) == -1)
			 {
				  fprintf(stderr, "land/fread/else/send error:%s\n", strerror(errno));
			 }
			 else
			 {
				  fprintf(stderr, "land/fread/else/send succeed!\n");
				  fclose(fp);
				  break;
			 }   
		}
	}/*while*/

}

/*获取在线列表*/
void get_list(int sockfd)
{
	struct ONLINE online[1024];				/*这里不够好*/
	struct USER *current = first->next;
	int i = 0;

	for(; current != NULL; current = current->next, i++)
	{
		printf("current->name = %s, current->user_ID = %ld.\n", current->name, current->user_ID);
		strcpy(online[i].name, current->name);
		online[i].ID = current->user_ID;
		printf("online[%d].name = %s, online[%d].ID = %ld\n", i, online[i].name, i, online[i].ID);
	}

	if (send(sockfd, online, sizeof(online), 0) == -1)
	{
		fprintf(stderr, "get_list/send error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "get_list/send succeed!\n");
	}
}

/*获取单个用户详细信息*/
void get_information(int sockfd, long ID)
{
	struct USER *current = first->next ;
	struct USER target_user;


    printf("进来get_information了吗\n");
/*    printf("现在输出链表的信息:\n");
    while(current != NULL)
    {
       
        
        current = current->next;
    }
*/
	while (current != NULL)
	{ 
	    printf("ID = %ld\n", ID);
	    fprintf(stdout, "get_内:current->ID = %ld\n", current->user_ID);
        fprintf(stdout, "current->name = %s\n", current->name);
        fprintf(stdout, "current->secret = %s\n\n", current->secret);
	    printf("循环!\n");
		
		if (ID == current->user_ID)
		{
			target_user = *current;
			if (send(sockfd, &target_user, sizeof(struct USER), 0) == -1)
			{
				fprintf(stderr, "get_information/send error:%s\n", strerror(errno));
			}
			else
			{
				fprintf(stdout, "get_inforamtion/send succeed!\n");
				fprintf(stdout, "对方获取的信息是:\n{\n\tID = %ld\n", target_user.user_ID);
				fprintf(stdout, "\tname = %s\n}\n", target_user.name);
			}
			break;
		}
		
		current = current->next;
	}/*while*/
	
	printf("get_information/循环外!\n");

}
