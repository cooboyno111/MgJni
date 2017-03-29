public class TinyServer {
static {
System.loadLibrary("TinyServer");
}
	
	/**
	 * 初始化方法
	 * @param port1 https监听端口
	 * @param port2 http监听端口
	 * @return 1
	 */
	public native int Init(String port1,String port2);
	/**
	 * 设置Token
	 * @param cert_path 证书目录
	 * @param key 密钥
	 */
	public native void SetSSl(String cert_path, String key);
	
	/**
	 * 开启服务端
	 * @return
	 */
	public native int Start();
	
	/**
	 * 关闭服务端
	 */
	public native void Close();
	
        /**
	 * 执行循环
	 */
	public native void Loop(int time);
        /**
         * callback
        */
	protected String callback(String data) {
		System.out.println(data);
System.out.println(data.length());
return "javacallback";
	}
}
