package com.yythac.yftpserver;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;

import com.yythac.yftpserver.R;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.DhcpInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity implements OnClickListener{
	
	public static final int FILE_RESULT_CODE = 1;
	
	YFtpController YFtpCtrl;
	
	Button btnFtpCtrl;
	Button btnBrow;
	EditText etListenPort;
	EditText etHostIP;
	EditText etRootDir;
	EditText etUser;
	EditText etPass;
	CheckBox cbDoAllow;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		YFtpCtrl = new YFtpController();
		
		etListenPort = (EditText)findViewById(R.id.etListenPort);
		etHostIP = (EditText)findViewById(R.id.etHostIP);
		etRootDir = (EditText)findViewById(R.id.etRootDir);
	
		etUser = (EditText)findViewById(R.id.etUserName);
		etPass = (EditText)findViewById(R.id.etPassword);
		
		cbDoAllow = (CheckBox)findViewById(R.id.cbAnonyAcess);
		
	//	etHostIP.setText("0.0.0.0");
		
		btnFtpCtrl = (Button) findViewById(R.id.btnFtpCtrl);
		btnFtpCtrl.setOnClickListener(this);
		
		btnBrow = (Button) findViewById(R.id.btnBrow);
		btnBrow.setOnClickListener(this);
		
	//	String tmp = YFtpCtrl.stringFromJNI();
	//	Toast Info = Toast.makeText(this, tmp, Toast.LENGTH_SHORT);
	//	Info.show();		
		try
		{
			SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
			
			etListenPort.setText(sharedPref.getString("FTP_PORT", "2121"));
			
			String strRootDir = sharedPref.getString("FTP_ROOT_DIR", "");
			if(strRootDir.isEmpty() == true)
			{
				strRootDir = Environment.getExternalStorageDirectory().getAbsolutePath();
			}
			etRootDir.setText(strRootDir);
			etUser.setText(sharedPref.getString("FTP_USER", ""));
			etPass.setText(sharedPref.getString("FTP_PASS", ""));
			
			cbDoAllow.setChecked(sharedPref.getBoolean("FTP_ALLOW", true));
			
			
			String hostip = new String("");
		
			hostip = this.getAddress();

/*			String ipaddress;
			Enumeration<NetworkInterface> netInterfaces = NetworkInterface.getNetworkInterfaces();
			while (netInterfaces.hasMoreElements())
			{
				
				NetworkInterface ni = netInterfaces.nextElement();
				
				if (ni.getName().toLowerCase().equals("eth0")== false 
						&& ni.getName().toLowerCase().equals("wlan0") == false)
					continue;
				if(ni.isLoopback() == true || ni.isVirtual() == true
						|| ni.isUp() == false)
					continue;
				
				Enumeration<InetAddress> ips = ni.getInetAddresses();
				while (ips.hasMoreElements())
				{
					InetAddress inetAddress = ips.nextElement();
				
					if (!inetAddress.isLoopbackAddress()) 
					{  
						ipaddress = inetAddress.getHostAddress();  
						if (!ipaddress.contains("::")) 
						{// ipV6�ĵ�ַ  
							if(hostip.equals("") == false)
								hostip += "\n";
							hostip += ipaddress;
						}
					}
					
					//etHostIP.append(ips.nextElement().getHostAddress() + "\n\n");
				}

			}
*/			
//			if(hostip.equals("") == true)
//				hostip = "0.0.0.0";
			etHostIP.setText(hostip);

		}
		catch (Exception e)
		{
			// TODO: handle exception
			Toast Info = Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT);
			Info.show();
		}

	}
	@Override
	public void onClick(View v)
	{

		switch (v.getId())
		{
			case R.id.btnFtpCtrl:
			{
				if(etRootDir.getText().toString().isEmpty() == true)
				{
					Toast Info = Toast.makeText(this, R.string.root_dir_empty, Toast.LENGTH_SHORT);
					Info.show();
					break;
				}
				if(etUser.getText().toString().isEmpty() == false 
						&& etPass.getText().toString().isEmpty() == true)
				{
					Toast Info = Toast.makeText(this, R.string.password_empty, Toast.LENGTH_SHORT);
					Info.show();
					break;		
				}
				if(cbDoAllow.isChecked() == false && etUser.getText().toString().isEmpty() == true)
				{
					Toast Info = Toast.makeText(this, R.string.user_empty, Toast.LENGTH_SHORT);
					Info.show();
					break;						
				}
				if(btnFtpCtrl.getText().toString().equals(new String(getString(R.string.ftp_start))))
				{
					//if(etHostIP.getText().toString().equals("") == true)
					{
						String HostIP = getAddress();
						etHostIP.setText(HostIP);
						if(HostIP.equals(""))
						{
							Toast Info = Toast.makeText(this, R.string.ftp_start_fail, Toast.LENGTH_SHORT);
							Info.show();
							break;
						}

					}
					
					YFtpCtrl.InitFtpServer(cbDoAllow.isChecked(),etUser.getText().toString(),etPass.getText().toString(),
							etRootDir.getText().toString(),(char)YFtpController.FTP_USER_PRIV_ALL,Integer.parseInt(etListenPort.getText().toString()));
					if(YFtpCtrl.StartFtpServer() == true)
					{
						SaveFtpSettings();
						btnFtpCtrl.setText(getString(R.string.ftp_stop));
					}
					else
					{
						Toast Info = Toast.makeText(this, R.string.ftp_start_fail, Toast.LENGTH_SHORT);
						Info.show();
					}
				}
				else
				{
					if(YFtpCtrl.StopFtpServer() == true)
						btnFtpCtrl.setText(getString(R.string.ftp_start));
				}
				break;
			}
			case R.id.btnBrow:
			{
/*
				String path = null;

				DirChooserDialog dlg = new DirChooserDialog(MainActivity.this,
						1, null, path);
				dlg.setTitle("Choose ftp server root directory");
				dlg.show();

				if(path != null)
				{
					etRootDir.setText(path);
				}
*/
				Intent intent = new Intent(MainActivity.this,MyFileManager.class);
				startActivityForResult(intent, FILE_RESULT_CODE);
				break;
			}
		}
	}
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(FILE_RESULT_CODE == requestCode){
			Bundle bundle = null;
			if(data!=null&&(bundle=data.getExtras())!=null){

				etRootDir.setText(bundle.getString("file"));
			}
		}
	}
	private String getAddress() {  
		WifiManager wifiManager = (WifiManager) this.getSystemService(Context.WIFI_SERVICE);  
		// �ж�wifi�Ƿ���  
		if (!wifiManager.isWifiEnabled()) 
		{  
			Toast Info = Toast.makeText(this, R.string.wifi_enable, Toast.LENGTH_SHORT);
			Info.show();
			return "";
//			wifiManager.setWifiEnabled(true);  
		}  
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();  
		//DhcpInfo info = wifiManager.getDhcpInfo();  
		int ipAddress = wifiInfo.getIpAddress();  
		 
		//int ii = info.ipAddress;  
		 return intToIp(ipAddress);  
		//return ipAddress + "    dhcp: " + ii;  
		  
	}  	
	private void SaveFtpSettings()
	{
		SharedPreferences sharedPref = getPreferences(Context.MODE_PRIVATE);
		SharedPreferences.Editor editor = sharedPref.edit();
		
		editor.putString("FTP_PORT", etListenPort.getText().toString());
		
		editor.putString("FTP_ROOT_DIR", etRootDir.getText().toString());
		editor.putString("FTP_USER", etUser.getText().toString());
		editor.putString("FTP_PASS", etPass.getText().toString());
		
		editor.putBoolean("FTP_ALLOW",cbDoAllow.isChecked());
		editor.commit();
	}
    private String intToIp(int i) {       
        
        return (i & 0xFF ) + "." +       
      ((i >> 8 ) & 0xFF) + "." +       
      ((i >> 16 ) & 0xFF) + "." +       
      ( i >> 24 & 0xFF) ;  
   }
}
