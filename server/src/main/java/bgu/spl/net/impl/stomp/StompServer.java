package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;


public class StompServer {

    public static void main(String[] args) {
       int port = Integer.parseInt(args[0]);
        String serv = args[1];
     if(serv.equals("tpc"))   // you can use any server... 
       Server.threadPerClient(
          port, //port
              () -> new StompMessagingProtocol<String>(), //protocol factory
                StompMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
      else if(serv.equals("reactor"))
         Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                port, //port
                () -> new StompMessagingProtocol<String>(), //protocol factory
                StompMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
    }
}
