import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.*;   
 
public class SchoolDatabaseServer {
	   public static void main (String[] argv) {
		   try {
			   //System.setProperty("java.rmi.server.hostname", "177.220.84.16");
			   //System.setSecurityManager(new RMISecurityManager());
 
			   SchoolDatabase Hello = new SchoolDatabase();	
			   LocateRegistry.createRegistry(2020);
			   Naming.rebind("rmi://localhost:2020/SchoolServerSide", Hello);
 
			   System.out.println("SchoolDatabase Server is ready.");
			   }catch (Exception e) {
				   System.out.println("SchoolDatabase Server failed: " + e);
				}
		   }
}
