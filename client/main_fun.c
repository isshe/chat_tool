/***********************************************************
 *					写在前面
 *1,注意select的第一个参数!
 *
 * ********************************************************/


#include "client.h"


void main_fun(int ser_sockfd)
{
//	int		maxfd = 0;				/*select*/
//	fd_set	set ;				/*文件描述符集*/
	int		operate_num = 0;

    while(1)
    {
	    fprintf(stdout, "----------------------------------------\n");
	    fprintf(stdout, " 1.注册;\n 2.登陆;\n 0.退出!\n");
	    fprintf(stdout, "----------------------------------------\n");
	    setbuf(stdin, NULL);
	    fscanf(stdin, "%d", &operate_num);

        if (!isdigit(operate_num))
        {
	        switch (operate_num)
	        {
		        case 0:					/*退出*/
			        exit(1);
		        case 1:					/*注册*/
			        user_enroll(ser_sockfd);			
			        break;
		        case 2:
			        user_land(ser_sockfd);	/*登陆*/
			        break;
		        default:
			        break;

	        }
	    }
	    else
	    {
	        printf("输入不对,请重新输入!\n");
	        continue;
	    }
    }	
}	
	


