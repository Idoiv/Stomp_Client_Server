package bgu.spl.net.impl.stomp;

import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;



public class Database {
    private ConcurrentHashMap<Integer,ConcurrentHashMap<Integer,String>> subscriptions;
    private ConcurrentHashMap<String, Set<Integer>> topicToConnectionId; // key: topic, value: ids of all clients subscribed
    private ConcurrentHashMap<String, String> userToPassword;
    private ConcurrentHashMap<String, Integer> ActiveUsers; // key: user name, value: password  
    public Database(){
        topicToConnectionId = new ConcurrentHashMap<>();
        userToPassword = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
        ActiveUsers = new ConcurrentHashMap<>();
    }
      
    public void subscribe(String channel, int connectionId, int subId) {
        if(!topicToConnectionId.containsKey(channel)){
            topicToConnectionId.put(channel, new HashSet<>());
        }
        topicToConnectionId.get(channel).add(connectionId);
        if(!subscriptions.containsKey(connectionId)){
            subscriptions.put(connectionId, new ConcurrentHashMap<>());
        }
        subscriptions.get(connectionId).put(subId, channel);
    }
    public void unsubscribe(int subId,int connectionId) {
        String topic = subscriptions.get(connectionId).remove(subId);
        if(topic != null){
            Set<Integer> connectionIds = topicToConnectionId.get(topic);
            if(connectionIds != null){
                connectionIds.remove(connectionId);
                if(connectionIds.isEmpty())
                topicToConnectionId.remove(topic);
            }
        }
    }



    public boolean isSubscribedTo(String channel,int connectionId) {
        Set<Integer> connectionIds = topicToConnectionId.get(channel);
        if (connectionIds != null) {
            return connectionIds.contains(connectionId);
        }
        return false;
    }
    public boolean isSubscribedTo(int subId, int connectionId) {
        ConcurrentHashMap<Integer, String> subs = subscriptions.get(connectionId);
        if (subs != null) {
            return subs.containsKey(subId);
        }
        return false;
    }

    public boolean clientContainId(int connectionId, int subId) {
        ConcurrentHashMap<Integer, String> subs = subscriptions.get(connectionId);
        if (subs != null) {
            return subs.containsKey(subId);
        }
        return false;
    }

    public int getSubId(String topic,int connectionId ) {
        ConcurrentHashMap<Integer, String> subs = subscriptions.get(connectionId);
        if (subs != null) {
            for(Map.Entry<Integer,String> sub: subs.entrySet()){
                if(sub.getValue().equals(topic)){
                    return sub.getKey();
                }
            }
        }
        return -1;
   }

    public Set<Integer> getTopicsClients(String topic) {
        if(topicToConnectionId.containsKey(topic)) {
            return topicToConnectionId.get(topic);
        }
        return null;
    }
    public void removeClient(int connectionId){
        Set<String> tempTopics = topicToConnectionId.keySet();   /// get all the topics
        for (String c : tempTopics) {
            if(isSubscribedTo(c, connectionId)){   // check if the client subscribed 
                topicToConnectionId.get(c).remove(connectionId);
                if(topicToConnectionId.get(c).isEmpty()){
                    topicToConnectionId.remove(c);
                }
            }
        }
        if(subscriptions.get(connectionId)!=null)
           subscriptions.remove(connectionId);
    }

   public boolean isUserActive(String userName){
      if(ActiveUsers.get(userName)==null)
           return false;

      return true;
   }
   public boolean addUser(String login, String passcode, int connectionId){
    if(userToPassword.get(login)!=null){
        if(userToPassword.get(login).equals(passcode)){
            ActiveUsers.put(login,connectionId);
              return true;
        }
        else{
            return false;
        }
    }
    userToPassword.put(login, passcode);
    ActiveUsers.put(login, connectionId);
    return true;
 }
  public void removeUser(String userName){
           ActiveUsers.remove(userName);
}
  public String getUserName(int connectionId){
   for(String x :ActiveUsers.keySet()){
     if(ActiveUsers.get(x)==connectionId)
        return x;
}
return "";
}
}
