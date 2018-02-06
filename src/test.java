
package test.rsa;
import java.io.BufferedOutputStream;
import java.io.BufferedWriter;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.security.Key;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Base64;
import java.util.zip.CRC32;
import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.spec.SecretKeySpec;

public class test {
	/*
	private void decryptMessage(EncryptMessage encryptMessage, Channel channel)
			throws Exception{
		//	get aes key, decrypt this message
		AttributeKey<byte[]> aesAttr = AttributeKey.valueOf("AES");
		Key key = new SecretKeySpec(channel.attr(aesAttr).get(), "AES");
		Cipher cipher = Cipher.getInstance("AES/ECB/NoPadding");
		cipher.init(Cipher.DECRYPT_MODE, key);
		
		byte[] encryMess = encryptMessage.getMessage();
		if (encryMess == null) {
			return;
		}
		LOG.info("MESSAGE LENGTH : "+encryptMessage.getLength());
//		LOG.info("ENCRYPT MESSAGE : "+bytesToHexString(encryMess));
		String data = new String(cipher.doFinal(encryMess), "UTF-8");
		
		//	take decryption data in message queue
		Message message = new Message(channel, data.intern());
		String[] msgs = data.split("\\r\\n");
		String body = msgs[1];
		String[] crcs = msgs[2].split(":");
		long recvCRC32 = Long.valueOf(crcs[1].trim()).longValue();
		
		CRC32 crc = new CRC32();
		crc.update(body.getBytes("utf-8"));
		long crc32 =  crc.getValue();
		
		LOG.info(body);
		LOG.info("recvCRC32 : "+recvCRC32+"  crc32 : "+crc32);
		
		if (crc32 == recvCRC32) {
			LOG.info("add message ........................");
			MessageDispatcher.getDispatcher().pushRecvMess(message);
			LOG.info("add message end ........................");
		}
	}
	*/
	public static String bytesToHexString(byte[] src){   
	    StringBuilder stringBuilder = new StringBuilder("");   
	    if (src == null || src.length <= 0) {   
	        return null;   
	    }   
	    for (int i = 0; i < src.length; i++) {   
	        int v = src[i] & 0xFF;   
	        String hv = Integer.toHexString(v);   
	        if (hv.length() < 2) {   
	            stringBuilder.append(0);   
	        }   
	        stringBuilder.append(hv);   
	    }   
	    return stringBuilder.toString();   
	}   
/*
	private void aesPrivateKeyReqHandler(EncryptMessage encryptMessage, Channel channel) 
			throws Exception {
		//	generate aes key
		KeyGenerator aesGenerator = KeyGenerator.getInstance("AES");
		aesGenerator.init(128);
		byte[] aesKey = aesGenerator.generateKey().getEncoded();
		//	save aes key for decrypting message
		AttributeKey<byte[]> aesAttr = AttributeKey.valueOf("AES");
		channel.attr(aesAttr).set(aesKey);
		
		//	use rsa private key to encrypt aes key
		AttributeKey<byte[]> rsaAttr = AttributeKey.valueOf("RSA");
		byte[] rsaPrivateKey = channel.attr(rsaAttr).get();
		PrivateKey privateKey = KeyFactory.getInstance("RSA").generatePrivate(new PKCS8EncodedKeySpec(rsaPrivateKey));
		Cipher cipher = Cipher.getInstance("RSA");
		cipher.init(Cipher.ENCRYPT_MODE, privateKey);
		byte[] aseEncrypt = cipher.doFinal(aesKey);
		LOG.info(channel.remoteAddress()+" : AES length "+aesKey.length+" :"+bytesToHexString(aesKey));
		//	set length and message
		encryptMessage.setLength(aseEncrypt.length);
		encryptMessage.setMessage(aseEncrypt);
		encryptMessage.setVerify(CommonHandler.getInstance().messageChecksum(aseEncrypt));
		
		channel.writeAndFlush(encryptMessage);
	}
*/
	private void rsaPublicKeyReqHandler()
	{
		KeyPair keyPair;
		try {
			 DataOutputStream out=new DataOutputStream(new BufferedOutputStream(new FileOutputStream("output1")));
			keyPair = KeyPairGenerator.getInstance("RSA").generateKeyPair();
			byte[] rsaPublic = keyPair.getPublic().getEncoded();
			final Base64.Decoder decoder = Base64.getDecoder();
			final Base64.Encoder encoder = Base64.getEncoder();
			final String encodedText = encoder.encodeToString(rsaPublic);
			System.out.println(encodedText);
			System.out.println(encodedText.length());
			for(int i=0;i<rsaPublic.length;i++){
			System.out.print(rsaPublic[i]);
    		out.writeByte(rsaPublic[i]);
 
			System.out.print(' ');
			}
	   		out.close();
	           /* 写入Txt文件 */  
         /*   File writename = new File("./output.txt"); // 相对路径，如果没有则要建立一个新的output。txt文件  
            writename.createNewFile(); // 创建新文件  
            BufferedWriter out = new BufferedWriter(new FileWriter(writename));  
            */
            
           
           

     /*      out.write(rsaPublic,0 , rsaPublic.length);
           // out.writeBytes(new String(rsaPublic)); // \r\n即为换行  
            out.flush(); // 把缓存区内容压入文件  
            out.close(); // 最后记得关闭文件  */
		} catch (NoSuchAlgorithmException | IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		//	get rsa public key and set length and content 
		
		
		//encryptMessage.setLength(rsaPublic.length);
		//encryptMessage.setMessage(rsaPublic);
		//encryptMessage.setVerify(CommonHandler.getInstance().messageChecksum(rsaPublic));
		//LOG.info(channel.remoteAddress()+" : RSA length "+rsaPublic.length+" : "+bytesToHexString(rsaPublic));
		//	save rsa private key to channel attribute for encrypt ase key to client
		//AttributeKey<byte[]> rsaAttributeKey = AttributeKey.valueOf("RSA");
		//channel.attr(rsaAttributeKey).set(keyPair.getPrivate().getEncoded());
	
	//	channel.writeAndFlush(encryptMessage);
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		test t=new test();
		t.rsaPublicKeyReqHandler();
	}

}


