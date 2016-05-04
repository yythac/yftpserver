/**
 * 
 */
package com.yythac.yftpserver;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.view.Gravity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * @author HKP
 * 2011-6-17
 *
 */
public class DirChooserDialog extends Dialog implements android.view.View.OnClickListener{
	
	private ListView list;
	ArrayAdapter<String> Adapter;
	ArrayList<String> arr=new ArrayList<String>();
	
	Context context;
	private String path;
	
	private TextView title;
	private EditText et;
	private Button home,back,ok;
	private LinearLayout titleView;
	
	private int type = 1;
	private String[] fileType = null;
	
	public final static int TypeOpen = 1;
	public final static int TypeSave = 2;
	
	/**
	 * @param context
	 * @param type 值为1表示创建打开目录类型的对话框，2为创建保存文件到目录类型的对话框
	 * @param fileType 要过滤的文件类型,null表示只选择目录
	 * @param resultPath 点OK按钮返回的结果，目录或者目录+文件名
	 */
	public DirChooserDialog(Context context,int type,String[]fileType,String resultPath) {
		super(context);
		// TODO Auto-generated constructor stub
		this.context = context;
		this.type = type;
		this.fileType = fileType;
		this.path = resultPath;
	}
	/* (non-Javadoc)
	 * @see android.app.Dialog#dismiss()
	 */
	@Override
	public void dismiss() {
		// TODO Auto-generated method stub
		super.dismiss();
	}
	/* (non-Javadoc)
	 * @see android.app.Dialog#onCreate(android.os.Bundle)
	 */
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.chooserdialog);
		
		path = getRootDir();
		arr = (ArrayList<String>) getDirs(path);
		Adapter = new ArrayAdapter<String>(context,android.R.layout.simple_list_item_1, arr);
		
		list = (ListView)findViewById(R.id.list_dir);
		list.setAdapter(Adapter);
		
		list.setOnItemClickListener(lvLis);

		home = (Button) findViewById(R.id.btn_home);
		home.setOnClickListener(this);
		
		back = (Button) findViewById(R.id.btn_back);
		back.setOnClickListener(this);
		
		ok = (Button) findViewById(R.id.btn_ok);
		ok.setOnClickListener(this);
		
		titleView = (LinearLayout) findViewById(R.id.dir_layout);
		
		if(type == TypeOpen){
			title = new TextView(context);
			titleView.addView(title);
			title.setText(path);
		}else if(type == TypeSave){
			et = new EditText(context);
			et.setWidth(240);
			et.setHeight(70);
			et.setGravity(Gravity.CENTER);
			et.setPadding(0, 2, 0, 0);
			titleView.addView(et);
			et.setText("wfFileName");
		}
//		title = (TextView) findViewById(R.id.dir_str);
//		title.setText(path);
		
	}

	Runnable add=new Runnable(){

		@Override
		public void run() {
			// TODO Auto-generated method stub
			arr.clear();
//System.out.println("Runnable path:"+path);


			List<String> temp = getDirs(path);
			for(int i = 0;i < temp.size();i++)
				arr.add(temp.get(i));
			Adapter.notifyDataSetChanged();
		}   	
    };
   
    private OnItemClickListener lvLis=new OnItemClickListener(){
		@Override
		public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
				long arg3) {
			String temp = (String) arg0.getItemAtPosition(arg2);
//System.out.println("OnItemClick path1:"+path);			
			if(temp.equals(".."))
				path = getSubDir(path);
			else if(path.equals("/"))
				path = path+temp;
			else
				path = path+"/"+temp;
			
//System.out.println("OnItemClick path2"+path);	
			if(type == TypeOpen)
				title.setText(path);
			
			Handler handler=new Handler();
	    	handler.post(add);
		}
    };
	
	private List<String> getDirs(String ipath){
		List<String> file = new ArrayList<String>();
//System.out.println("GetDirs path:"+ipath);		
		File[] myFile = new File(ipath).listFiles();
		if(myFile == null){
			file.add("..");
			
		}else
			for(File f: myFile){

				if(f.isDirectory()){
					String tempf = f.toString();
					int pos = tempf.lastIndexOf("/");
					String subTemp = tempf.substring(pos+1, tempf.length());
//					String subTemp = tempf.substring(path.length(),tempf.length());
					file.add(subTemp);	
//System.out.println("files in dir:"+subTemp);
				}

				if(f.isFile() && fileType != null){
					for(int i = 0;i< fileType.length;i++){
						int typeStrLen = fileType[i].length();
						
						String fileName = f.getPath().substring(f.getPath().length()- typeStrLen);
						if (fileName.toLowerCase().equals(fileType[i])) {
							file.add(f.toString().substring(path.length()+1,f.toString().length()));	
						}
					}
				}
			}
		
		if(file.size()==0)
			file.add("..");
		
//		System.out.println("file[0]:"+file.get(0)+" File size:"+file.size());
		return file;
	}
	/* (non-Javadoc)
	 * @see android.view.View.OnClickListener#onClick(android.view.View)
	 */
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		if(v.getId() == home.getId()){
			path = getRootDir();
			if(type == TypeOpen)
				title.setText(path);			
			Handler handler=new Handler();
	    	handler.post(add);
		}else if(v.getId() == back.getId()){
			path = getSubDir(path);
			if(type == TypeOpen)
				title.setText(path);			
			Handler handler=new Handler();
	    	handler.post(add);
		}else if(v.getId() == ok.getId()){
			dismiss();
			if(type == TypeSave)
				path = path+"/"+et.getEditableText().toString()+".wf";
			Toast.makeText(context, path, Toast.LENGTH_SHORT).show();
	//		this.notify();
		}
			
		
	}
	
	private String getSDPath(){ 
	       File sdDir = null; 
	       boolean sdCardExist = Environment.getExternalStorageState()   
	                           .equals(android.os.Environment.MEDIA_MOUNTED);   
	       if(sdCardExist)   
	       {                               
	         sdDir = Environment.getExternalStorageDirectory();
	      }   
	       if(sdDir == null){
//Toast.makeText(context, "No SDCard inside!",Toast.LENGTH_SHORT).show();
	    	   return null;
	       }
	       return sdDir.toString(); 
	       
	} 
	
	private String getRootDir(){
		String root = "/";
		
		path = getSDPath();
		if (path == null)
			path="/";
		
		return root;
	}
	
	private String getSubDir(String path){
		String subpath = null;
		
		int pos = path.lastIndexOf("/");
		
		if(pos == path.length()){
			path = path.substring(0,path.length()-1);
			pos = path.lastIndexOf("/");
		}
		
		subpath = path.substring(0,pos);
		
		if(pos == 0)
			subpath = path;
		
		return subpath;
	}
}
