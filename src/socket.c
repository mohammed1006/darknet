#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define MAXLINE 4096
#include <cJSON.h>
#include "verifyutil.h"
int sockfd;
static char *robotIDg="";
static float threshg=0;
static char ipg[100];
static int portg;
int ftp(char *,int,char file[]);
void modiftFtp(const char *ip,const char *name,const char *pwd,const char *path);
void destroy();
void setupSocket(char *server,int port,char *robotID,float thresh);
int modifyIP(cJSON *pSub );
extern void modifyFtp(const char *ip,const char *name,const char *pwd,const char *path);
char *request(char param,char *url)
{
    cJSON *pJsonRoot = NULL;
    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "version", "1.0.0");
    cJSON_AddStringToObject(pJsonRoot, "messageid", "123456");
    cJSON_AddStringToObject(pJsonRoot, "mode", "request");
    cJSON_AddStringToObject(pJsonRoot, "robotid", robotIDg);
    cJSON_AddStringToObject(pJsonRoot, "targetID", "4");
    cJSON_AddStringToObject(pJsonRoot, "sourceID", "6");
    switch(param)
    {
    case 'a':
        cJSON_AddStringToObject(pJsonRoot, "orderType","pdaHeart" );
        break;
    case 'b':
        cJSON_AddStringToObject(pJsonRoot, "orderType","personDetect");
        break;
    case 'l':
        cJSON_AddStringToObject(pJsonRoot, "orderType","loginMainServer");
        break;
    }
    if(NULL!=url)
    {
        cJSON_AddStringToObject(pJsonRoot, "param", url);
    }
    char *p = cJSON_Print(pJsonRoot);
    cJSON_Delete(pJsonRoot);
    return p;
}

int modifyIP(cJSON *pSub )
{
    cJSON *pSubSubIp = cJSON_GetObjectItem(pSub, "serverip");
    cJSON *pSubSubPort = cJSON_GetObjectItem(pSub,"serverport");
    cJSON *pSubSubFtpip = cJSON_GetObjectItem(pSub,"ftpip");
    cJSON *pSubSubPortFtpuser = cJSON_GetObjectItem(pSub,"ftpusr");
    cJSON *pSubSubPortFtppwd = cJSON_GetObjectItem(pSub,"ftppwd");
    cJSON *pSubSubPortftpdath = cJSON_GetObjectItem(pSub,"ftpdpath");
    destroy();
    setupSocket(pSubSubIp->valuestring,pSubSubPort->valueint,robotIDg,threshg);
    modifyFtp(pSubSubFtpip->valuestring,pSubSubPortFtpuser->valuestring,pSubSubPortFtppwd->valuestring,pSubSubPortftpdath->valuestring);
    return 1;
}
char *getParam(char *pMsg)
{
    if(NULL == pMsg)
    {
        return NULL;
	}
    cJSON *pJson = cJSON_Parse(pMsg);
    cJSON *pSub = cJSON_GetObjectItem(pJson, "param");
    cJSON *orderType = cJSON_GetObjectItem(pJson,"orderType");
    if(strncmp(orderType->valuestring,"pdParam",7))
    {
        cJSON *pSubSub = cJSON_GetObjectItem(pSub, "thresh");
        int th = pSubSub->valueint;
        threshg =0.01* th;
    }
    else if(strncmp(orderType->valuestring,"serverAD",8))
    {
        modifyIP(pSub);
    }
    //cJSON_AddNumberToObject(pJson,"statusCode",1000);
    cJSON_AddStringToObject(pJson,"statusCode","1000");
    char *p = cJSON_Print(pJson);
    cJSON_Delete(pJson);
    return p;
}
void setupSocket(char *server,int port,char *robotID,float thresh)
{
    robotIDg=robotID;
    int rec_len;
    char    buf[MAXLINE];
    struct sockaddr_in    servaddr;
    threshg = thresh;
    strcpy(ipg,server);
    portg=port;
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if( inet_pton(AF_INET,server, &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n",server);
        return;
        // exit(0);
    }
    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        //  exit(0); a
        sockfd=-1;
        return;
    }
    printf("send msg to server: \n");
    //fgets(sendline, 4096, stdin);
    char *sendjson=request('l',NULL);
    int len = strlen(sendjson);
    char mess[/*messlen*/1000];
    long crc = Crc32((unsigned char *)sendjson,len);
    sprintf(mess,"ContentLength:%d\r\n%s\r\nCRC32Verify:%ld\\",len,sendjson,crc);
    // printf("sj finish\n");
    printf("send message:%s,%d\n",mess,(int)strlen(mess));
    int sendLen=send(sockfd, mess, strlen(mess), 0) ;
    if(  sendLen!=(int)strlen(mess))
    {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
    }
    free(sendjson);
    // printf("sj1 finish\n");
    /*if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1)
    {
        perror("recv error");
        printf("recv error setup socket fail");
    }
    //  buf[rec_len]  = '\0';*/
    printf("socket finish\n");
}
void sendData(char *data,int size,float thresh)
{
    printf("socket send!\n");
    if(-1==sockfd){
        destroy();
        setupSocket(ipg,portg,robotIDg,threshg);
		if(-1==sockfd)
				return;
	}
    static int index=0;
    char retData='a';
    char *url=NULL;
    char fileOut[100];
    printf("thresh:%f,",thresh);
    if(NULL!=data&&size>0&&thresh>=threshg)
    {
        printf("%f\n",threshg);
        ftp(data,size,fileOut);
        url=&fileOut[0];
        retData='b';
    }
       // return;
    printf("send end\n");
    //char urlChar[100];
    //sprintf(urlChar,"%s",url);
    char *sj=request(retData,url);
    int len = strlen(sj);
    char mess[1000];
    long crc = Crc32((unsigned char *)sj,len);
    sprintf(mess,"ContentLength:%d\r\n%s\r\nCRC32Verify:%ld\\",len,sj,crc);
    if( send(sockfd, mess, strlen(mess), MSG_NOSIGNAL) < 0)
    {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        printf("begin again link to server(%s,%d,%s,%f)\n",ipg,portg,robotIDg,threshg);
        //again setup
        destroy();
        setupSocket(ipg,portg,robotIDg,threshg);
    }
    free(sj);
    if(-1==sockfd)
			return;
    fd_set fdR;
    FD_ZERO(&fdR);
    FD_SET(sockfd, &fdR);
    struct timeval timeout;
    timeout.tv_usec=10;
    timeout.tv_sec=0;
    switch (select(sockfd + 1, &fdR, NULL,NULL, &timeout))
    {
    case -1:
        printf("selet error\n");
        break;
    case 0:
        break;
    default:
        if (FD_ISSET(sockfd,&fdR))
        {
            /*        char buf[MAXLINE];
                      if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {
                      perror("recv error");
            //  exit(1);
            }
            free(para);*/
        }

    }
    printf("socket finsh num:%d\n",index++);
}

void destroy()
{
    if(close(sockfd) < 0)
        printf("close error\n");
}
