#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

#include<mongoc.h> //for mongodb database

//the thread function
void *connection_handler(void *);

// mongoDB
// client creation
mongoc_client_t *createDatabaseClient();
// database retrieval
mongoc_database_t *getDatabase(mongoc_client_t *client);
//collection retrieval
mongoc_collection_t *getCollection(mongoc_client_t *client, char *collName);
//retrieve a document, given an id and optional attribute
char *retrieveDocumentWithAttribute(mongoc_client_t *client, mongoc_collection_t *collection, char* user_id, char* attribute, char* value);

//returns a list of commands available to the user_id
char *listAvailableCommands(bool isProfessor);
// return all courses and their infos

// return content of a specific course

// return all info of a specific course

// return all courses ids and titles

// add a comment on the course (only available to course's professor)

// get most recent comment on the course

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char message[2000] , client_message[2000], user_id[7];
    char *userTypeCollection = "student";
    bool isProfessor = false,
        error = false,
        firstContact = false,
        loggedIn = false,
        informedPassword = true;

    /*
     * mongoDB
     */
    mongoc_client_t *client = createDatabaseClient();
    mongoc_database_t *database = getDatabase(client);

    /*
    bson_t *command, reply;
    bson_error_t error;
    char *str;
    bool retval;

    command = BCON_NEW ("ping", BCON_INT32 (1));

    retval = mongoc_client_command_simple (
       client, "admin", command, NULL, &reply, &error);

    if (!retval) {
       fprintf (stderr, "%s\n", error.message);
       return EXIT_FAILURE;
    }

    str = bson_as_json (&reply, NULL);
    printf ("%s\n", str);

    bson_destroy (&reply);
    bson_destroy (command);
    bson_free (str);
    */

    //Send some messages to the client
    strcpy(message, "Welcome to Uniluder's system!");
    write(sock , message , strlen(message));

    strcpy(message,"If you want to log in as a professor, please type '1'.\nOtherwise, if you want to log in as a student, press Enter.");
    write(sock , message , strlen(message));

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //last valid character is always a '\n'
        client_message[--read_size] = '\0';

        puts("Received query:");
        puts(client_message);

        if(!firstContact){
            firstContact = true;

            //gets answer from client and checks whether it is a '1'
            if(strcmp(client_message, "1") == 0){
                isProfessor = true;
                userTypeCollection = "professor";
                informedPassword = false;
                strcpy(message,"You are now logging in as a professor.\n");
            }
            else
                strcpy(message, "You are now logging in as a student.\n");
            //asks client to enter his system ID
            strcat(message, "Please type your Academic Registration number:");
        }
        else if(!loggedIn){
            if(retrieveDocumentWithAttribute(client, getCollection(client, userTypeCollection), client_message, NULL, NULL)) {
                loggedIn = true;
                strcpy(user_id, client_message);

                if(!isProfessor)
                    strcpy(message, "You are now logged in! For a list of available commands, type 'help'");
                else
                    strcpy(message, "Please type your password:");
            }
            else
                strcpy(message, "Academic Registration number not found in th system. Try again:");
        }
        else if(!informedPassword){
            if(retrieveDocumentWithAttribute(client, getCollection(client, userTypeCollection), user_id, "password", client_message) != NULL){
                informedPassword = true;
                strcpy(message, "You are now logged in! For a list of available commands, type 'help'");
            } else{
                strcpy(message, "Incorrect password. Try again.");
            }
        }
        //now the user is logged in correctly and s/he may make queries to the server
        else{
            if(strcmp(client_message, "help") == 0){
                strcpy(message, listAvailableCommands(isProfessor));
            }
            else{
                //parse user's request and return the value of a mongo query to user
                strcpy(message, "Pretend your answer is here!");
            }
        }
        write(sock, message, strlen(message));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}

/*
Returns a string with the value of a document wanted in a given collection with a known ID and attribute value
*/
char *retrieveDocumentWithAttribute(mongoc_client_t *client, mongoc_collection_t *collection, char* user_id, char* attribute, char* value)
{
    bson_error_t error;
    mongoc_cursor_t *cursor;
    bson_t *filter = bson_new();
    const bson_t *doc;
    bool success;
    char *ans;

    /* Find document with specific ID and attribute's value */
    BSON_APPEND_UTF8 (filter, "_id", user_id);
    if(attribute != NULL)
        BSON_APPEND_UTF8 (filter, attribute, value);

    cursor = mongoc_collection_find_with_opts(collection, filter, NULL, NULL);

    //since we are searching with an ID, only one answer at most can be retrieved
    if(mongoc_cursor_next (cursor, &doc)){
        ans = bson_as_canonical_extended_json (doc, NULL);
    }

    if(mongoc_cursor_error (cursor, &error))
        fprintf (stderr, "An error occurred: %s\n", error.message);

    mongoc_cursor_destroy (cursor);
    bson_destroy (filter);

    return ans;
}
/*
  Will retrieve database
*/
mongoc_database_t *getDatabase(mongoc_client_t *client){
  /*
   * Get a handle on the database "tcp"
   */
  return mongoc_client_get_database (client, "tcp");
}

mongoc_collection_t *getCollection(mongoc_client_t *client, char *collName){
  /*
   * Get a handle on the database "tcp" to a collection
   */
  return mongoc_client_get_collection (client, "tcp", collName);
}

/*
  Will connect to mongoDB and return a client
*/
mongoc_client_t *createDatabaseClient(){

  const char *uri_str = "mongodb://localhost:27017";
  mongoc_client_t *client;
  /*
   * Required to initialize libmongoc's internals
   */
  mongoc_init ();
  /*
   * Create a new client instance
   */
  client = mongoc_client_new (uri_str);
  /*
   * Register the application name so we can track it in the profile logs
   * on the server. This can also be done from the URI (see other examples).
   */
  mongoc_client_set_appname (client, "tcp-login");

  return client;
}

char *listAvailableCommands(bool isProfessor){
    return "TODO";
}
