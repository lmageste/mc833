#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread

#include <sys/time.h>

#include <mongoc.h> //for mongodb database

//the thread function
void *connection_handler(void *);
//A string is modified to display a list of available commands to the user
void listAvailableCommands(char *help, bool isProfessor);
//parses user's request and updates its contents with its result
void parseRequestAndUpdateWithResult(char *ans, mongoc_client_t *client, mongoc_database_t *database, char *request, bool isProfessor, char *userID);
//fetches a word from a string, ignoring whitespaces and returns the number of characters read in the original string (including whitespace)
int fetchWord(char *original, char *word);

// mongoDB
// client creation
mongoc_client_t *createDatabaseClient();
// database retrieval
mongoc_database_t *getDatabase(mongoc_client_t *client);
//collection retrieval
mongoc_collection_t *getCollection(mongoc_client_t *client, char *collName);
//retrieve a document, given an id and optional attribute
bool retrieveDocument(char *ans, mongoc_client_t *client, mongoc_collection_t *collection, bson_t* filter, bson_t *opts);
//return all courses and their infos

//return content of a specific course

//return all info of a specific course

//return all courses ids and titles

//add a comment on the course (only available to course's professor)

//get most recent comment on the course


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
    char message[5000] , client_message[2000], userID[7];
    char *userTypeCollection = "student";
    bool isProfessor = false,
        error = false,
        firstContact = false,
        loggedIn = false,
        informedPassword = true;
    struct timeval tv1, tv2;
    char timestr[5000];

    /*
     * mongoDB
     */
    mongoc_client_t *client = createDatabaseClient();
    mongoc_database_t *database = getDatabase(client);

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
            bson_t *filter = BCON_NEW("_id", client_message);
            if(retrieveDocument(message, client, getCollection(client, userTypeCollection), filter, NULL)) {
                loggedIn = true;
                strcpy(userID, client_message);

                if(!isProfessor)
                    strcpy(message, "You are now logged in! For a list of available commands, type 'help'");
                else
                    strcpy(message, "Please type your password:");
            }
            else
                strcpy(message, "Academic Registration number not found in th system. Try again:");
            bson_destroy(filter);
        }
        else if(!informedPassword){
            bson_t *filter = BCON_NEW("_id", userID, "password", client_message);
            if(retrieveDocument(message, client, getCollection(client, userTypeCollection), filter, NULL)){
                informedPassword = true;
                strcpy(message, "You are now logged in! For a list of available commands, type 'help'");
            } else
                strcpy(message, "Incorrect password. Try again.");
            bson_destroy(filter);
        }
        //now the user is logged in correctly and s/he may make queries to the server
        else{
            if(strcmp(client_message, "help") == 0){
                listAvailableCommands(message, isProfessor);
            }
            else{
                //parse user's request and return the value of a mongo query to user
                gettimeofday(&tv1, NULL);
                parseRequestAndUpdateWithResult(message, client, database, client_message, isProfessor, userID);
                gettimeofday(&tv2, NULL);

                sprintf(timestr, "%ld", tv2.tv_usec-tv1.tv_usec);
                strcpy(timestr, strcat(timestr, " - "));
                strcpy(message, strcat(timestr, message));
            }
        }
        write(sock, message, strlen(message));

        fflush(stdin);
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
Populates a string with the value of document(s) wanted after being updated in a given collection. Returns a boolean indicating if update was successful.
*/
bool updateDocument(char *ans, mongoc_client_t *client, mongoc_collection_t *collection, bson_t *filter, bson_t *update){
    mongoc_find_and_modify_opts_t *opts;
    bson_t reply;
    bson_error_t error;
    bool success;

    opts = mongoc_find_and_modify_opts_new ();
    mongoc_find_and_modify_opts_set_update (opts, update);
    mongoc_find_and_modify_opts_set_flags (opts, MONGOC_FIND_AND_MODIFY_RETURN_NEW);

    success = mongoc_collection_find_and_modify_with_opts (
    collection, filter, opts, &reply, &error);
    if (success) {
        bson_iter_t iter;
        bson_iter_init(&iter, &reply);
        bson_iter_find(&iter, "value");
        const bson_value_t *foundVal = bson_iter_value(&iter);
        //unable to fetch document
        if(foundVal->value_type == BSON_TYPE_NULL)
            success = false;
        //document fetched, so return the document which was changed
        else if(foundVal->value_type == BSON_TYPE_DOCUMENT){
            bson_t *val = bson_new_from_data(foundVal->value.v_doc.data, foundVal->value.v_doc.data_len);
            strcpy(ans, bson_as_canonical_extended_json(val, NULL));
            bson_destroy(val);
        }
        else //for some other reason
            success = false;
    } else {
        fprintf (
           stderr, "Got error: \"%s\" on line %d\n", error.message, __LINE__);
    }

    bson_destroy(&reply);
    mongoc_find_and_modify_opts_destroy(opts);

    return success;
}

/*
Populates a string with the value of document(s) wanted in a given collection with. Returns a boolean indicating success
*/
bool retrieveDocument(char *ans, mongoc_client_t *client, mongoc_collection_t *collection, bson_t *filter, bson_t *opts)
{
    bson_error_t error;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bool success = false;

    if(filter)
        cursor = mongoc_collection_find_with_opts(collection, filter, opts, NULL);
    else{ //sending NULL as filter causes an exception
        bson_t *emptyFilter = bson_new();
        cursor = mongoc_collection_find_with_opts(collection, emptyFilter, opts, NULL);
        bson_destroy(emptyFilter);
    }

    ans[0] = '\0'; //effectively empties current string to append content to
    while(mongoc_cursor_next (cursor, &doc)){
        strcat(ans, bson_as_canonical_extended_json (doc, NULL));
        success = true;
    }

    if(mongoc_cursor_error (cursor, &error))
        fprintf (stderr, "An error occurred: %s\n", error.message);

    mongoc_cursor_destroy (cursor);

    return success;
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
  //mongoc_client_set_appname (client, "tcp-login");

  return client;
}
/*
A string is modified to display a list of available commands to the user, based on its privileges (student/professor)
*/
void listAvailableCommands(char *help, bool isProfessor){
    char *str;
    help[0] = '\0';
    str = "'content COURSE_CODE' = Displays content of given course\n";
    strcat(help, str);
    str = "'content all' = Displays contents of all courses\n";
    strcat(help, str);
    str = "'list COURSE_CODE' = Displays name of given course\n";
    strcat(help, str);
    str = "'list all' = Displays names of all courses available\n";
    strcat(help, str);
    str = "'detail COURSE_CODE' = Displays detailed information of given course\n";
    strcat(help, str);
    str = "'detail all' = Displays detailed information of all courses available\n";
    strcat(help, str);
    str = "'remark COURSE_CODE' = Displays professor's remarks on the course";
    strcat(help, str);
    str = "'remark all' = Displays professors' remarks on all courses";
    strcat(help, str);
    if(isProfessor){
        str = "\n'write COURSE_CODE MESSAGE' = Adds a new remark to the subject in case the lecturer of this course is you";
        strcat(help, str);
    }
    return;
}
/*
Parses user's request and updates its contents with its result
*/
void parseRequestAndUpdateWithResult(char *ans, mongoc_client_t *client, mongoc_database_t *database, char *request, bool isProfessor, char *userID) {
    //list command
    char command[200];
    int startNextWord;
    bool invalid = false;
    mongoc_collection_t *collection;
    bson_t *opts = NULL, *filter = NULL, *update = NULL;

    startNextWord = fetchWord(request, command);

    //only one word command
    if(request[startNextWord] == '\0')
        invalid = true;
    //list command
    else if(strcmp(command, "list") == 0){
        //gets course collection from db
        collection = getCollection(client, "course");
        //only gets title and _id from courses
        opts = BCON_NEW("projection", "{", "_id", BCON_BOOL(true), "title", BCON_BOOL(true), "}");
        //filter a specific course
        if(strcmp(request+startNextWord, "all") != 0)
            filter = BCON_NEW("_id", request+startNextWord);
    }
    //content command
    else if(strcmp(command, "content") == 0){
        //gets course collection from db
        collection = getCollection(client, "course");
        //only gets content from courses
        opts = BCON_NEW("projection", "{", "content", BCON_BOOL(true), "_id", BCON_BOOL(false) , "}");
        //filter a specific course
        if(strcmp(request+startNextWord, "all") != 0)
            filter = BCON_NEW("_id", request+startNextWord);
    }
    //detail command
    else if(strcmp(command, "detail") == 0){
        //gets course collection from db
        collection = getCollection(client, "course");
        //all info, so no opts needed
        //filter a specific course
        if(strcmp(request+startNextWord, "all") != 0)
            filter = BCON_NEW("_id", request+startNextWord);
    }
    //remark command
    else if(strcmp(command, "remark") == 0){
        //gets course collection from db
        collection = getCollection(client, "course");
        //gets only messages from course
        opts = BCON_NEW("projection", "{", "comments", BCON_BOOL(true), "_id", BCON_BOOL(false), "}");
        //filters specific course
        if(strcmp(request+startNextWord, "all") != 0)
            filter = BCON_NEW("_id", request+startNextWord);

    }
    //write command
    else if(strcmp(command, "write") == 0 && isProfessor){
        char course[100];
        int charCount, startMessage;
        //gets message
        startMessage = startNextWord+fetchWord(request+startNextWord, course);
        request[startMessage-1] = '\0';
        //writes a comment
        if(request[startMessage] != '\0'){
            //gets course collection from db
            collection = getCollection(client, "course");
            //filters a specific course that this professor teaches
            filter = BCON_NEW("_id", request+startNextWord, "idProfessor", userID);
            puts("os parametros sao:");
            puts(request+startNextWord);
            puts(request+startMessage);
            puts(userID);
            //adds new message to the message set
            update = bson_new();
            bson_t *comment = bson_new();
            bson_t *msg = bson_new();
            BSON_APPEND_DOCUMENT_BEGIN(update, "$addToSet", comment);
            //comment BSON
            BSON_APPEND_DOCUMENT_BEGIN(comment, "comments", msg);
            //message BSON
            BSON_APPEND_UTF8(msg, "message", request+startMessage);
            time_t t = time(NULL);
            struct tm now = *localtime(&t);
            BSON_APPEND_DATE_TIME(msg, "date", mktime (&now) * 1000);
            //finish appending
            bson_append_document_end(comment, msg);
            bson_append_document_end(update, comment);

            bson_destroy(comment);
            bson_destroy(msg);
        }
        else
            invalid = true;
    }
    else
        invalid = true;

    if(invalid)
        strcpy(ans, "Invalid Command. Type 'help' for a list of available commands.");
    else if(update!=NULL){
        //insert new element in field's set
        if(!updateDocument(ans, client, collection, filter, update))
            strcpy(ans, "Course not found or you are not the course's professor.");
        else
            strcpy(ans, "Message sent successfully!");
    }
    else if(!retrieveDocument(ans, client, collection, filter, opts)){
        strcpy(ans, "No entries found.");
    }

    //free memory
    if(opts)
        bson_destroy(opts);
    if(filter)
        bson_destroy(filter);
    if(update)
        bson_destroy(update);

    return;
}
/*
Fetches a word from a string, ignoring whitespaces and returns the number of characters read in the original string (including whitespace)
*/
int fetchWord(char *original, char *word){
    int i = 0, indexWord;
    while(original[i]==' ')
        i++;
    indexWord = i;
    while(original[i]!=' ' && original[i]!='\0'){
        word[i-indexWord] = original[i];
        i++;
    }
    word[i-indexWord] = '\0';
    while(original[i]==' ')
        i++;
    return i;
}