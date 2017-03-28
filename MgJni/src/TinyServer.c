#include <jni.h>

#include "TinyServer.h"
#include "mongoose.h"
JavaVM *gvm = NULL;
jobject jobj;
static const char *document_root=".";
//HTTPS server 
static  struct mg_mgr mgr;
static  struct mg_connection *nc;
static const char *s_http_port = "8443";
static const char *s_ssl_cert = "server.pem";
static const char *s_ssl_key = "server.key";
static struct mg_serve_http_opts s_http_server_opts;
//HTTP server
static  struct mg_mgr mgr2;
static  struct mg_connection *nc2;
static const char *s_http_port2 = "8000";
static struct mg_serve_http_opts s_http_server_opts2;
void printca(const char *array,int len);
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data);
int initHttpsServer();
int initHttpServer();
void setHttpsServer();
void setHttpServer();
static void handle_java(struct mg_connection *nc, struct http_message *hm);
JNIEnv* getJNIEnv(int* needsDetach);
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) ;
//-----------------------------------------以下为C相关代码-------------------------------------
void printca(const char *array,int len)
{
   printf("len=%d\n",len);
   for (int i = 0; i < len; i++)
   {
 	printf("%c",array[i]);
   }
   printf("\n");
}
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
	struct http_message *hm = (struct http_message *) ev_data;
	if (ev == MG_EV_HTTP_REQUEST) {
		printf("requested...");
		if (mg_vcmp(&hm->uri, "/java/") == 0) {
		   handle_java(nc, hm);
		}else{
		   mg_serve_http(nc, (struct http_message *) ev_data, s_http_server_opts);
                }
	}
}
int initHttpsServer()
{
  struct mg_bind_opts bind_opts;
  const char *err;
  //init HTTPS server
  mg_mgr_init(&mgr, NULL);
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.ssl_cert = s_ssl_cert;
  bind_opts.ssl_key = s_ssl_key;
  bind_opts.error_string = &err;
  printf("Starting SSL server on port %s, cert from %s, key from %s\n",
         s_http_port, bind_opts.ssl_cert, bind_opts.ssl_key);
  nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
  if (nc == NULL) {
    printf("Failed to create listener: %s\n", err);
    return 1;
  }else
  {
    return 0;
  }
}
int initHttpServer()
{
  //init HTTP server
  mg_mgr_init(&mgr2, NULL);
  printf("Starting web server on port %s\n", s_http_port2);
  nc2 = mg_bind(&mgr2, s_http_port2, ev_handler);
  if (nc2 == NULL) {
    printf("Failed to create listener\n");
    return 1;
  }else
  {
    return 0;
  }
}
void setHttpsServer()
{
  // Set up HTTPS server parameters
  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = document_root;  // Serve current directory
  s_http_server_opts.enable_directory_listing = "yes";
}
void setHttpServer()
{
  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc2);
  s_http_server_opts2.document_root = document_root;  // Serve current directory
  s_http_server_opts2.enable_directory_listing = "yes";
}
static void handle_java(struct mg_connection *nc, struct http_message *hm) {
//初始化字符串
char data[2049];
memset(data, 0, sizeof(data));
JNIEnv *env;
jclass jcls;
jmethodID jmid;
int needsDetach=0;
env = getJNIEnv(&needsDetach);
jcls = (*env)->GetObjectClass(env, jobj);
if (jcls == NULL) {
	printf("FindClass() Error.....");
	return;
}else
{
	printf("FindClass() OK.....");
}
//jmid = (*env)->GetMethodID(env, jcls, "callback", "(Ljava/lang/String;)Ljava/lang/String");
jmid = (*env)->GetMethodID(env, jcls, "callback", "(Ljava/lang/String;)Ljava/lang/String;");
if (jmid == NULL) {
	printf("GetMethodID() Error.....");
	return;
}else
{
	printf("GetMethodID() OK.....");
}
printf("\n");
//获取输入值
mg_get_http_var(&hm->query_string, "data", data, sizeof(data));
//printca(&data,strlen(data));
jstring jstr = (*env)->NewStringUTF(env, data);
jstring jjstr = (*env)->CallObjectMethod(env, jobj, jmid, jstr);
char* t=(char*)(*env)->GetStringUTFChars(env,jjstr,0);  
//int len=(*env)->GetStringUTFLength(env,jjstr);
//printca(t,len);
mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
mg_printf_http_chunk(nc, "{\"data\": [");
mg_printf_http_chunk(nc,t);
mg_printf_http_chunk(nc,"]}\n");
mg_send_http_chunk(nc, "", 0);
}
//-----------------------------------------以下为JNI相关代码-------------------------------------
JNIEnv* getJNIEnv(int* needsDetach){
    JNIEnv* env = NULL;
    if ((*gvm)->GetEnv(gvm, (void**) &env, JNI_VERSION_1_4) != JNI_OK){
        int status = (*gvm)->AttachCurrentThread(gvm, &env, 0);
        if (status < 0){
            printf("failed to attach current thread\n");
            return NULL;
        }
        *needsDetach = 1;
    }
    printf("GetEnv Success\n");
    return env;
}
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    printf("hello in c native code.\n");
    // 获取JNI版本
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_4) != JNI_OK) {
    	printf("GETEnv failed!\n");
    	return result;
    }else
    {
        printf("GETEnv OK!\n");
    }
    return JNI_VERSION_1_4;
}

/*
 * Class:     TinyServer
 * Method:    Init
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_TinyServer_Init
  (JNIEnv *env, jobject obj, jstring port1, jstring port2) {
        int len=0;
	s_http_port = (*env)->GetStringUTFChars(env, port1, 0);
	len=(*env)->GetStringUTFLength(env,port1);
        printca(s_http_port,len);
	s_http_port2 = (*env)->GetStringUTFChars(env, port2, 0);
	len=(*env)->GetStringUTFLength(env,port2);
        printca(s_http_port2,len);
	return 1;
}

/*
 * Class:     TinyServer
 * Method:    SetSSl
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_TinyServer_SetSSl
  (JNIEnv *env, jobject obj,jstring cert_path, jstring token) {
        int len=0;
	s_ssl_cert = (*env)->GetStringUTFChars(env, cert_path, 0);
	len=(*env)->GetStringUTFLength(env,cert_path);
        printca(s_ssl_cert,len);
	s_ssl_key = (*env)->GetStringUTFChars(env, token, 0);
	len=(*env)->GetStringUTFLength(env,token);
        printca(s_ssl_key,len);
}


/*
 * Class:     TinyServer
 * Method:    Start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_TinyServer_Start
  (JNIEnv *env, jobject obj) {
  (*env)->GetJavaVM(env, &gvm);
  jobj = (*env)->NewGlobalRef(env, obj);

  initHttpsServer();
  initHttpServer();
  setHttpsServer();
  setHttpServer();
  return 0;
}

/*
 * Class:     TinyServer
 * Method:    Destory
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_TinyServer_Close
  (JNIEnv *env, jobject obj) {
  mg_mgr_free(&mgr);
  mg_mgr_free(&mgr2);
}
/*
 * Class:     TinyServer
 * Method:    Loop
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_TinyServer_Loop
  (JNIEnv *pEnv, jobject obj, jint time)
{
/*
char *ret = "OKOKOOK";
 jmethodID mid  = NULL;
 jstring jjstr = NULL;  
 jclass cls = (*pEnv)->GetObjectClass(pEnv, obj);
// mid = (*pEnv)->GetMethodID(pEnv,cls,"fsum","(II)V");
 //(*pEnv)->CallVoidMethod(pEnv,obj,mid, 1, 2);
//mid = (*pEnv)->GetMethodID(pEnv,cls,"callback", "(Ljava/lang/String;)Ljava/lang/String;");
if (mid != NULL) {
printf("GetMethodID() OK.....\n");
jstring jstr = (*pEnv)->NewStringUTF(pEnv, ret);
jjstr = (*pEnv)->CallObjectMethod(pEnv,obj,mid, jstr);
char* t=(char*)(*pEnv)->GetStringUTFChars(pEnv,jjstr,0);  
int len=(*pEnv)->GetStringUTFLength(pEnv,jjstr);
printca(t,len);
}
*/
 mg_mgr_poll(&mgr, time);
 mg_mgr_poll(&mgr2, time);
}

