#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <time.h>
#include "nftp.h"
#define MAXBUF 50
#define MAXBUFDATA 1024
#define CHAR20 20
#define CHAR60 60
#define PORT 21
//#define STDIN_FI:qLENO 1
//#define STDOUT_FILENO 0


#define USERNAME 220
#define PASSWORD 331
#define LOGIN 230
#define PATHNAME 257
#define CLOSEDATA 226
#define ACTIONOK 250


//char *rbuf,*rbuf1,*wbuf,*wbuf1;
//char *ftp_name,*ftp_pwd,*ftp_path/*,ftp_path_n*/;
char rbuf[MAXBUF], rbuf1[MAXBUF], wbuf[MAXBUF], wbuf1[MAXBUF];
char ftp_name[CHAR20], ftp_pwd[CHAR20], ftp_path[CHAR60];
char host[CHAR20];
//char rec_time_out[CHAR20] = "0.20";
int ftp_usec = 10;
int ftp_sec = 0;
struct sockaddr_in servaddr;
int fd = -1;

int cliopen(char *host, int port);
int strtosrv(char *str);
int ftp_get(int sck, char *pDownloadFileName);
int ftp_put(int sck, char *pUploadFileName_s, int size);
int cmd_tcp(int, char *, int size, char b[]);
void  setupFTP(const char *, const char *, const char *, const char *);
int ftp(char *, int, char a[]);
void destroyFTP();
/*
int main(int argc, char *argv[])
{
    char file[200];
    setupFTP("127.0.0.1", "xyz", "1", "picture/person");
    ftp((char*)"asdf", 4, file);
    destroyFTP();
}*/
void handle_pipe(int sig)
{
	//fd = -1;
	destroyFTP();
	setupFTP(host, ftp_name, ftp_pwd, ftp_path);
	printf("socket close error ,pipe break!,link again%d\n",sig);
}
void setupFtpTimeOut(const char* timeChar, int len)
{
	float value = atof(timeChar);
	ftp_sec = (int)value;
	ftp_usec = (value - ftp_sec) * 1000;

	printf("set%d sec=%d,usec=%d\n",len, ftp_sec, ftp_usec);
}
void setupFTP(const char *ip, const char *name, const char *pwd, const char *path)
{
	int static againCount = 0;
	printf("setupFTP %s,%s,%s,%s,count=%d\n", ip, name, pwd, path, againCount);
	if (againCount > 10)
		exit(0);
	else
		againCount++;

	strcpy(host, ip);
	strcpy(ftp_name, name);
	strcpy(ftp_pwd, pwd);
	strcpy(ftp_path, path);
	fd = ftp_connect(host, PORT, ftp_name, ftp_pwd);
	if (fd < 0)
	{
		printf("ftp connect fail!\n");
		destroyFTP();
		return;
	}
	int ret = ftp_cwd(fd, ftp_path);
	if (0 != ret)
	{
		printf("setup cwd is error,ret=%d", ret);
		destroyFTP();
	}
}
void modifyFtp(const char *ip, const char *name, const char *pwd, const char *path)
{
	/*strcpy(host, ip);
	strcpy(ftp_name, name);
	strcpy(ftp_pwd, pwd);
	strcpy(ftp_path, path);*/
	destroyFTP();
	setupFTP(ip, name, pwd, path);

}
void printfFtp(char out[], int size)
{
	sprintf(out, "ftp_ip=%s\nftp_name=%s\nftp_pwd=%s\nftp_path=%s\nftp_time_out=%.3f\n", host, ftp_name, ftp_pwd, ftp_path, ftp_sec + 0.001 * ftp_usec);
	printf("modifyFtp:%s,%d\n",out,size);
}
int ftp_active()
{
	int nwrite = 5;
	int again = -1;
	char* cmd = (char*) "system\r\n";
	int len = strlen(cmd);
	if ((nwrite = write(fd, cmd, len)) == len)
	{
		fd_set fdR;
		FD_ZERO(&fdR);
		FD_SET(fd, &fdR);
		struct timeval timeout;
		timeout.tv_usec = ftp_usec;
		timeout.tv_sec = ftp_sec;
		switch (select(fd + 1, &fdR, NULL, NULL, &timeout))
		{
		case -1:
			perror("select -1\n");
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
			break;
		case 0:
			printf("in 10us,there is no data(select)");
			break;
		default:
			if (FD_ISSET(fd, &fdR))
			{
				again = 1;
				char buf[100] = {0};
				int rec_len = -1;
				if ((rec_len = recv(fd, buf, 100, 0)) == -1)
				{
					printf("recv error ftp select\n");

				}
				else
				{
					printf("active recv:%s\n", buf);
				}

			}

		}
	}
	if (again < 0)
	{
		printf("send recv is error,so link again!\n");
		destroyFTP();
		setupFTP(host, ftp_name, ftp_pwd, ftp_path);
		return 0;
	}
	return 1;
}
int againConnect()
{
	printf("again Ftp!\n");
	destroyFTP();
	setupFTP(host, ftp_name, ftp_pwd, ftp_path);
	return fd;
}
int  ftp(char *data, int size, char fileOut_O[])
{
	int ret = -1;
	/*int fd = ftp_connect(host, PORT, ftp_name, ftp_pwd);*/
	if (fd < 0)
	{
		printf("fd is error,please ftp of fd!\n");
		return -1;
	}
//	if (ftp_active() == 0)
//		return ret;
	char *listdat;
	unsigned long long listlen;
	/*printf("ls %s\n", ftp_path);*/
	ret = ftp_list_n(fd, (char*) "."/*ftp_path*/, (void **) &listdat, &listlen);
	if (0 != ret)
	{
		printf("ftp_list_n:ret=%d", ret);
		return -1;
	}
	time_t timep;
	struct tm *p;
	struct timeval tval;
	gettimeofday(&tval, NULL);
	time(&timep);
	p = localtime(&timep); /*取得当地时间*/
	char fileOut[50];
	sprintf(fileOut, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d%ld.jpg", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tval.tv_usec);
	char dict[50];
	sprintf(dict, "%4.4d%2.2d%2.2d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
	/*char mkd[100];*/
	/*sprintf(mkd, "%s/%s", ftp_path, dict);*/
	if (NULL == strstr(listdat, dict))
	{
		//  ftp_cdup(fd);
		printf("list:%s,", listdat);
		ret = ftp_mkd(fd,/* mkd*/dict);
	}
	if (NULL != listdat)
		free(listdat);
	if (0 != ret)
	{
		printf("mkdir is error!ret=%d,", ret);
		return -1;
	}
	ret = ftp_cwd(fd, dict);
	printf("cd:ret=%d\n", ret);
	if (0 != ret)
	{
		printf("cwd is error,please check ret\n");
		return -1;
	}
	/*ftp_cwd(fd, mkd);*/
	if (0 !=  (ret = ftp_storefile_data(fd, data, fileOut, size)))
	{

		printf("ftp_storefile_data:ret=%d", ret);
		return -1;
	}
	if ( 0 != (ret = ftp_cdup(fd)))
	{
		printf("cd ..,failed .again link:ret=%d\n", ret);
		return -1;
	}
	/*ret = ftp_quit(fd);*/
	sprintf(fileOut_O, "/%s/%s/%s", ftp_path, dict, fileOut);
	return ret;
}
void destroyFTP()
{
	if (fd > 0)
	{
		int ret = ftp_quit(fd);
		printf("destroy:ret=%d\n", ret);
	}
	fd = -1;
}



int cliopen(char *host, int port)
{
	int control_sock;
	struct hostent *ht = NULL;
	control_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (control_sock < 0)
	{
		printf("socket error\n");
		return -1;
	}
	ht = gethostbyname(host);
	if (!ht)
	{
		printf("host by name failed\n");
		return -1;
	}

	memset(&servaddr, 0, sizeof(struct sockaddr_in));
	memcpy(&servaddr.sin_addr.s_addr, ht->h_addr, ht->h_length);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);

	if (connect(control_sock, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) == -1)
	{
		printf("connect ftp failed!\n");
		return -1;
	}
	return control_sock;
}


int s(char *str, char *s2)
{
	return sscanf(str, " get %s", s2) == 1;

}


int st(char *str, char *s1)
{
	return sscanf(str, " put %s", s1) == 1;
}


int strtosrv(char *str)
{
	int addr[6];
	sscanf(str, "%*[^(](%d,%d,%d,%d,%d,%d)", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
	bzero(host, strlen(host));
	sprintf(host, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	int port = addr[4] * 256 + addr[5];
	return port;
}


void ftp_list(int sockfd)
{
	int nread;
	for (;;)
	{
		if ((nread = recv(sockfd, rbuf1, MAXBUF, 0)) < 0)
		{
			printf("recv error\n");
		}
		else if (nread == 0)
		{
			break;
		}
		if (write(STDOUT_FILENO, rbuf1, nread) != nread)
			printf("send error to stdout\n");
	}
	if (close(sockfd) < 0)
		printf("close error\n");
}


int ftp_get(int sck, char *pDownloadFileName)
{
	int handle = open(pDownloadFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE);
	int nread;
	printf("%d\n", handle);
	for (;;)
	{
		if ((nread = recv(sck, rbuf1, MAXBUF, 0)) < 0)
		{
			printf("receive error\n");
		}
		else if (nread == 0)
		{
			printf("over\n");
			break;
		}
		if (write(handle, rbuf1, nread) != nread)
			printf("receive error from server!");
		if (write(STDOUT_FILENO, rbuf1, nread) != nread)
			printf("receive error from server!");
	}
	if (close(sck) < 0)
		printf("close error\n");
	return 0;
}


int ftp_put(int sck, char *data, int size)
{
	if (sck < 0)
	{
		printf("socket open failed!\n");
		return -1;
	}
	int nread = 0;
	int i = 0;
	// int sum=strlen(data);
	int sum = size;
	while (i < sum)
	{
		nread = (i + MAXBUFDATA <= sum) ? MAXBUFDATA : (sum - i);
		if (write(sck, &data[i], nread) != nread)
			printf("send error nread is not equal send!\n");
		i += nread;
	}
	if (close(sck) < 0)
		printf("close error\n");
	return 0;
}



//void cmd_tcp(int sockfd)
int cmd_tcp(int sockfd, char *data, int size, char fileOut[] )
{
	int maxfdp1, nread, nwrite, replycode, tag = 0, data_sock;
	replycode = 0;
	//int port;
	//   char *pathname;
	fd_set rset;
	FD_ZERO(&rset);
	maxfdp1 = sockfd + 1;
	//int index=0;
	for (;;)
	{
		if (1)
		{
			nwrite = 0;
			nread = 0;
			bzero(wbuf, MAXBUF);         //zero
			bzero(rbuf1, MAXBUF);
			nwrite = nread + 5;
			// printf("%dddf,%d\n",nread,replycode);
			if (replycode == USERNAME)
			{
				// int lenName=strlen(name);
				// nwrite+=lenName;
				// strcpy(rbuf1,name);

				// printf("asdf\n");
				//    strcpy(rbuf1,"xyz\n");
				//rbuf1[3]=;
				sprintf(wbuf, "USER %s\n", ftp_name);
				nwrite = 5 + strnlen(ftp_name, CHAR20) + 1;
				//  nwrite+=3;
				if (write(sockfd, wbuf, nwrite) != nwrite)
				{
					printf("write error\n");
				}
				//   printf("wbuf:%s\n",wbuf);
				//memset(rbuf1,0,sizeof(rbuf1));
				//memset(wbuf,0,sizeof(wbuf));
				//printf("1:%s\n",wbuf);
			}


			if (replycode == PASSWORD)
			{

				//strcpy(rbuf1,"1\n");
				// nwrite+=2;
				// printf("%s\n",rbuf1);
				// int lenPwd=strlen(pwd);
				//  nwrite+=lenPwd;
				// strcpy(rbuf1,pwd);a
				nwrite = 5 + strnlen(ftp_pwd, CHAR20) + 1;
				sprintf(wbuf, "PASS %s\n", ftp_pwd);
				if (write(sockfd, wbuf, nwrite) != nwrite)
					printf("write error\n");
				//bzero(rbuf,sizeof(rbuf));
				// printf("%s\n",wbuf);
				//printf("2:%s\n",wbuf);
			}
			if (replycode == LOGIN)
			{

				// strcpy(rbuf1,"put\n");
				strcpy(rbuf1, "cwd\n");
			}
			if (replycode == ACTIONOK)
			{
				strcpy(rbuf1, "put\n");
			}
			if (replycode == CLOSEDATA)
			{
				printf("quit is ready\n");
				strcpy(rbuf1, "quit\n");
			}
			if (replycode == 550 || replycode == LOGIN || replycode == CLOSEDATA || replycode == PATHNAME || replycode == ACTIONOK)
			{
				if (strncmp(rbuf1, "pwd", 3) == 0)
				{
					//printf("%s\n",rbuf1);
					sprintf(wbuf, "%s", "PWD\n");
					write(sockfd, wbuf, 4);
					// continue;
				}
				if (strncmp(rbuf1, "quit", 4) == 0)
				{
					sprintf(wbuf, "%s", "QUIT\n");
					write(sockfd, wbuf, 5);
					//close(sockfd);
					if (close(sockfd) < 0)
						printf("close error\n");
					break;
				}
				if (strncmp(rbuf1, "cwd", 3) == 0)
				{
					//sprintf(wbuf,"%s","PASV\n");
					sprintf(wbuf, "CWD %s\n", ftp_path);
					write(sockfd, wbuf, strnlen(wbuf, 50));

					//sprintf(wbuf1,"%s","CWD\n");

					// continue;
				}
				if (strncmp(rbuf1, "ls", 2) == 0)
				{
					tag = 2;
					//printf("%s\n",rbuf1);
					sprintf(wbuf, "%s", "PASV\n");
					//printf("%s\n",wbuf);
					write(sockfd, wbuf, 5);
					//read
					//sprintf(wbuf1,"%s","LIST -al\n");
					nwrite = 0;
					//write(sockfd,wbuf1,nwrite);
					//ftp_list(sockfd);
					//  continue;
				}
				if (strncmp(rbuf1, "get", 3) == 0)
				{
					tag = 1;
					sprintf(wbuf, "%s", "PASV\n");
					//printf("%s\n",s(rbuf1));
					//char filename[100];
					s(rbuf1, fileOut);
					printf("%s\n", fileOut);
					write(sockfd, wbuf, 5);
					//  continue;
				}
				if (strncmp(rbuf1, "put", 3) == 0)
				{
					tag = 3;
					sprintf(wbuf, "%s", "PASV\n");
					// st(rbuf1,filename);
					write(sockfd, wbuf, 5);
					//     continue;
				}
			}
			/*if(close(sockfd) <0)
			  printf("close error\n");*/
		}

		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		// printf("seb\n");
		int ret = select(maxfdp1, &rset, NULL, NULL, NULL);
		// printf("sed\n");
		//int ret=select(maxfdp1,&rset,NULL,NULL,NULL);
		if (ret < 0)
		{
			printf("select error\n");
		}
		if (FD_ISSET(sockfd, &rset))
		{
			bzero(rbuf, strlen(rbuf));
			if ((nread = recv(sockfd, rbuf, MAXBUF, 0)) < 0)
				printf("recv error\n");
			else if (nread == 0)
				break;

			if (strncmp(rbuf, "220", 3) == 0 || strncmp(rbuf, "530", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				strcat(rbuf, "your name:");
				//printf("%s\n",rbuf);
				nread += 12;
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = USERNAME;
			}
			if (strncmp(rbuf, "331", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n")*/;
				strcat(rbuf, "your password:");
				nread += 16;
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = PASSWORD;
			}
			if (strncmp(rbuf, "230", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = LOGIN;
			}
			if (strncmp(rbuf, "257", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = PATHNAME;
			}
			if (strncmp(rbuf, "226", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = CLOSEDATA;
			}
			if (strncmp(rbuf, "250", 3) == 0)
			{
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				replycode = ACTIONOK;
			}
			if (strncmp(rbuf, "550", 3) == 0)
			{
				replycode = 550;
			}
			/*if(strncmp(rbuf,"150",3) == 0)
			  {
			  if(write(STDOUT_FILENO,rbuf,nread) != nread)
			  printf("write error to stdout\n");
			  }*/
			//fprintf(stderr,"%d\n",1);
			if (strncmp(rbuf, "227", 3) == 0)
			{
				printf("227%d\n", 13);
				/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
				  printf("write error to stdout\n");*/
				int port1 = strtosrv(rbuf);
				printf("%d\n", port1);
				printf("%s\n", host);
				data_sock = cliopen(host, port1);



				//bzero(rbuf,sizeof(rbuf));
				//printf("%d\n",fd);
				//if(strncmp(rbuf1,"ls",2) == 0)
				if (tag == 2)
				{
					write(sockfd, "list\n", strlen("list\n"));
					ftp_list(data_sock);
					/*if(write(STDOUT_FILENO,rbuf,nread) != nread)
					  printf("write error to stdout\n");*/

				}
				//else if(strncmp(rbuf1,"get",3) == 0)
				else if (tag == 1)
				{
					//sprintf(wbuf,"%s","RETR\n");
					//printf("%s\n",wbuf);
					//int str = strlen(filename);
					//printf("%d\n",str);

					sprintf(wbuf, "RETR %s\n", fileOut);
					printf("%s\n", wbuf);
					//int p = 5 + str + 1;


					//printf("%d\n",write(sockfd,wbuf,strlen(wbuf)));
					write(sockfd, wbuf, strlen(wbuf));
					//printf("%d\n",p);
					ftp_get(data_sock, fileOut);
				}
				else if (tag == 3)
				{
					time_t timep;
					struct tm *p;
					struct timeval tval;
					gettimeofday(&tval, NULL);
					time(&timep);
					p = localtime(&timep); /*取得当地时间*/
					sprintf(fileOut, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d%ld.jpg", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tval.tv_usec);
					//  printf("%s\n",fileOut);
					sprintf(wbuf, "STOR %s\n", fileOut);
					printf("%s\n", wbuf);
					write(sockfd, wbuf, strlen(wbuf));
					ftp_put(data_sock, data, size);
					// sprintf(fileOut,"%s/%s",ftp_path,fileOut);
				}
				nwrite = 0;
			}
			/*if(strncmp(rbuf,"150",3) == 0)
			  {
			  if(write(STDOUT_FILENO,rbuf,nread) != nread)
			  printf("write error to stdout\n");
			  }*/
			//printf("%s\n",rbuf);
			//  if(write(STDOUT_FILENO,rbuf,nread) != nread)
			//    printf("write error to stdout\n");
			/*else
			  printf("%d\n",-1);*/
		}
	}
	printf("cmd_tcp finish!\n");
	return 1;
}
