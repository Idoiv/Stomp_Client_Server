package bgu.spl.net.impl.stomp;
import java.io.IOException;
import java.util.concurrent.ConcurrentHashMap;


import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class ConnectionsImpl<T> implements Connections<T> {
  
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> connections; // key: client unique id, value: connection handler   
 
    private ConcurrentHashMap<Integer, Boolean> activeConnections; // key: connection Id, value: is connected 
    private Database db;

    public ConnectionsImpl() {
        connections = new ConcurrentHashMap<>();
        activeConnections = new ConcurrentHashMap<>();
        db = new Database();
    }

    
   

   
    @Override
    public boolean send(int connectionId, T msg) {
        synchronized(this){
        ConnectionHandler<T> connection = connections.get(connectionId);
        if (connection == null) {
            return false;
        }
        connection.send(msg);
        return true;
    }
        }

    @Override
    public void send(String channel, T msg) {
         synchronized(channel){
        for (Integer x :db.getTopicsClients(channel)) {
            ConnectionHandler<T> connection = connections.get(x);
            connection.send(msg);
        }
    }
    }

    @Override
    public void disconnect(int connectionId) {
        db.removeClient(connectionId);
        String userName=db.getUserName(connectionId); // get ActiveUser name
        if(!userName.equals(""))
          db.removeUser(userName);
        activeConnections.put(connectionId,false);
       connections.remove(connectionId);
    }

    public void tryConnection(int connectionId, ConnectionHandler<T> connection) {
        connections.put(connectionId, connection);
    }

    public boolean addActiveConnection(int connectionId,String userName, String passcode)
    {
       if(!db.addUser(userName, passcode,connectionId))
           return false;
        activeConnections.put(connectionId,true);
        return true;
    }

    public boolean isActiveConnection(int connectionId)
    {
        if(activeConnections.get(connectionId)==null ||activeConnections.get(connectionId)==false)
            return false;

        return true;
    }

    
    
    public Database getDataBase(){
             return db;
    }
  


}
