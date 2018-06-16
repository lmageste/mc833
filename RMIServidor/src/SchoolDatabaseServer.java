
import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.Date;

import com.mongodb.client.*;
import com.mongodb.MongoClient; 
import com.mongodb.client.MongoCollection;
import com.mongodb.client.model.Filters;
import com.mongodb.client.model.Projections;
import com.mongodb.client.model.Updates;

import org.bson.Document;
import org.bson.conversions.Bson; 
 
public class SchoolDatabaseServer
         implements SchoolDatabaseInterface {
	
		public static void main (String[] argv) {
		   try {
			   	SchoolDatabaseServer obj = new SchoolDatabaseServer();
	            SchoolDatabaseInterface stub = (SchoolDatabaseInterface) UnicastRemoteObject.exportObject(obj, 0);
	            //System.setProperty("java.rmi.server.hostname", "192.168.0.30");
	            
	            // Bind the remote object's stub in the registry
	            Registry registry = LocateRegistry.getRegistry();
	            registry.bind("SchoolServerSide", stub);
			   
			   /*
			   System.setSecurityManager(new RMISecurityManager());
			   System.setProperty("java.rmi.server.hostname", "192.168.0.30");
				
			   SchoolDatabase Hello = new SchoolDatabase();	
			   LocateRegistry.createRegistry(2020);
			   Naming.rebind("SchoolServerSide", Hello);
			   */

			   System.out.println("SchoolDatabase Server is ready.");
			   }catch (Exception e) {
				   System.out.println("SchoolDatabase Server failed: " + e);
				}
		   }
	
	
	
		MongoClient mongo;
		MongoDatabase database;
		
      public SchoolDatabaseServer () throws RemoteException {  
    	  // Creating a Mongo client 
          mongo = new MongoClient( "localhost" , 27017 ); 
          
          //Accessing the database 
          database = mongo.getDatabase("tcp");   
      }
      
      private MongoCollection<Document> getCollection(String collectionName){
    	  //Retrieving a collection
          MongoCollection<Document> collection = database.getCollection(collectionName);
          return collection;
      }
 
      public int add(int a, int b) throws RemoteException {
    	  int result=a+b;
    	  return result;
      }
      
      public String welcomingMessage() throws RemoteException{
    	  String msg = "Welcome to Uniluder's system!\nTo log in as a student, simply enter your ID.\nOtherwise, if you want to log in as a professor, enter your ID followed by your password.";
    	  return msg;
      }
	  public String[] logIn(String query) throws RemoteException{
		  String[] parts = query.split(" ");
		  MongoCollection<Document> collection;
		  Bson filter;
		  String collectionName;
		  if(parts.length==1) {
			  collectionName = "student";
			  collection = getCollection(collectionName);
			  filter = Filters.eq("_id", parts[0]);
			  
		  } else if(parts.length==2) {
			  collectionName = "professor";
			  collection = getCollection(collectionName);
			  filter = Filters.and(Filters.eq("_id", parts[0]), Filters.eq("password", parts[1]));
		  }
		  else
			  return new String[0];
		  
		  FindIterable<Document> iterDoc = collection.find(filter);
		  //if the filter returns any element, this means that the data entered was correct
		  if(iterDoc.iterator().hasNext()) {
			  String[] ans = {collectionName, parts[0]};
			  return ans;
		  }
		  else
			  return new String[0];
	  }
	  
	  public String showHelp(String userType) throws RemoteException{
		  StringBuilder ans = new StringBuilder();
		  ans.append("'help' = Displays all available commands\n");
		  ans.append("'content COURSE_CODE' = Displays content of given course\n");
		  ans.append("'content all' = Displays contents of all courses\n");
		  ans.append("'list COURSE_CODE' = Displays name of given course\n");
		  ans.append("'list all' = Displays names of all courses available\n");
		  ans.append("'detail COURSE_CODE' = Displays detailed information of given course\n");
		  ans.append("'detail all' = Displays detailed information of all courses available\n");
		  ans.append("'remark COURSE_CODE' = Displays professor's remarks on the course\n");
		  ans.append("'remark all' = Displays professors' remarks on all courses");
		  if(userType.equals("professor"))
			  ans.append("\n'write COURSE_CODE MESSAGE' = Adds a new remark to the subject in case the lecturer of this course is you");
		  
		  return ans.toString();
	  }
	  
	  private String parseIterDoc(FindIterable<Document> iterDoc) {
		  MongoCursor<Document> it = iterDoc.iterator();
		  
		  //if no elements, return 'no entries' message
		  if(it.hasNext()==false)
			  return "No entries found";
		  
		  //transform all docs to strings
		  StringBuilder ans = new StringBuilder();
		  while(it.hasNext() == true) {
			  ans.append(it.next().toJson());
		  }
		  return ans.toString();
	  }
	  
	  private String fetchContent(String which) {
		  FindIterable<Document> iterDoc;
		  Bson projections = Projections.fields(Projections.excludeId(), Projections.include("content"));
		  
		  if(!which.equals("all")) {
			  Bson filters = Filters.eq("_id", which);
			  iterDoc = getCollection("course").find(filters);
		  }
		  else
			  iterDoc = getCollection("course").find();
		  iterDoc  = iterDoc.projection(projections);
		  
		  return parseIterDoc(iterDoc);
	  }
	  
	  private String fetchList(String which) {
		  FindIterable<Document> iterDoc;
		  Bson projections = Projections.include("title");
		  
		  if(!which.equals("all")) {
			  Bson filters = Filters.eq("_id", which);
			  iterDoc  = getCollection("course").find(filters);
		  }
		  else
			  iterDoc  = getCollection("course").find();
		  iterDoc  = iterDoc.projection(projections);
		  
		  return parseIterDoc(iterDoc);
	  }
	  
	  private String fetchDetail(String which) {
		  FindIterable<Document> iterDoc;
		  if(!which.equals("all")) {
			  Bson filter = Filters.eq("_id", which);
			  iterDoc  = getCollection("course").find(filter);
		  }
		  else
			  iterDoc  = getCollection("course").find();
		  
		  return parseIterDoc(iterDoc);
	  }
	  
	  private String fetchRemark(String which) {
		  FindIterable<Document> iterDoc;
		  Bson projections = Projections.include("comments");
		  
		  if(!which.equals("all")) {
			  Bson filters = Filters.eq("_id", which);
			  iterDoc  = getCollection("course").find(filters);
		  }
		  else
			  iterDoc  = getCollection("course").find();
		  iterDoc = iterDoc.projection(projections);
		  
		  return parseIterDoc(iterDoc);
	  }
	  
	  private String updateWrite(String which, String message, String userID) {
		  Bson filters = Filters.and(Filters.eq("_id", which), Filters.eq("idProfessor", userID));
		  Document newComment = new Document("message", message);
		  newComment.append("date", (new Date().getTime()));
		  Bson updates = Updates.addToSet("comments", newComment);
		  
		  Document doc = getCollection("course").findOneAndUpdate(filters, updates);
		  if(doc == null)
			  return "Course not found or you are not the course's professor.";
		  else
			  return "Message sent successfully!";
	  }
	  
	  public String query(String userQuery, String userType, String userID) throws RemoteException{
		  long timeElapsed = System.nanoTime();
		  
		  String[] parts = userQuery.split(" ");
		  
		  String ans = null;
		  
		  if(parts.length==1 && parts[0].equals("help"))
			  return showHelp(userType);
		  else if(parts.length==2) {
			  if(parts[0].equals("content"))
				  ans = fetchContent(parts[1]);
			  else if(parts[0].equals("list"))
				  ans = fetchList(parts[1]);
			  else if(parts[0].equals("detail"))
				  ans = fetchDetail(parts[1]);
			  else if(parts[0].equals("remark"))
				  ans = fetchRemark(parts[1]);
		  }
		  else if(parts.length>=3) {
			  if(parts[0].equals("write")) {
				  String message = userQuery.substring(parts[0].length()+parts[1].length()+2);
				  if(userType.equals("professor"))
					  ans = updateWrite(parts[1], message, userID);
			  }
		  }
		  if(ans == null)
			  ans = "Please try again, your request did not match any available command.\nType 'help' for a list of available commands.";
		  
		  timeElapsed = System.nanoTime() - timeElapsed;
		  
		  //just for testing purposes. Comment it to really work as intended.
		  ans = Long.toString(timeElapsed) + " " + ans;
		  
		  return ans;
	  }
 }
