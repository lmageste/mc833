# About
Server/Client project for managing courses, students and professors.

# Acknowledgements
Core of server/client network communication done using a modified version of tutorial available at: https://www.binarytides.com/server-client-example-c-sockets-linux/

# MongoDB
Install MongoDB:
https://docs.mongodb.com/manual/installation/

Install MongoDB C driver:
http://mongoc.org/libmongoc/current/tutorial.html#include-and-link-libmongoc-in-your-c-program

# How to use
 - Create your own database in mongo (in *create_courses.sh* there are some commands to create a very small database)

 - Compile the C files (*sever.c* and *client.c*), linking the *libmongo.c* library to compilation.
   ```
   gcc -o client client.c $(pkg-config --libs --cflags libmongoc-1.0)
   gcc -pthread -o server server.c $(pkg-config --libs --cflags libmongoc-1.0)
   ```
  
 - Initiate your MongoDB server before running any file:
 > $ mongod

 - Run your files (first *server.out* and then *client.out*) (hopefully with no mistakes :smile:)
