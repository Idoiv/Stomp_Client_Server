package bgu.spl.net.impl.stomp;

import java.util.concurrent.atomic.AtomicInteger;
import bgu.spl.net.srv.Connections;
import java.util.Set;
import java.util.Iterator;

public class StompMessagingProtocol<T> implements bgu.spl.net.api.StompMessagingProtocol<T> {
    private boolean shouldTerminate = false;
    private ConnectionsImpl<String> connectionsImpl;
    private int connectionId;
    private static AtomicInteger messageId=new AtomicInteger(0);

    /**
     * Used to initiate the current client protocol with it's personal connection ID and the connections implementation
     **/
    public void start(int connectionId, Connections<T> connections){
        this.connectionId=connectionId;
        this.connectionsImpl=(ConnectionsImpl)connections;
    }



    public void process(T nextMessage){
        String a =  (String) nextMessage;
        String[] parts = a.split("\n",-1);
        String command = parts[0];
        String receipt = getReceipt(parts);
        boolean succeed= false;

        
        if( !command.equals("CONNECT") && !connectionsImpl.isActiveConnection(connectionId)) // unconnected client sends illegal commands
            errorHandler( receipt,parts, "unrecognise client","you need to connect first",connectionId);
        else
        {
            switch (command) {
                case "CONNECT":
                    succeed=connect(parts, receipt);
                    if(succeed)
                    {String message= connectedMessage();
                        connectionsImpl.send(connectionId, message);
                    }
                    break;
                case "SEND":
                    succeed=send(parts, receipt);
                    break;
                case "SUBSCRIBE":
                    succeed=subscribe(parts, receipt);
                    break;
                case "UNSUBSCRIBE":
                    succeed=unSubscribe(parts, receipt);
                    break;
                case "DISCONNECT":
                    succeed=disconnect(parts, receipt);
                    break;
                 default:
                 errorHandler(receipt, parts, "No such command", "command doesnt exist",connectionId);
                 break;
            }
        }

        if(receipt!="" && succeed && !command.equals("DISCONNECT")) // send receipt if needed
            connectionsImpl.send(connectionId, receipt(receipt));
    }
    


    /**
     * @return true if the connection should be terminated
     */
    public boolean shouldTerminate(){
        return shouldTerminate;

    }
    
    public boolean connect(String [] msg,String receipt){ // check if already connected exception!!

        int i=1;
        int cnt=0;
        String userName="";
        String passcode="";

        if(connectionsImpl.isActiveConnection(connectionId))
        { errorHandler( receipt,msg, "already connected","The client is already logged in, log out before trying again",connectionId);
            return false;}

        while( i<msg.length){
            if(msg[i].isEmpty()){   // handle header
                i++;
                break;
            }
            int x = msg[i].indexOf(':');
            String header = msg[i].substring(0, x).trim();
            String headerVal= msg[i].substring(x+1).trim();
            System.out.println(header+":"+headerVal);
            if (header.equals("accept-version")) {
                if(!headerVal.equals("1.2")) {
                   errorHandler( receipt,msg, "Wrong accept-version","accept-version needs to be 1.2 ",connectionId);
                    return false;
                }
                cnt++;
            } else if (header.equals("host")) {
                if(!headerVal.equals("stomp.cs.bgu.ac.il")) {
                    errorHandler( receipt,msg, "Wrong host","host needs to be: stomp.cs.bgu.ac.il",connectionId);
                    return false;
                }
                cnt++;
            } else if (header.equals("login")) {
                userName=headerVal;
                cnt++;
            } else if (header.equals("passcode")) {
                passcode=headerVal;
                cnt++;
            }

            i++;
        }

        if(cnt<4){
           errorHandler( receipt,msg, "missing headers","you must send all the required headers",connectionId);
            return false;
        }
        if(!msg[i].isEmpty() ) // body frame not empty
        { errorHandler( receipt,msg, "body frame not empty","body frame needs to be empty",connectionId);
            return false;
        }

        if(passcode.isEmpty() || userName.isEmpty())
        {
            errorHandler( receipt,msg, "missing password/user name","password/user name must contain values",connectionId);
            return false;
        }
        if(connectionsImpl.getDataBase().isUserActive(userName)){
            {errorHandler( receipt,msg, "user is already connected","logout first",connectionId);
                return false;}
        }

        if (!connectionsImpl.addActiveConnection(connectionId, userName ,passcode))
        {errorHandler( receipt,msg, "cannot connect","your password is incorrect",connectionId);
            return false;}

        return true;

    }
    
    public boolean send(String [] msg,String receipt){

        String destination="";
        String body="";
        int i=1;
        int x=0 ;
        String header="";
        String headerVal="";

        while(i<msg.length){
            if(msg[i].isEmpty()){ //handle headers
                i++;
                break;
            }
            x = msg[i].indexOf(':');
            if(x>=0)
            {
                headerVal= msg[i].substring(x+1).trim();
                header = msg[i].substring(0, x).trim();

                if(header.equals("destination")){
                    destination=headerVal;
                }
            }
            i++;
        }
        while( i<msg.length-1) 
        { body=body+msg[i]+"\n"; i++;}

        if(!destination.isEmpty()){
            int subId = connectionsImpl.getDataBase().getSubId(destination,connectionId);
            if(subId==-1){
                errorHandler( receipt,msg, "you're not subscribed to this topic","topic: "+destination,connectionId);
                return false;
            }
            else{
                Set<Integer> clientsId =connectionsImpl.getDataBase().getTopicsClients(destination);  // get all clients subscribe to topic
                Iterator<Integer> it = clientsId.iterator();
                while(it.hasNext()){

                    int clientId= it.next();
                    subId = connectionsImpl.getDataBase().getSubId(destination,clientId); // for each client getting subId

                    if(subId!=-1){ // checking the subId is correct

                        String msgToSend= "MESSAGE\n" +"subscription:" + subId + "\n" + // 00 is general number that
                                "Message-id:" + messageId.getAndIncrement() + "\n" + "destination:" + destination+ "\n"  + "\n" + body ;


                        connectionsImpl.send(clientId,msgToSend); // sending message to the client

                    }
                }
                return true;
            }
        }

        else{
            errorHandler( receipt,msg, "no destination","topic: "+destination,connectionId);
            return false;
        }
    }
    

    public boolean subscribe(String [] msg,String receipt) {
        int i=1;
        int cnt=0;
        int id=-1;

        String destination="";


        while(i<msg.length ){ // checking headers
            if(msg[i].isEmpty()){
                i++;
                break;
            }
            int x = msg[i].indexOf(':');
            String header = msg[i].substring(0, x).trim();
            String headerVal= msg[i].substring(x+1).trim();
            switch (header) {

                case "id":
                    try {
                        id= Integer.parseInt(headerVal);

                    } catch (NumberFormatException e)  {
                        errorHandler( destination, msg, "id error", "id is not a valid number",connectionId);
                        return false;
                    }
                    if(connectionsImpl.getDataBase().clientContainId(connectionId,id))
                    {  errorHandler( destination, msg, "id error", "id already exists",cnt);
                        return false;
                    }
                    cnt++;
                    break;
                case "destination":
                    destination=headerVal;
                    cnt++;
                    if(destination.equals("")){
                        errorHandler( receipt, msg, "destinantion error", "destination is empty",connectionId);
                        return false;
                    }
                    break;
            }
            i++;
        }

        if(!msg[i].isEmpty()) //message body not empty
        {  errorHandler( receipt,msg, "body frame not empty","body frame needs to be empty",connectionId); return false;}

        if(cnt!=2) // doesnt include all headers
        {errorHandler( receipt,msg, "headers missing"," doesnt include all headers",connectionId);
            return false;}
        if(connectionsImpl.getDataBase().isSubscribedTo(destination, connectionId))
        {errorHandler( receipt,msg, "You cannot join this channel","You're already subscribed to this channel",connectionId);
            return false;}
        connectionsImpl.getDataBase().subscribe( destination,connectionId,id );

        return true;
    }
   
    public boolean unSubscribe(String [] msg,String receipt)  {
        int subId=-1;
        int i=1;
        int cnt=0;
        String header="";
        String headerVal="";
        while(i<msg.length ){ // checking headers
            if(msg[i].isEmpty()){
                i++;
                break;
            }
            int x = msg[i].indexOf(':');
            header = msg[i].substring(0, x).trim();
            headerVal= msg[i].substring(x+1).trim();

            switch (header){
                case "id":{ //check id correctness
                    try { Integer.parseInt(headerVal); }
                    catch (NumberFormatException e) {
                        errorHandler( receipt, msg, "id error", "id is not a valid number",connectionId);
                        return false;
                    }
                    subId=Integer.parseInt(headerVal);
                    cnt++;
                }

            }
            i++;
        }

        if(!msg[i].isEmpty()) //message body not empty
        {  errorHandler( receipt,msg, "body frame not empty","body frame needs to be empty",connectionId);
            return false;}

        if(cnt==0) // doesnt include id
        {errorHandler( receipt,msg, "id missing","msg needs to include the Id",connectionId);
            return false;}

        if(!connectionsImpl.getDataBase().isSubscribedTo(subId, connectionId)) // subscripiton error
        { errorHandler( receipt,msg, "subscription error","you are not subscribe to this channel ",connectionId);
            return false;
        }

        connectionsImpl.getDataBase().unsubscribe(subId,connectionId);
        return true;

    }
    
    public boolean disconnect(String[] msg,String receipt){
        if(receipt.isEmpty()){
            errorHandler(receipt, msg, "receipt id is missing", receipt,connectionId);
            return false;
        }
        if(msg.length<4 || msg[1].isEmpty() || !msg[2].isEmpty() ){
            errorHandler(receipt, msg, "wrong frame structue", receipt,connectionId);
            return false;}
        connectionsImpl.send(connectionId, receipt(receipt));
        shouldTerminate=true;
        connectionsImpl.disconnect(connectionId);
        return false;

    }
  
    public String getReceipt(String [] msg)
    {
        int i=1;
        while(i<msg.length){ // check each line separatly before the body message
            int x = msg[i].indexOf(':');
            if(x>=0){
                String header = msg[i].substring(0, x).trim();
                String headerVal= msg[i].substring(x+1).trim();
                if(header.equals("receipt"))
                    return headerVal;
            }
            i++;
        }
        return "";
    }

    public void errorHandler( String receiptId, String [] msg, String errorSummary, String errorDetails, int connectionId)
    {   String []errorMessage;
        boolean flg=receiptId.equals("");
        if(flg)
            errorMessage= new String[7+msg.length];
        else
            errorMessage= new String[8+msg.length];

        int i=0;
        errorMessage[i] = "ERROR";
        i++;
       if(!flg)
         {   errorMessage[i] = "receipt-id: " + receiptId; i++;}
        errorMessage[i] = "message: " + errorSummary; i++;
        errorMessage[i]= ""; i++;
        errorMessage[i]= "The message"; i++;
        errorMessage[i]= "-----"; i++;
        for(int j=0; j<msg.length;j++)
        {
            errorMessage[i]=msg[j];
            i++;
        }
        errorMessage[i]= "-----"; i++;
        errorMessage[i]=errorDetails; i++;
        

        String fnl = String.join(System.lineSeparator(), errorMessage); // combine the string arr to a hole string
        connectionsImpl.send(connectionId, fnl);
        shouldTerminate=true;
        connectionsImpl.disconnect(connectionId);        
    }


    public String receipt(String receiptId)
    {
        String []receiptMessage=new String[3];
        receiptMessage[0] = "RECEIPT";
        receiptMessage[1]= "receipt-id:" + receiptId;
        receiptMessage[2]="";
        String fnl = String.join(System.lineSeparator(), receiptMessage); // combine the string arr to a hole string
        return fnl;

    }


    public String connectedMessage()
    {
        String []connectedMessage=new String[3];
        connectedMessage[0] = "CONNECTED";
        connectedMessage[1]= "version:1.2";
        connectedMessage[2]="";
        String fnl = String.join(System.lineSeparator(), connectedMessage); // combine the string arr to a hole string
        return fnl;

    }
}