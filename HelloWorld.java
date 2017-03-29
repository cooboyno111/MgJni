import java.util.*;

public class HelloWorld
{

     public static void main(String argv[])
     {
         new HelloWorld();
     }
	public static void printarray(String s, byte[] ar) {
		System.out.println(s + " len:" + ar.length);
		for (int i = 0; i < ar.length; i++) {
			System.out.printf("%x ", ar[i]);
		}
		System.out.println();
	}
     public HelloWorld()
     {
         
        TinyServer s = new TinyServer();
        s.Init("18043","18080");
        s.SetSSl("server.pem", "server.key");
	s.Start();
        while(true)
 	{
          s.Loop(100);
        }
     }
}
