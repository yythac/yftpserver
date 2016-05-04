package com.yythac.yftpserver;


public class YFtpController {
	
	public static final char READFILE = 0x1;
	public static final char WRITEFILE = 0x2;
	public static final char DELETEFILE = 0x4;
	public static final char LIST = 0x8;
	public static final char CREATEDIR = 0x10;
	public static final char DELETEDIR = 0x20;
	
	public static final char FTP_USER_PRIV_READ=(READFILE|LIST);
	public static final char FTP_USER_PRIV_WRITE=(WRITEFILE|CREATEDIR);
	public static final char FTP_USER_PRIV_DEL=(DELETEFILE|DELETEDIR);
	public static final char FTP_USER_PRIV_ALL=(FTP_USER_PRIV_READ|FTP_USER_PRIV_WRITE|FTP_USER_PRIV_DEL);

	
	public native String  stringFromJNI();

	public native boolean StartFtpServer();

	public native boolean StopFtpServer();

	public native boolean InitFtpServer(Boolean bDoAllow, String szusername,String szpassword
			,String szStartPath, char ucPriv,int port);
	
	public YFtpController(){};
	
	static {
		try {
			// 此处即为本地方法所在链接库名
			System.loadLibrary("yftpserver-jni");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Cannot load yftpserver library:\n "
					+ e.toString());
		}
	}
}
