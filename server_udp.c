#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/time.h>
#include <mongoc.h> //for mongodb database

#define PORT 8888
#define MESSAGE_SIZE 5000
#define CLIENT_MESSAGE_SIZE 1000

//A string is modified to display a list of available commands to the user
void listAvailableCommands(char *help);
//parses user's request and updates its contents with its result
void parseRequestAndUpdateWithResult(char *ans, mongoc_client_t *client, mongoc_database_t *database, char *request);
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

int main(int argc , char *argv[])
{
    struct sockaddr_in server , client;
    int sock, slen = sizeof(client);
    int read_size;
    char message[MESSAGE_SIZE] , client_message[CLIENT_MESSAGE_SIZE];
    struct timeval tv1, tv2;
    char timestr[MESSAGE_SIZE];

    /*
     * mongoDB
     */
    mongoc_client_t *clientDB = createDatabaseClient();
    mongoc_database_t *database = getDatabase(clientDB);

    //Create socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons( PORT );

    //Bind
    if( bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Keeps listening for data
    while(true){
        puts("Waiting for data...");

        //read_size always is the total size of message object, regardless of size of actual string contained
        if ((read_size = recvfrom(sock, client_message, CLIENT_MESSAGE_SIZE, 0, (struct sockaddr *) &client, &slen)) == -1){
            puts("Receive failed");
            return 1;
        }
        client_message[read_size] = '\0';
        puts("Received Message:");
        puts(client_message);

        //TO-DO: treat request
        //strcpy(message, client_message);
        //parse user's request and return the value of a mongo query to user
        gettimeofday(&tv1, NULL);
        parseRequestAndUpdateWithResult(message, clientDB, database, client_message);
        gettimeofday(&tv2, NULL);

        sprintf(timestr, "%ld", tv2.tv_usec-tv1.tv_usec);
        strcat(timestr, " - ");
        strcpy(message, strcat(timestr, message));

        if (sendto(sock, message, strlen(message), 0, (struct sockaddr*) &client, slen) == -1){
            puts("Send failed");
            return 1;
        }
    }
    close(sock);
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
void listAvailableCommands(char *help){
    char *str;
    help[0] = '\0';
    str = "'help' = Displays all available commands\n";
    strcat(help, str);
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
    str = "'remark COURSE_CODE' = Displays professor's remarks on the course\n";
    strcat(help, str);
    str = "'remark all' = Displays professors' remarks on all courses\n";
    strcat(help, str);
    str = "'write PROFESSOR_ID PASSWORD COURSE_CODE MESSAGE' = By providing your professor's ID and system password, adds a new remark to a course in case you are the lecturer of it";
    strcat(help, str);
    return;
}
/*
Parses user's request and updates its contents with its result
*/
void parseRequestAndUpdateWithResult(char *ans, mongoc_client_t *client, mongoc_database_t *database, char *request) {
    //list command
    char command[200];
    int startNextWord;
    bool invalid = false;
    mongoc_collection_t *collection;
    bson_t *opts = NULL, *filter = NULL, *update = NULL;

    //makes size of answer 0
    ans[0]='\0';

    startNextWord = fetchWord(request, command);
    //only one word command
    if(request[startNextWord] == '\0'){
        if(strcmp(command, "help")==0)
            listAvailableCommands(ans);
        else
            invalid = true;
    }
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
    else if(strcmp(command, "write") == 0){
        char course[100], userID[100], password[100];
        int charCount, startMessage;
        //gets message
        startNextWord = startNextWord+fetchWord(request+startNextWord, userID);
        startNextWord = startNextWord+fetchWord(request+startNextWord, password);
        startMessage = startNextWord+fetchWord(request+startNextWord, course);
        //if no message or fewer arguments than expected
        if(request[startMessage]=='\0')
            invalid = true;
        //else writes a comment
        else if(request[startMessage] != '\0'){
            //first needs to check whether password and professor ID match, then update if positive result
            bool informedPassword = false;
            bson_t *passwordFilter = BCON_NEW("_id", userID, "password", password);
            if(retrieveDocument(password, client, getCollection(client, "professor"), passwordFilter, NULL)){
                informedPassword = true;
            }
            bson_destroy(passwordFilter);
            //incorrect password
            if(!informedPassword)
                strcpy(ans, "Incorrect password for this Professor's ID");
            //correct password
            else{
                //gets course collection from db
                collection = getCollection(client, "course");
                //filters a specific course that this professor teaches
                filter = BCON_NEW("_id", course, "idProfessor", userID);
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
        }
        else
            invalid = true;
    }
    else
        invalid = true;

    if(ans[0]=='\0'){
        if(invalid){
            strcpy(ans, "Invalid Command. Type 'help' for a list of available commands.");
        }
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
