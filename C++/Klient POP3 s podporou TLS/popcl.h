/*
    ISA - POP3 client (header file)
    Richard Seipel
    xseipe00
*/

#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <err.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <regex>
#include <fstream>
#include <ctime>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#define BUFFER 1024
#define MARKERFILE "last_message_marker"


/* 
 * Arguments - class for reading and processing arguments  
 * ---------------------------------------------------------------------------------------------------------------------*/

class Arguments
{
    std::string getValue(int position, int argc, char *argv[]);

    char getArgumentAsChar(std::string argument);

    void validate();

    void setDefaultValues();
    
    public:
    std::string server;
    std::string port;
    std::string certFilePath;
    std::string certDirPath;
    std::string authFilePath;
    std::string outDirPath;

    bool isServer = false;
    bool isPort = false;
    bool isT = false;
    bool isS = false;
    bool isCertFilePath = false;
    bool isCertDirPath = false;
    bool isDelete = false;
    bool isNew = false;
    bool isAuthFilePath = false;
    bool isOutDirPath = false;

    void loadArguments(int argc, char *argv[]);
};


/* 
 * Auth - class for reading and storing authentication data  
 * ---------------------------------------------------------------------------------------------------------------------*/

class Auth
{
    public:
    std::string username = "";
    std::string password = "";

    void loadAuth(std::string path);
};


/* 
 * Connection - class for establishing and handling a secure and unsecure connection
 * ---------------------------------------------------------------------------------------------------------------------*/

class Connection
{   
    char buffer[BUFFER] = {0};

    void clearBuffer();

    std::string getHostWithPort(std::string hostname, std::string port);

    void getCertificates(Arguments *arguments);

    void connect(Arguments *arguments);
    
    void connectWithSSL(Arguments *arguments);

    void connectWithSTLS(Arguments *arguments);

    public:
    BIO *bio;
    SSL_CTX *ctx;
    SSL *ssl;

    Connection();

    ~Connection();

    void loadOpenSSL();
    
    void write(std::string input);

    std::string read();
    
    std::string readLine();

    std::string readMessage();

    void establishConnection(Arguments *arguments);

    void establishSecureConnection(Arguments *arguments);

    void establishSTLSConnection(Arguments *arguments);
};

// -------------------------------------------------------------------------*/


/* 
 * Constants - namespace for storing string constants 
 * ---------------------------------------------------------------------------------------------------------------------*/

namespace Constants
{
    const std::string endOfLine = "\r\n";
    const std::string endOfMessage = "\r\n.\r\n";
    const std::string helpString = (
        "Usage: " 
        "popcl <server> [-p <port>]" 
        "[-T|-S [-c <certfile>] [-C <certaddr>]]\n"
        "[-d] [-n] -a <auth_file> -o <out_dir>\n"
        "[] - optional"
    );
}


/* 
 * Response - namespace for processing inputs and outputs from files or server 
 * ---------------------------------------------------------------------------------------------------------------------*/

namespace Response
{
    void checkIfStartsWithOk(std::string response);

    void checkDirectory(std::string path);
    
    std::string removeFirstLine(std::string response);
    
    std::string removeEndDot(std::string response);

    std::string findDataInStream(std::istream *stream, std::string prefix);
    
    std::string findIdInMessage(std::string message);

    std::string findIdInMarkerFile(std::string path);

    std::string findDataInFile(std::string path, std::string prefix);
}


/* 
 * Functions that represents the pop3 client 
 * ---------------------------------------------------------------------------------------------------------------------*/

void logIn(Connection *connection, std::string username, std::string password);

int getNumberOfEmails(Connection *connection);

std::string getLastMessageOnServer(Connection *connection, int lastMessageNumber);

void generateOutputMessage(int lastMessageNumber, int actualMessageNumber, bool onlyNew);

void storeEmails(Connection *connection, int lastEmailNumber, std::string outputPath, bool onlyNew);

void deleteEmails(Connection *connection, int lastEmailNumber);

void quitSession(Connection *connection);
