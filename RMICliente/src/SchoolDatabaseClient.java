import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner; 
 
public class SchoolDatabaseClient {
	public static void main (String[] args) {
		SchoolDatabaseInterface client;

		try {
			System.setProperty("java.rmi.server.hostname", "192.168.0.30");
			Registry registry = LocateRegistry.getRegistry("192.168.0.30");
			client = (SchoolDatabaseInterface)registry.lookup("SchoolServerzinho");
			
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
				
				long t = System.nanoTime();
				String ans = client.query(line, userType, userID);
				t = System.nanoTime()-t;
				System.out.println(ans.split(" ")[0]);
				System.out.println(Long.toString(t));
			}
			
			//the end
			scan.close();
 
			}catch (Exception e) {
				System.out.println("client exception: " + e);
			}
		}
}
