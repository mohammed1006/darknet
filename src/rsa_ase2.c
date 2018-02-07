//rsa_test.cpp  用于测试rsa和aes功能代码
//sudo apt-get install openssl
//g++ rsa_test.c  -I/usr/include/openssl  -lpthread -lm  -lcrypto -g -o rsa_test

#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 1000
#define ADDR "192.168.0.249" //在本机测试用这个地址，如果连接其他电脑需要更换IP
#define SERVERPORT 65535

char strPemFileName[1000];
void write_read_rsac(char* writebuf, int len, char* namefile, int type)
{
	int fd = -1;        // fd 就是file descriptor，文件描述符
	//char buf[100] = {0};
//	char writebuf[20] = "l love linux";
	int ret = -1;

	// 第一步：打开文件
	fd = open(namefile, O_RDWR);//注意之前自己定义个a.txt
	if (-1 == fd)        // 有时候也写成： (fd < 0)
	{
		printf("文件打开错误\n");
	}
	else
	{
		printf("文件打开成功，fd = %d.\n", fd);
	}

	// 第二步：读写文件
	// 写文件
	if (type == 1)
	{
		ret = write(fd, writebuf, len);
	}
	else
	{
		ret = read(fd, writebuf, 1000);
	}
	if (ret < 0)
	{
		printf("write失败.\n");
	}
	else
	{
		printf("write成功，写入了%d个字符\n", ret);
	}
	close(fd);
};
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';
int Base64Decode(char * input, int length, char* result)
{
	/*{{{*/
	//tring result;
	static char decode[1024] = {0};
	if (NULL == input || length <= 0 || length >= 1024)
	{
		//  return result;
		return -1;
	}
	int len = EVP_DecodeBlock((unsigned char*)decode, (const unsigned char*)input, length);
	if (len >= 1024 || len <= 0)
	{
		//  return result;
		return -1;
	}
	decode[len] = '\0';

	//result.resize(len);
	result[len] = '\0';
	for (int i = 0; i < len; i++)
	{
		result[i] = decode[i];
	}
	return len;
}/*}}}*/
int Base64Encode(char * input, int length, char* result)
{
	/*{{{*/
	static char encoded[1024] = {0};
//	string result;
	if (NULL == input || length <= 0 || length >= 1024)
	{
//		return result;
		return -1;
	}

	int len = EVP_EncodeBlock((unsigned char*)encoded, (const unsigned char*)input, length);
	if (len >= 1024 || len <= 0)
	{
//		return result;
		return -1;
	}
	encoded[len] = '\0';
	result[len] = '\0';
//	result = string(encoded);
	for (int i = 0; i < len; i++)
	{
		result[i] = encoded[i];
	}
//	return result;
	return len;
}/*}}}*/
/*编码代码
* const unsigned char * sourcedata， 源数组
* char * base64 ，码字保存
*/
int base64_encode(const unsigned char * sourcedata, const int datalength, char * base64)
{
	printf("### base64 encode\n");
	/*{{{*/
	int i = 0, j = 0;
	unsigned char trans_index = 0;  // 索引是8位，但是高两位都为0
	//const int datalength = strlen((const char*)sourcedata);
	for (; i < datalength; i += 3)
	{
		// 每三个一组，进行编码
		// 要编码的数字的第一个
		trans_index = ((sourcedata[i] >> 2) & 0x3f);
		base64[j++] = base64char[(int)trans_index];
		// 第二个
		trans_index = ((sourcedata[i] << 4) & 0x30);
		if (i + 1 < datalength)
		{
			trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
			base64[j++] = base64char[(int)trans_index];
		}
		else
		{
			base64[j++] = base64char[(int)trans_index];

			base64[j++] = padding_char;

			base64[j++] = padding_char;

			break;   // 超出总长度，可以直接break
		}
		// 第三个
		trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
		if (i + 2 < datalength)  // 有的话需要编码2个
		{
			trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
			base64[j++] = base64char[(int)trans_index];

			trans_index = sourcedata[i + 2] & 0x3f;
			base64[j++] = base64char[(int)trans_index];
		}
		else
		{
			base64[j++] = base64char[(int)trans_index];

			base64[j++] = padding_char;

			break;
		}
	}

	base64[j] = '\0';

	return 0;
}/*}}}*/
//读取base64字符串转为Rsa公钥
//bool strConvert2PublicKey( const std::string& strPublicKey, RSA* pRSAPublicKey )
//int strConvert2PublicKey( char* strPublicKey, int nPublicKeyLen, RSA** pRSAPublicKey )
RSA* strConvert2PublicKey(char* strPublicKey, int nPublicKeyLen)
{

	//int nPublicKeyLen = strPublicKey.size();      //strPublicKey为base64编码的公钥字符串
	printf("strconvert2publickey ch%s\n", strPublicKey);
	int tempInt = 0;
	for (int i = 64; i < nPublicKeyLen; i += 64)
	{
		if (strPublicKey[i] != '\n')
		{
			//strPublicKey.insert(i, "\n");
			char temp[1000] = {0};
			for (int j = 0; j + i < nPublicKeyLen + tempInt; j++)
			{
				temp[j] = strPublicKey[i + j];
			}
			strPublicKey[i] = '\n';
			tempInt++;
			strPublicKey[i + 1] = 0;
			strcat(strPublicKey, temp);
		}
		i++;
	}
	printf("ch%s\n", strPublicKey);
	//strPublicKey.insert(0, "-----BEGIN PUBLIC KEY-----\n");
	char strPublicKeyB[1000] = "-----BEGIN PUBLIC KEY-----\n";
	strcat(strPublicKeyB, strPublicKey);
	//strPublicKey.append("\n-----END PUBLIC KEY-----\n");
	strcat(strPublicKeyB, "\n-----END PUBLIC KEY-----\n");
	BIO *bio = NULL;
	char *chPublicKey = strPublicKeyB;//const_cast<char *>(strPublicKey.c_str());
	printf("ch%s,%ddd\n", chPublicKey, strlen(chPublicKey));
	if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
	{
		// cout<<"BIO_new_mem_buf failed!"<<endl;
		printf("BIO_new_mem_buf failde!\n");
	}
	RSA* pRSAPublicKey = NULL;
	pRSAPublicKey = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);   //从bio结构中得到rsa结构
	if (!pRSAPublicKey)
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		//cout<< "load public key failed["<<errBuf<<"]"<<endl;
		printf("load public key failed[%s]\n", errBuf);
		BIO_free_all(bio);
		//return -1;

	}
	return pRSAPublicKey;
}

//利用 RSA公钥结构体  解密数据
//std::string RSAPubKeyDncodeData(RSA *pRSAPubKey,std::string& strEncoded )
int RSAPubKeyDncodeData(RSA* pRSAPubKey, char* strEncoded, int strELen, char* strRet)
{
	/*{{{*/
	if (!pRSAPubKey || !strEncoded)
	{
		// assert(false);
		printf("input data is empty\n");
		return -1;
	}

	int nLen = RSA_size((const RSA *)pRSAPubKey);
	//char* pDecode = new char[nLen + 1];
	char* pDecode = (char*)malloc(nLen + 1);
	//string strRet;
	memset(strRet, 0, 1000);
	//int ret = RSA_public_decrypt(strEncoded.length(), (const unsigned char*)strEncoded.c_str(), (unsigned char*)pDecode, pRSAPubKey, RSA_PKCS1_PADDING);
	int ret = RSA_public_decrypt(strELen, strEncoded, pDecode, pRSAPubKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		//  strRet = std::string((char*)pDecode, ret);
		memcpy(strRet, pDecode, ret);
	}
	free(pDecode);
//	delete [] pDecode;
	CRYPTO_cleanup_all_ex_data();
//	return strRet;
	return ret;
}/*}}}*/

//std::string EncodeRSAKeyFile( const std::string& strPemFileName, const std::string& strData )
//加密
int EncodeRSAKeyFile( const char* strPemFileName, const char* strData, int strDataLen, char* pEncode)
{
	/*{{{*/
	/* if (strPemFileName.empty() || strData.empty())
	 {
	     assert(false);
	     return "";
	 }  */
	FILE* hPubKeyFile = fopen(strPemFileName, "rb");
	if ( hPubKeyFile == NULL )
	{
		//assert(false);
		printf("fopen failed,check %s\n", strPemFileName);
		return -1;
	}
	//std::string strRet;
	RSA* pRSAPublicKey = RSA_new();
	if (PEM_read_RSA_PUBKEY(hPubKeyFile, &pRSAPublicKey, 0, 0) == NULL)
	{
		// assert(false);
		//  return "";
		printf("read_RSA_pubkey failed!\n");
		return -1;
	}

	int nLen = RSA_size(pRSAPublicKey);
	//char* pEncode = new char[nLen + 1];
	pEncode[nLen] = 0;
	int ret = RSA_public_encrypt(strDataLen, strData, pEncode, pRSAPublicKey, RSA_PKCS1_PADDING);
	/* if (ret >= 0)
	 {
	    // strRet = std::string(pEncode, ret);
	 }  */
	//delete[] pEncode;
	RSA_free(pRSAPublicKey);
	fclose(hPubKeyFile);
	CRYPTO_cleanup_all_ex_data();
	//  return strRet;
	return ret;
}/*}}}*/
// std::string DecodeRSAKeyFile( const std::string& strPemFileName, const std::string& strData )
//解密
int DecodeRSAKeyFile( const char* strPemFileName, const char* strData, int strDataLen, char* pDecode )
{
	/*{{{*/

	/* if (strPemFileName.empty() || strData.empty())
	 {
	     assert(false);
	     return "";
	 }  */
	//  FILE* hPriKeyFile = fopen(strPemFileName.c_str(),"rb");
	FILE* hPriKeyFile = fopen(strPemFileName, "rb");
	if ( hPriKeyFile == NULL )
	{
		//  assert(false);
		//  return "";
		printf("fopen failed,check %s\n", strPemFileName);
		return -1;
	}
	//std::string strRet;
	RSA* pRSAPriKey = RSA_new();
	if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)
	{
		// assert(false);
		//  return "";
		printf("PEM_read_RSAPrivateKey faild \n");
		return -1;
	}
	int nLen = RSA_size(pRSAPriKey);
	//char* pDecode = new char[nLen+1];
	pDecode[nLen] = 0;
//   int ret = RSA_private_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING);
	int ret = RSA_private_decrypt(strDataLen, (const unsigned char*)strData, (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING);
	/* if(ret >= 0)
	 {
	    // strRet = std::string((char*)pDecode, ret);

	 }  */
	//delete [] pDecode;
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	CRYPTO_cleanup_all_ex_data();
//	return strRet;
	return ret;
}/*}}}*/
//利用 Private.pem文件 加密数据
//std::string RSAPriKeyEncodeData(std::string& strPemFileName,std::string& strData )
int RSAPriKeyEncodeData(char* strPemFileName, char* strData, int strLen, char* strRet)
{
	/*{{{*/
	/*if (strPemFileName.empty() || strData.empty())
	{
	    assert(false);
	    return "";
	} */

	//FILE* hPriKeyFile = fopen(strPemFileName.c_str(), "rb");
	FILE* hPriKeyFile = fopen(strPemFileName, "rb");
	if ( hPriKeyFile == NULL )
	{
		// assert(false);
		return -1;//"";
	}
//	std::string strRet;
	RSA* pRSAPriKey = RSA_new();
	if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)
	{
		// assert(false);
		// return "";
		return -1;
	}

	int nLen = RSA_size(pRSAPriKey);
	char* pEncode = (char*)malloc(nLen + 1);
	//char* pEncode = new char[nLen + 1];
	//私钥加密
	int ret = RSA_private_encrypt(strLen, strData, pEncode, pRSAPriKey, RSA_PKCS1_PADDING);
	//int ret = RSA_private_encrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPriKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		//    strRet = std::string(pEncode, ret);
		memcpy(strRet, pEncode, ret);
	}
//	delete[] pEncode;
	free(pEncode);
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	CRYPTO_cleanup_all_ex_data();
//	return strRet;
	return ret;
}/*}}}*/
//读取pubkey.pem文件 转为Rsa公钥结构体
//bool PubKeyPemCvt2RSA(const std::string& pubkeyFile, RSA** pRSAPublicKey)
int PubKeyPemCvt2RSA(char* pubkeyFile, RSA** pRSAPublicKey)
{
	/*{{{*/

	//FILE* hpubKeyFile = fopen(pubkeyFile.c_str(),"rb");
	FILE* hpubKeyFile = fopen(pubkeyFile, "rb");
	int ret = 1;
	if ( hpubKeyFile == NULL )
	{
		// assert(false);
		//  return false;
		ret = -1;
	}
	//std::string strRet;
	if (PEM_read_RSA_PUBKEY(hpubKeyFile, pRSAPublicKey, 0, 0) == NULL)
	{
		//assert(false);
		//return  false;
		printf("read_RSA_Publickey failed!\n");
		ret = -1;
	}
	fclose(hpubKeyFile);
	return ret;//true;
}/*}}}*/

//AES_加std::string EncodeAES( const std::string& password, const std::string& data )密
int EncodeAES( const char* password, int passwordLen, const char* data, int dataLen, char* strRet)
{
	/*{{{*/
	memset(strRet, 0, 1000);
	AES_KEY aes_key;
	//cout << "password_length " << password.length() <<endl;

	if (AES_set_encrypt_key((const unsigned char*)password, passwordLen * 8, &aes_key) < 0)
	{
		//assert(false);
		//return "";
		printf("AES_SET_Encrypt_key fail %s(%d)\n", password, (int)strlen(password));
		return -1;
	}

	//std::string strRet;
	//std::string data_bak = data;
	//unsigned int data_length = data_bak.length();
	char* data_bak[1000] = {0};
	memcpy(data_bak, data, dataLen);
	unsigned int data_length = dataLen;
	int padding = 0;
	if (/*data_bak.length()*/data_length % AES_BLOCK_SIZE > 0)
	{
		// padding =  AES_BLOCK_SIZE - data_bak.length() % AES_BLOCK_SIZE;
		padding =  AES_BLOCK_SIZE - data_length % AES_BLOCK_SIZE;
	}
	data_length += padding;
	int data_temp = data_length;
	while (padding > 0)
	{
		//data_bak += '\0';
		data_bak[data_temp] = '\0';
		data_temp++;
		padding--;
	}

	for (unsigned int i = 0; i < data_length / AES_BLOCK_SIZE; i++)
	{
		//std::string str16 = data_bak.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		char* str16[1000] = {0};
		//strcpy(str16,&data_bak[i*AES_BLOCK_SIZE],AES_BLOCK_SIZE);
		for (int j = 0; j < AES_BLOCK_SIZE; j++)
		{
			str16[j] = data_bak[i * AES_BLOCK_SIZE + j];
		}
		unsigned char out[AES_BLOCK_SIZE];
		memset(out, 0, AES_BLOCK_SIZE);
		AES_encrypt((const unsigned char*)str16, out, &aes_key);
		//strRet += std::string((const char*)out, AES_BLOCK_SIZE);
		strcat(strRet, out);
	}
	// return strRet;
	return strlen(strRet);
}/*}}}*/


//AES解密
int DecodeAES( const char* strPassword, int strPasswordLen, const char* strData, int strDataLen, char* strRet)
{
	/*{{{*/
	memset(strRet, 0, 1000);
	AES_KEY aes_key;
	if (AES_set_decrypt_key((const unsigned char*)strPassword, strPasswordLen * 8, &aes_key) < 0)
	{
		//assert(false);
		//return "";
		printf("AES-set-decrypt-key failed\n");
		return -1;
	}
	//std::string strRet;
	//for(unsigned int i = 0; i < strData.length()/AES_BLOCK_SIZE; i++)
	for (int i = 0; i < strDataLen / AES_BLOCK_SIZE; i++)
	{
		//std::string str16 = strData.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		char* str16[1000];
		memcpy(str16, &strData[i * AES_BLOCK_SIZE], AES_BLOCK_SIZE);
		unsigned char out[AES_BLOCK_SIZE];
		memset(out, 0, AES_BLOCK_SIZE);
		AES_decrypt((const unsigned char*)str16, out, &aes_key);
		//strRet += std::string((const char*)out, AES_BLOCK_SIZE);
		strcat(strRet, out);
	}
	return strlen(strRet);
}/*}}}*/
char chk_xrl(const char *data, int length)
{
	/*{{{*/
	const char *buf = data;
	char retval = 0;

	while (length)
	{
		retval ^= *buf++;
		--length;
	}
	return retval;
}/*}}}*/
int getASE(int fd)
{
	printf("get ase fd %d\n", fd);
	if (fd < 0)
		return -1;
	char rsa_public[9] = {0};
	rsa_public[0] = 0x7E;
	rsa_public[1] = 0x7E;
	rsa_public[2] = 0x01;
	rsa_public[8] = 0x0D;
	int len_en = send(fd, rsa_public, 9, 0);
	char rsa_rec[1000];
	int rec_en = recv(fd, rsa_rec, 1000, 0);
	printf("get ase len1 %d\n", rec_en);



	int len_en1 = (((int)rsa_rec[3]+256)%256) * 16 * 16 * 16;
	int len_en2 = (((int)rsa_rec[4]+256)%256) * 16 * 16;
	int len_en3 = (((int)rsa_rec[5]+256)%256) * 16;
	int len_en4 = len_en1 + len_en2 + len_en3 + (((int)rsa_rec[6]+256)%256);

	//std::string public_RSA(&rsa_rec[7],len_en4);
	char public_RSA[1000] = {0};
	memcpy(public_RSA, &rsa_rec[7], len_en4);

	printf("get ase len %d\n", len_en4);
	static int index = 1;
	char nameff[100] = {0};
	sprintf(nameff, "%dftp.txt", index);
	printf("%snn\n", nameff);
	write_read_rsac(public_RSA, len_en4, nameff, 1);
	index++;




	char baseCh[1000];
	base64_encode(public_RSA, len_en4, (const unsigned char*) baseCh);
	RSA* rsa_p;
	rsa_p = strConvert2PublicKey(baseCh, strlen(baseCh));
	// std::string pRSAPublicKey;
	char pRSAPublicKey[1000];
	rsa_public[2] = 0x02;
	len_en = send(fd, rsa_public, 9, 0);
	rec_en = recv(fd, rsa_rec, 1000, 0);

	sprintf(nameff, "%dftp.txt", index++);
	printf("%snn\n", nameff);
	write_read_rsac(rsa_rec, rec_en, nameff, 1);

	len_en1 = (((int)rsa_rec[3]+256)%256) * 16 * 16 * 16;
	len_en2 = (((int)rsa_rec[4]+256)%256) * 16 * 16;
	len_en3 = (((int)rsa_rec[5]+256)%256) * 16;
	len_en4 = len_en1 + len_en2 + len_en3 + (((int)rsa_rec[6]+256)%256);
	//std::string public_RSA_ASE(&rsa_rec[7],len_en4);
//	char* public_RSA_ASE[1000];
	char public_ase[1000] = {0};
	memcpy(public_ase, &rsa_rec[7], len_en4);

	int ret = RSAPubKeyDncodeData(rsa_p, public_ase, strlen(public_ase), strPemFileName);

	printf("get ase key(%d):",ret);
	for (int i = 0; i < ret; i++)
	{
		printf("%d,", (int)strPemFileName[i]);
	}


	return ret;

}
int send_en(int fd, char* content, int len_char, int param)
{
	printf("#########res  send content:%s", content);
	if (fd < 0)
		return -1;
	char send_content[1000];
	send_content[0] = 0x7E;
	send_content[1] = 0x7E;
	send_content[2] = 0x03;

	char en_cont[1000];
	int len = EncodeAES(strPemFileName, 0, content, 0, en_cont);
	send_content[3 + 0]  = len >> 24;
	send_content[3 + 1] = len >> 16;
	send_content[3 + 2] = len >> 8;
	send_content[3 + 3] = len;
	memcpy(&send_content[7], en_cont, len);
	send_content[len + 7] = (char)chk_xrl(&send_content[7], len);
	send_content[len + 8] = 0x0D;


  printf("send len:%d,(%d,%d,%d,%d),c32:%d\n",len,(int)send_content[6],(int)send_content[5],(int)send_content[4],(int)send_content[3],(int)send_content[len+7]);


	printf("send enco:");
	for (int i = 0; i < len + 9; i++)
	{
		printf("%d,", (int)send_content[i]);
	}


	return send(fd, send_content, len + 9, param);
}
int recv_en(int fd, char* content, int rec_content_maxlen, int param)
{
  printf("####recv a message from server\n");
	if (fd < 0)
		return -1;
	char rec_content[1000];
	int len = recv(fd, rec_content, rec_content_maxlen,  param);
//-----------------debug----------//
	static int index = 0;
	char nameff[100] = {0};
	sprintf(nameff, "%dftp.txt", index++);
	write_read_rsac(rec_content, len, nameff, 1);

	printf("recv len(3-6):%d,%d,%d,%d.\n", (int)rec_content[3], (int)rec_content[4], (int)rec_content[5], (int)rec_content[6]);
//------------end----------------//

	int len_c = 0;
	len_c = rec_content[6];
	len_c += rec_content[5] << 8;
	len_c += rec_content[4] << 16;
	len_c += rec_content[3] << 24;
	//std::string cont_str(&rec_content[7],len_c);
	char chk = (char)chk_xrl(&rec_content[7], len_c);

  printf("recv len:%d,c32:%d\n",len_c,(int)chk);

  for(int i=0;i<len_c;i++)
	{
					printf("%x ",rec_content[7+i]);
	}

	if (rec_content[len_c + 7] != chk)
	{
		printf("recevie data sum fail\n");
		return -1;
	}
	char rec_cont[1000];
	len = DecodeAES( strPemFileName, 0, &rec_content[7], 0, rec_cont);
	memcpy(content, rec_cont, len);
	printf("recv deco:%s\n", content);
	return len;
}
int main4(int argc, char *argv[])
{
	int sock;
	char opmsg[BUF_SIZE];
	char get_msg[BUF_SIZE] = {0};
	int len;
	struct sockaddr_in serv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		return -1;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ADDR);  // 注释1
	serv_addr.sin_port = htons(SERVERPORT);
	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) // 注释2
	{
		printf("not connet\n");
		return -1;
	}
	else
	{
		printf("connetct ss\n");
	}
	getASE(sock);
	return 0;
	while (1)
	{
		//fgets(opmsg, BUF_SIZE, stdin);
		len = write(sock, opmsg, strlen(opmsg)); // 注释4
		printf("len:%d,c:%s\n", len, opmsg);
		if (!strcmp(opmsg, "q\n") || !strcmp(opmsg, "Q\n"))
		{
			puts("q pressed\n");
			break;
		}
		else
		{
			int read_msg = read(sock, get_msg, len);
			printf("len:%d,c:%s\n", read_msg, get_msg);
		}
	}
	close(sock);
	return 0;
}
int main3()
{










	char one[1000] = "1234567812345678";
	char one1[1000] = {0};
	int len = RSAPriKeyEncodeData("private_rsa.pem", one, strlen(one), one1);
//	printf("rrsa%d,%d,%s", len, (int)strlen(one1), one1);

	RSA* rsap;
//	int ret = PubKeyPemCvt2RSA("public_rsa.pem", &rsap);
	char szTest[1000] = {0};
	FILE *fp = fopen("1ftp.txt", "rb");
	if (NULL == fp)
	{
		printf("failed to open dos.txt\n");
		return 1;
	}
	int lenRead = 0;
	while (!feof(fp))
	{
		char temp[1000] = {0};
		int tempL = fread(&szTest[lenRead], sizeof(char), 1000, fp);
		lenRead += tempL;
	}
	fclose(fp);
	printf("\n");
	//printf("tt:%s,str(%d)\n", szTest, strlen(szTest));
	/*for (int i = 0; i <lenRead; i++)
	{
	    printf("%d ", (int)szTest[i]);
	}*/
	char szBase64[1000] = {0};

	base64_encode(szTest, lenRead, szBase64);
	printf("sz:%s\n", szBase64);
	int ret = 1;
	rsap = strConvert2PublicKey(szBase64, strlen(szBase64));

//	rsap = strConvert2PublicKey(szTest, strlen(szTest));
//	PubKeyPemCvt2RSA("public_rsa.pem",&rsap);
	if (!rsap)
		printf("asdf\n");


	FILE *fp2 = fopen("12ftp.txt", "rb");
	if (NULL == fp2)
	{
		printf("failed to open dos.txt\n");
		return 1;
	}
	int lenRead2 = 0;
	char szTest2[1000] = {0};
	while (!feof(fp2))
	{

		int tempL = fread(&szTest2[lenRead2], sizeof(char), 1000, fp2);
		lenRead2 += tempL;
	}
	fclose(fp2);


	char three[1000];
	len = RSAPubKeyDncodeData(rsap, /*one1*/szTest2, lenRead2/*strlen(one1)*/, three);
	RSA_free(rsap);
	printf("rs:%d,%d,%d,%s", len, (int) strlen(three), ret, three);
	for (int i = 0; i < len; i++)
	{
		printf("%d ", (int)three[i]);
	}
	/*  char three[1000] = "1234567812345678";
	    char four[1000] = "@@wufsfadfyuan@";
	    char out[1000] = {0};
	    char out2[1000] = {0};
	    int len = EncodeAES(three, 16, four, strlen(four), out);
	    printf("encode %d,%d,%s\n", len, strlen(out), out) ;
	    len =    DecodeAES(three, 16, out, strlen(out), out2);
	    printf("encode %d,%d,%s\n", len, strlen(out2), out2)   ;
	*/
	return 0;
}
