import java.rmi.*;
import java.util.Scanner; 
 
public class SchoolDatabaseClient {
	public static void main (String[] args) {
		SchoolDatabaseInterface client;
		try {
  		    //System.setSecurityManager(new RMISecurityManager());
			client = (SchoolDatabaseInterface)Naming.lookup("rmi://localhost:2020/SchoolServerSide");
			
			Scanner scan = new Scanner(System.in);
			
			//first interaction
			System.out.println(client.welcomingMessage());

			//logging in
			String[] loggedIn = new String[0];
			while(loggedIn.length==0) {
				String line = scan.nextLine();
				loggedIn = client.logIn(line);
				if(loggedIn.length==0)
					System.out.println("Credentials failed, try again");
				else
					System.out.println("Successfully logged in!");
			}
			
			//get type and userId from log in answer
			String userType = loggedIn[0];
			String userID = loggedIn[1];
			
			//exit program
			System.out.println("If you would like to log out, type 'exit'");
			
			//show available commands to the user
			System.out.println(client.showHelp(userType));
			
			//loops on user queries
			while(true) {
				String line = scan.nextLine();
				if(line.equals("exit"))
					break;
				
				String ans = client.query(line, userType, userID);
				System.out.println(ans);
			}
			
			//the end
			scan.close();
 
			}catch (Exception e) {
				System.out.println("client exception: " + e);
			}
		}
}
