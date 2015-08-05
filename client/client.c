#include "client.h"


int main(void)
{
	int		sockfd = 0;
	struct	sockaddr_in servaddr;
	struct	hostent *host;

	if ((host = gethostbyname(SERVER_ID)) == NULL)
	{
		fprintf(stderr, "main/gethostbyname error:%s\n", strerror(h_errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "main/gethostbyname succeed!\n");
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "main/socket error:%s\n", strerror(errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "main/socket succeed!\n");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr = *(struct in_addr*)host->h_addr;		/*不太理解*/

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
	{
		fprintf(stderr, "main/connect error:%s\n", strerror(errno));
		exit(1);
	}
	else
	{
		fprintf(stdout, "main/connect succeed!\n");
	}

	main_fun(sockfd);		/*暂时放sockfd*/



	close(sockfd);
	return 0;

}
