import java.rmi.*;
 
public interface SchoolDatabaseInterface extends Remote {
	public int add(int a,int b) throws RemoteException;
	public String welcomingMessage() throws RemoteException;
	public String[] logIn(String query) throws RemoteException;
	public String query(String userQuery, String userType, String userID) throws RemoteException;
	public String showHelp(String userType) throws RemoteException;
}
