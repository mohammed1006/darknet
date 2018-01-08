//rsa_test.cpp  用于测试rsa和aes功能代码  
//sudo apt-get install openssl
//g++ rsa_test.c  -I/usr/include/openssl  -lpthread -lm  -lcrypto -g -o rsa_test 

#include <openssl/rsa.h>  
#include <openssl/aes.h>
#include <openssl/err.h>  
#include <openssl/pem.h>  
  
#include <iostream>  
#include <string>  
#include <cstring>  
#include <cassert>  


using namespace std;  
std::string strPemFileName;


//读取base64字符串转为Rsa公钥
bool strConvert2PublicKey( const std::string& strPublicKey, RSA* pRSAPublicKey )  
{
	int nPublicKeyLen = strPublicKey.size();      //strPublicKey为base64编码的公钥字符串
	for(int i = 64; i < nPublicKeyLen; i+=64)
	{
		if(strPublicKey[i] != '\n')
		{
			strPublicKey.insert(i, "\n");
		}
		i++;
	}
	strPublicKey.insert(0, "-----BEGIN PUBLIC KEY-----\n");
	strPublicKey.append("\n-----END PUBLIC KEY-----\n");

	BIO *bio = NULL; 
	RSA *rsa = NULL; 
	char *chPublicKey = const_cast<char *>(strPublicKey.c_str());
	if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
	{     
		    cout<<"BIO_new_mem_buf failed!"<<endl;      
	}       
	rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
	if (!rsa)
	{
		    ERR_load_crypto_strings();
		    char errBuf[512];
		    ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		    cout<< "load public key failed["<<errBuf<<"]"<<endl;
			BIO_free_all(bio);
	}
}



//加密  
std::string EncodeRSAKeyFile( const std::string& strPemFileName, const std::string& strData )  
{  
    if (strPemFileName.empty() || strData.empty())  
    {  
        assert(false);  
        return "";  
    }  
    FILE* hPubKeyFile = fopen(strPemFileName.c_str(), "rb");  
    if( hPubKeyFile == NULL )  
    {  
        assert(false);  
        return "";   
    }  
    std::string strRet;  
    RSA* pRSAPublicKey = RSA_new();  
    if(PEM_read_RSA_PUBKEY(hPubKeyFile, &pRSAPublicKey, 0, 0) == NULL)  
    {  
        assert(false);  
        return "";  
    }  
  
    int nLen = RSA_size(pRSAPublicKey);  
    char* pEncode = new char[nLen + 1];  
    int ret = RSA_public_encrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
    {  
        strRet = std::string(pEncode, ret);  
    }  
    delete[] pEncode;  
    RSA_free(pRSAPublicKey);  
    fclose(hPubKeyFile);  
    CRYPTO_cleanup_all_ex_data();   
    return strRet;  
}  
  
//解密  
std::string DecodeRSAKeyFile( const std::string& strPemFileName, const std::string& strData )  
{  
    if (strPemFileName.empty() || strData.empty())  
    {  
        assert(false);  
        return "";  
    }  
    FILE* hPriKeyFile = fopen(strPemFileName.c_str(),"rb");  
    if( hPriKeyFile == NULL )  
    {  
        assert(false);  
        return "";  
    }  
    std::string strRet;  
    RSA* pRSAPriKey = RSA_new();  
    if(PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)  
    {  
        assert(false);  
        return "";  
    }  
    int nLen = RSA_size(pRSAPriKey);  
    char* pDecode = new char[nLen+1];  
  
    int ret = RSA_private_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING);  
    if(ret >= 0)  
    {  
        strRet = std::string((char*)pDecode, ret);  
    }  
    delete [] pDecode;  
    RSA_free(pRSAPriKey);  
    fclose(hPriKeyFile);  
    CRYPTO_cleanup_all_ex_data();   
    return strRet;  
}  
  


//AES_加密
std::string EncodeAES( const std::string& password, const std::string& data )
{
    AES_KEY aes_key;
    cout << "password_length " << password.length() <<endl;     

    if(AES_set_encrypt_key((const unsigned char*)password.c_str(), password.length() * 8, &aes_key) < 0)
    {
        assert(false);
        return "";
    }

    std::string strRet;
    std::string data_bak = data;
    unsigned int data_length = data_bak.length();
    int padding = 0;
    if (data_bak.length() % AES_BLOCK_SIZE > 0)
    {
        padding =  AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
    }
    data_length += padding;
    while (padding > 0)
    {
        data_bak += '\0';
        padding--;
    }
    for(unsigned int i = 0; i < data_length/AES_BLOCK_SIZE; i++)
    {
        std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        ::memset(out, 0, AES_BLOCK_SIZE);
        AES_encrypt((const unsigned char*)str16.c_str(), out, &aes_key);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }
    return strRet;
}


//AES解密
std::string DecodeAES( const std::string& strPassword, const std::string& strData )
{
	AES_KEY aes_key;
	if(AES_set_decrypt_key((const unsigned char*)strPassword.c_str(), strPassword.length() * 8, &aes_key) < 0)
	{
		assert(false);
		return "";
	}
	std::string strRet;
	for(unsigned int i = 0; i < strData.length()/AES_BLOCK_SIZE; i++)
	{
		std::string str16 = strData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		unsigned char out[AES_BLOCK_SIZE];
		::memset(out, 0, AES_BLOCK_SIZE);
		AES_decrypt((const unsigned char*)str16.c_str(), out, &aes_key);
		strRet += std::string((const char*)out, AES_BLOCK_SIZE);
	}
	return strRet;
}
uint8_t chk_xrl(const char *data, int length)  
{  
  const uint8_t *buf = data;  
  uint8_t retval = 0;  
  
  while(length)  
  {  
    retval ^= *buf++;  
    --length;  
  }  
  return retval;  
}  
std::string getASE(int fd)
{
    if(fd<0)
        return NULL;
    char rsa_public[9]={0};
    rsa_public[0]=0x7E;
    rsa_public[1]=0x7E;
    rsa_public[2]=0x01;
    rsa_public[8]=0x0D;
    int len_en=send(sockfd,rsa_public,9,0);
    char rsa_rec[1000];
    recv(sockfd,rsa_rec,1000,0);
    int len_en1=(int)rsa_rec[3]*16*16*16;
    int len_en2=(int)rsa_rec[4]*16*16;
    int len_en3=(int)rsa_rec[5]*16;
    int len_en4=len_en1+len_en2+len_en3+rsa_rec[6];
 
    std::string public_RSA(&rsa_rec[7],len_en4);
  //  strConvert2PublicKey(public_RSA,);
    std::string pRSAPublicKey;
    rsa_public[2]=0x02;
    len_en=send(sockfd,rsa_public,9,0);
    recv(sockfd,rsa_rec,1000,0);
    int len_en1=(int)rsa_rec[3]*16*16*16;
    int len_en2=(int)rsa_rec[4]*16*16;
    int len_en3=(int)rsa_rec[5]*16;
    std::string public_RSA_ASE(&rsa_rec[7],len_en4);
    strPemFileName=DecodeRSAKeyFile( public_RSA, public_RSA_ASE);
    return strPemFileName;
}
int send_en(int fd,char* content,int len_char,int param)
{
     if(fd<0)
        return NULL;
    char* send_content[1000];
    send_content[0]=0x7E;
    send_content[1]=0x7E;
    send_content[2]=0x03;

    std::string en_cont=EncodeAES( strPemFileName, content );
    int len=en_cont.size();
    send_content[3+0]  = len >> 24;     
    send_content[3+1] = len >> 16;
    send_content[3+2] = len >> 8;
    send_content[3+3] = len;
    memcpy(&send_content[7],&en_cont.str[0],len);
    send_content[len+7]=(char)chk_xrl(&send_content[7],len);
    send_content[len+8]=0x0D;
    return send(fd,send_content,len+9,0);
}
int recv_en(int fd,char* content,int& len,int param)
{
    if(fd<0)
        return NULL;
    char* rec_content[1000];
    int len=recv_en(fd,rec_content,1000,param)
    int len_c=0;
    len_c=rec_content[6];
    len_c+=rec_content[5]<<8;
    len_c+=rec_content[4]<<16;
    len_c+=rec_content[3]<<24;
    std::string cont_str(&rec_content[7],len_c);
    char chk=(char)chk_xrl(&rec_content[7],len_c);
    if(send_content[len+7]!=chk)
    {
        printf("recevie data sum fail\n");
        return -1;
    }
    std::string rec_cont=DecodeAES( strPemFileName, rec_content );
    len=rec_cont.size();
    memset(content,rec_cont.str(),len);
    return len;
}

int maintt()  
{  

    /*
    //原文  
    //AES加密，块大小必须为128位（16字节），如果不是，则要补齐，密钥长度可以选择128位、192位、256位。
    const string one = "1234567812345678";
    cout << "one: " << one << endl;  
  
    //密文（二进制数据）  
    string two = EncodeRSAKeyFile("public.pem", one);  
    cout << "two: " << two << endl;  
  
    //顺利的话，解密后的文字和原文是一致的  
    string three = DecodeRSAKeyFile("privkey.pem", two);  
    cout << "three: " << three << endl;  
    */
 

    const string three = "1234567812345678"; //要加密的内容 
    const string four = "@@wufsfadfyuan@"; //要加密的内容 
	string five = EncodeAES(three, four);
    cout << "five: " << five << endl; 

	string six = DecodeAES(three, five);
    cout << "six: " << six << endl; 

    return 0;  
}



/*
#define OPENSSLKEY "test.key"
#define PUBLICKEY "test_pub.key"
#define BUFFSIZE 1024
char* my_encrypt(char *str,char *path_key);//加密
char* my_decrypt(char *str,char *path_key);//解密
int mainaa(void){
    char *source="i like dancing !";
    char *ptr_en,*ptr_de;
    printf("source is    :%s\n",source);
    ptr_en=my_encrypt(source,PUBLICKEY);
    printf("after encrypt:%s\n",ptr_en);
    ptr_de=my_decrypt(ptr_en,OPENSSLKEY);
    printf("after decrypt:%s\n",ptr_de);
    if(ptr_en!=NULL){
        free(ptr_en);
    }   
    if(ptr_de!=NULL){
        free(ptr_de);
    }   
    return 0;
}
char *my_encrypt(char *str,char *path_key){
    char *p_en;
    RSA *p_rsa;
    FILE *file;
    int flen,rsa_len;
    if((file=fopen(path_key,"r"))==NULL){
        perror("open key file error");
        return NULL;    
    }   
    if((p_rsa=PEM_read_RSA_PUBKEY(file,NULL,NULL,NULL))==NULL){
    //if((p_rsa=PEM_read_RSAPublicKey(file,NULL,NULL,NULL))==NULL){   换成这句死活通不过，无论是否将公钥分离源文件
        ERR_print_errors_fp(stdout);
        return NULL;
    }   
    flen=strlen(str);
    rsa_len=RSA_size(p_rsa);
    p_en=(unsigned char *)malloc(rsa_len+1);
    memset(p_en,0,rsa_len+1);
    if(RSA_public_encrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_en,p_rsa,RSA_NO_PADDING)<0){
        return NULL;
    }
    RSA_free(p_rsa);
    fclose(file);
    return p_en;
}
char *my_decrypt(char *str,char *path_key){
    char *p_de;
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if((file=fopen(path_key,"r"))==NULL){
        perror("open key file error");
        return NULL;
    }
    if((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL){
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    rsa_len=RSA_size(p_rsa);
    p_de=(unsigned char *)malloc(rsa_len+1);
    memset(p_de,0,rsa_len+1);
    if(RSA_private_decrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_de,p_rsa,RSA_NO_PADDING)<0){
        return NULL;
    }
    RSA_free(p_rsa);
    fclose(file);
    return p_de;
}*/