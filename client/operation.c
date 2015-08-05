#include "client.h"

int create_udpser(int ser_sockfd)			/*总服务器的sockfd*/
{
	int serfd = 0;
	struct USER send_fdaddr;                  /*把sockfd和addr发送给服务器保存!*/
	struct sockaddr_in seraddr;
    unsigned int udp_port = 0;
    
    while(1)
    {
        fprintf(stdout, "请输入udp服务器端口号(大于3000,小于30000):\n");
        fscanf(stdin, "%d", &udp_port);
        setbuf(stdin, NULL);
        
        if ((udp_port <= 3000) && (udp_port > 30000))
        {
            printf("输入的端口不符合范围!\n");
            continue;
        }
        else
        {
            break;
        }
    }
    
    /*socket*/
	if ((serfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		fprintf(stderr, "create_udpser/socket error:%s\n", strerror(errno));
		fprintf(stdout, "udp服务器创建失败!\n");
	}
	else
	{
		fprintf(stdout, "create_udpser/socket succeed!\n");
	}

	/*填充结构*/
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(udp_port);
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) == -1)
	{
		fprintf(stderr, "create_udpser/bind error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "create_udpser/bind succeed!\n");
	}
	
	send_fdaddr.user_addr = seraddr;
	send_fdaddr.sockfd = serfd;

    /*发送给服务器保存*/
	if (send(ser_sockfd, (struct USER *)&send_fdaddr, sizeof(send_fdaddr), 0) == -1)
	{
		fprintf(stderr, "create_dupser/send error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "creat_dupser/send succeed!\n");
	}
	
	
	printf("create_udpserver/family = %d, s_addr = %d, port = %d\n"
	        , send_fdaddr.user_addr.sin_family, seraddr.sin_addr.s_addr, seraddr.sin_port);
	
	return serfd;			
}


/*获取在线列表*/
void get_list(int ser_sockfd)
{
	int i = 0;
	int flag = -3;									/*-3获取列表*/
	struct ONLINE online[1024];

    for (i = 0; i < 1024; i++)
    {
        online[i].ID = -1;
    }
	if (send(ser_sockfd, &flag, sizeof(int), 0) == -1)						/*看服务器是否接收得了*/
	{
		fprintf(stderr, "land/select/get_list error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout, "land/select/get_list send succeed!\n");
	}

	memset(online, 0, sizeof(online));
	if (recv(ser_sockfd, online, sizeof(online), 0) == -1)
	{
		fprintf(stderr, "land/select/get_list/recv error:%s\n", strerror(errno));
	}
	else
	{   
	    fprintf(stdout, "land/select/get_list/recv succeed!\n");
	    fprintf(stdout, "昵称(账号)\n");
		for( i = 0; online[i].ID > 0; i++)
		{
			fprintf(stdout, "%s(%ld)\n", online[i].name, online[i].ID);
		}
	}
}


void get_information(int read_num, int ser_sockfd, int udp_serfd, long ID)
{
    struct USER udp_partner;
    struct USER send_user;
    struct USER myself;
    char send_msg[MSGSIZE];
    
    if (send(ser_sockfd, &ID, sizeof(int), 0)	== -1)		/*看下服务器flag是否正确*/
    {
	    fprintf(stderr, "select/get_information/send1 error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout,"select/get_information/send1 succeed!\n");
	}
	
	if (recv(ser_sockfd, &myself, sizeof(struct USER), 0) == -1)
	{
	    fprintf(stderr, "select/get_information/recv1 error:%s\n", strerror(errno));
	}
	else
	{
	    fprintf(stdout, "select/get_information/recv1 succeed!\n");
	}
	fprintf(stdout, "my_ID = %ld\nmy_name = %s\n", myself.user_ID, myself.user_name);
	fprintf(stdout, "my_family = %d, my_data = %d\n", myself.user_addr.sin_family, myself.user_addr.sin_port);
    
    if (send(ser_sockfd, &read_num, sizeof(int), 0)	== -1)		/*看下服务器flag是否正确*/
    {
	    fprintf(stderr, "select/get_information/send2 error:%s\n", strerror(errno));
	}
	else
	{
		fprintf(stdout,"select/get_information/send2 succeed!\n");
	}
	
	if (recv(ser_sockfd, &udp_partner, sizeof(udp_partner), 0) == -1)
	{
	    fprintf(stderr, "select/get_information/recv2 error:%s\n", strerror(errno));
	}
	else
	{
	    fprintf(stdout, "select/get_information/recv2 succeed!\n");
	}
	fprintf(stdout, "partner_ID = %ld\npartner_name = %s\n", udp_partner.user_ID, udp_partner.user_name);
	fprintf(stdout, "family = %d, data = %d\n", udp_partner.user_addr.sin_family, udp_partner.user_addr.sin_port);

    /*发送ID和name*/
    strcpy(send_user.user_name, myself.user_name);
    send_user.user_ID = myself.user_ID;
    printf("发送的:name = %s, ID = %ld\n", send_user.user_name, send_user.user_ID);
    if (sendto(udp_serfd, &send_user, sizeof(send_user), 0,
                (struct sockaddr*)&udp_partner.user_addr, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "sendto ID, name error:%s\n", strerror(errno));
    }
    else
    {
        fprintf(stdout, "sendto ID, name succeed!\n");
    }
    
    /*chat*/
    memset(send_msg, 0, MSGSIZE);
    fprintf(stdout, "请输入要发送的信息:\n");
    fscanf(stdin, "%s", send_msg);
    setbuf(stdin, NULL);
    
    if (sendto(udp_serfd, &send_msg, strlen(send_msg), 0, 
        (struct sockaddr*)&udp_partner.user_addr, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "sendto/error!%s\n", strerror(errno));
    }
    else
    {
        fprintf(stdout, "sendto succeed!\n");
    }  

//    return udp_partner;
	
}
