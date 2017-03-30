MgJni

如果您在某些嵌入式设备或移动设备（Android，Openwrt，Raspberry Pi ）想嵌入一个HttpServer/HttpsServer。由于空间或平台限制又不能打入一个完整web环境如（jetty，tomcat）那本项目可以帮助您。
本项目使用了开源工程mongoose 的代码。https://github.com/cesanta/mongoose
使用它实现了一个标准的HttpServer/HttpsServer 它支持TLS。结合一定的其他手段（如DNS处理一下）可以通过App Transport Security（Apple ATS）的残酷考验。
如果你想写代码通过此项目与java层交互的话，也是可以支持的，它支持一种简单的形式。
http://127.0.0.1:18080/java/?data=1234

data的数据不要超过2048个字节。

这些数据会回调到TinyServer.java	protected String callback(String data) 这个方法中。

当然您需要返回一个字符串让TinyServer回调给调用者。

具体内部制定什么协议及实现由您自己来定了。
