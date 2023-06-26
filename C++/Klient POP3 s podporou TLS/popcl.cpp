/*
    ISA - POP3 client (implementation)
    Richard Seipel
    xseipe00
*/

#include "popcl.h"

/* 
 * Arguments - class for reading and processing arguments  
 * ---------------------------------------------------------------------------------------------------------------------*/

std::string Arguments::getValue(int position, int argc, char *argv[])
{
    position++;
    if (not (position < argc))
    {
        std::cerr << "Expected parameter after argument " << argv[position-1] << ".\n"
        << Constants::helpString << std::endl;
        throw std::exception();
    }
    return argv[position];
}

char Arguments::getArgumentAsChar(std::string argument)
{
    if (argument.length() != 2 or argument[0] != '-')
    {
        std::cerr << "Incorrect argument " << argument << ".\n"
        << Constants::helpString << std::endl;
        throw std::exception();
    }
    return argument[1];
}

void Arguments::validate()
{
    if (not (isServer and isAuthFilePath and isOutDirPath))
    {
        std::cerr << "Missing required argument.\n"
        << Constants::helpString << std::endl;
        throw std::exception();
    }
    if (isT and isS)
    {
        std::cerr << "Parameters -T and -S cannot be used at the same time." << std::endl;
        throw std::exception();
    }
    if ((isCertFilePath or isCertDirPath) and ! (isT or isS))
    {
        std::cerr << "Cannot add path to certificates when a secure connection is not being established." << std::endl;
        throw std::exception();
    }
}

void Arguments::setDefaultValues()
{
    if (!isPort)
    {
        if (isT) port = "995";
        else port = "110";
    }
}

void Arguments::loadArguments(int argc, char *argv[])
{
    int position = 1;
    char argument;

    if (position < argc)
    {
        server = argv[1];
        isServer = true;
        position++;
    }

    while (position < argc)
    {
        argument = getArgumentAsChar(argv[position]);
        switch (argument)
        {
        case 'p':
            port = getValue(position, argc, argv);
            isPort = true;
            position += 2;
            break;
        
        case 'T':
            isT = true;
            position++;
            break;
        
        case 'S':
            isS = true;
            position++;
            break;
        
        case 'c':
            certFilePath = getValue(position, argc, argv);
            isCertFilePath = true;
            position += 2;
            break;
        
        case 'C':
            certDirPath = getValue(position, argc, argv);
            isCertDirPath = true;
            position += 2;
            break;
        
        case 'd':
            isDelete = true;
            position++;
            break;
        
        case 'n':
            isNew = true;
            position++;
            break;
        
        case 'a':
            authFilePath = getValue(position, argc, argv);
            isAuthFilePath = true;
            position += 2;
            break;
        
        case 'o':
            outDirPath = getValue(position, argc, argv);
            isOutDirPath = true;
            position += 2;
            break;
        
        default:
            std::cerr << "Incorrect argument " << argv[position] << "." << "\n"
            << Constants::helpString << std::endl;
            throw std::exception();
            break;
        }
    }
    validate();
    setDefaultValues();
}


/* 
 * Auth - class for reading and storing authentication data  
 * ---------------------------------------------------------------------------------------------------------------------*/

void Auth::loadAuth(std::string path)
{
    username = Response::findDataInFile(path, "username = ");
    password = Response::findDataInFile(path, "password = ");
}


/* 
 * Connection - class for establishing and handling a secure and unsecure connection
 * ---------------------------------------------------------------------------------------------------------------------*/

Connection::Connection()
{
    bio = NULL;
    ctx = NULL;
    ssl = NULL;
}

Connection::~Connection()
{
    if (ctx) SSL_CTX_free(ctx);
    if (bio) BIO_free_all(bio);    
}

void Connection::loadOpenSSL()
{
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL)
    {
        std::cerr << "Error while creating ctx object." << std::endl;
        throw std::exception();
    } 
}

void Connection::clearBuffer()
{
    memset(buffer, 0, BUFFER);
}

std::string Connection::getHostWithPort(std::string hostname, std::string port)
{
    return hostname + ":" + port;
}

void Connection::getCertificates(Arguments *arguments)
{
    if (arguments->isCertFilePath) 
    {
        if(! SSL_CTX_load_verify_locations(ctx, arguments->certFilePath.c_str(), NULL))
        {
            std::cerr << "Unsuccessful loading of certificates from the specified file." << std::endl;
            throw std::exception();
        }
    }
    if (arguments->isCertDirPath)
    {
        Response::checkDirectory(arguments->certDirPath);
        if(! SSL_CTX_load_verify_locations(ctx, NULL, arguments->certDirPath.c_str()))
        {
            std::cerr << "Unsuccessful loading of certificates from the specified directory." << std::endl;
            throw std::exception();
        }
    }
    
    if (! SSL_CTX_set_default_verify_paths(ctx))
    {
        std::cerr << "Unsuccessful loading of default certificate paths." << std::endl;
        throw std::exception();
    }
}

void Connection::connect(Arguments *arguments)
{
    std::string hostWithPort = getHostWithPort(arguments->server, arguments->port);
            
    // Author: Kenneth Ballard
    // Source: https://developer.ibm.com/tutorials/l-openssl/
    bio = BIO_new_connect(hostWithPort.c_str());
    if(bio == NULL)
    {
        std::cerr << "Socket creation failed." << std::endl;
        throw std::exception();
    }
    if(BIO_do_connect(bio) <= 0)
    {
        std::cerr << "Connection to " << hostWithPort << " failed." << std::endl;
        throw std::exception();
    }
    //
}

void Connection::connectWithSSL(Arguments *arguments)
{
    std::string hostWithPort = getHostWithPort(arguments->server, arguments->port);

    // Author: Kenneth Ballard
    // Source: https://developer.ibm.com/tutorials/l-openssl/
    bio = BIO_new_ssl_connect(ctx);
    if(bio == NULL)
    {
        std::cerr << "Establishment of secured connection failed." << std::endl;
        throw std::exception();
    }
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    BIO_set_conn_hostname(bio, hostWithPort.c_str());
    if(BIO_do_connect(bio) <= 0)
    {
        std::cerr << "Connection to " << hostWithPort << " failed." << std::endl;
        throw std::exception();        
    }
    if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        std::cerr << "Verification of " << hostWithPort << " failed." << std::endl;
        throw std::exception();
    }
    //
}

void Connection::connectWithSTLS(Arguments *arguments)
{
    std::string hostWithPort = getHostWithPort(arguments->server, arguments->port);

    // Author: Martin Prikryl
    // Source: https://stackoverflow.com/questions/49132242/openssl-promote-insecure-bio-to-secure-one
    BIO *newbio = NULL, *biossl = NULL;

    if ((biossl = BIO_new_ssl(ctx, 1)) == NULL)
    {
        std::cerr << "Establishment of secured connection failed." << std::endl;
        throw std::exception();
    }
    if ((newbio = BIO_push(biossl, bio)) == NULL)
    {
        std::cerr << "Establishment of secured connection failed." << std::endl;
        throw std::exception();
    }
    bio = newbio;
    //

    // Author: Kenneth Ballard
    // Source: https://developer.ibm.com/tutorials/l-openssl/
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    if(BIO_do_connect(bio) <= 0)
    { 
        std::cerr << "Connection to " << hostWithPort << " failed." << std::endl;
        throw std::exception();   
    }
    if(SSL_get_verify_result(ssl) != X509_V_OK)
    {
        std::cerr << "Verification of " << hostWithPort << " failed." << std::endl;
        throw std::exception();
    }
    //
}

void Connection::write(std::string input)
{   
    input += "\r\n";

    // Author: Kenneth Ballard
    // Source: https://developer.ibm.com/tutorials/l-openssl/
    if(BIO_write(bio, input.c_str(), input.length()) <= 0)
    {
        std::cerr << "Read failed." << std::endl;
        throw std::exception();
    }
    //
}

std::string Connection::read() 
{        
    clearBuffer();

    // Author: Kenneth Ballard
    // Source: https://developer.ibm.com/tutorials/l-openssl/
    int x = BIO_read(bio, buffer, BUFFER);
    if(x == 0)
    {
        std::cerr << "Cannot read, connection closed." << std::endl;
        throw std::exception();
    }
    else if(x < 0)
    {
        std::cerr << "Read failed." << std::endl;
        throw std::exception();
    }
    //

    std::string response(buffer);
    return response;
}

std::string Connection::readLine()
{
    std::string response = "";
    std::size_t patternPosition = std::string::npos;
    
    while (patternPosition == std::string::npos)
    {
        response += read();
        patternPosition = response.find(Constants::endOfLine);
    }

    return response;
}

std::string Connection::readMessage()
{
    std::string response = "";
    std::size_t patternPosition = std::string::npos;
    
    response += read();
    Response::checkIfStartsWithOk(response);
    
    while (true)
    {
        patternPosition = response.find(Constants::endOfMessage);
        if (patternPosition != std::string::npos)
        {
            response = Response::removeFirstLine(response);
            response = Response::removeEndDot(response);
            return response;
        }
        response += read();
    }
}

void Connection::establishConnection(Arguments *arguments)
{
    connect(arguments);
    std::string response = readLine();
    Response::checkIfStartsWithOk(response);
}

void Connection::establishSecureConnection(Arguments *arguments)
{
    getCertificates(arguments);
    connectWithSSL(arguments);
    std::string response = readLine();
    Response::checkIfStartsWithOk(response);
}

void Connection::establishSTLSConnection(Arguments *arguments)
{
    std::string response;

    establishConnection(arguments);
    
    write("STLS");
    response = readLine();
    Response::checkIfStartsWithOk(response);

    getCertificates(arguments);
    connectWithSTLS(arguments);
}

// -------------------------------------------------------------------------*/


/* 
 * Response - namespace for processing inputs and outputs from files or server 
 * ---------------------------------------------------------------------------------------------------------------------*/

namespace Response
{
    void checkIfStartsWithOk(std::string response)
    {
        if (response.compare(0,3, "+OK") != 0) 
        {
            std::cerr << "Server returned error: " << response;
            throw std::exception();
        }        
    }

    void checkDirectory(std::string path)
    {
        // Author: Ingo Leonhardt 
        // Source: https://stackoverflow.com/questions/18100097/portable-way-to-check-if-directory-exists-windows-linux-c
        struct stat info;

        if(stat(path.c_str(), &info) != 0)
        {
            std::cerr << "Unable to open directory " << path << std::endl;
            throw std::exception();
        }
        else if(! (info.st_mode & S_IFDIR)) 
        {
            std::cerr << path << " is not a directory." << std::endl;
            throw std::exception();
        }
        //
    }

    std::string removeFirstLine(std::string response)
    {
        std::size_t patternPosition = response.find(Constants::endOfLine);
        if (patternPosition == std::string::npos) return "";
        return response.substr(patternPosition + Constants::endOfLine.length());
    }

    std::string removeEndDot(std::string response)
    {
        std::size_t patternPosition = response.find(Constants::endOfMessage);
        if (patternPosition == std::string::npos) return response;
        return response.substr(0, patternPosition + Constants::endOfLine.length());
    }
    
    std::string findDataInStream(std::istream *stream, std::string prefix)
    {
        std::string line;
        std::size_t position;

        while (std::getline(*stream, line))
        {
            position = line.find(prefix);
            if (position != std::string::npos)
            {
                // Author: cplusplus.com
                // Source: https://www.cplusplus.com/reference/string/string/find_last_not_of/
                std::string whitespaces (" \t\f\v\n\r");
                std::size_t found = line.find_last_not_of(whitespaces);
                
                if (found!=std::string::npos)
                    line.erase(found+1);
                else
                    line.clear();
                //
                
                if (line.length() < position + prefix.length()) line.append(" ");
                return line.substr(position + prefix.length());
            }
        }
        throw std::logic_error("Prefix " + prefix + " not found in the stream");
    }
    
    std::string findIdInMessage(std::string message)
    {
        std::istringstream stream(message);
        std::string messageId;
        try {
            messageId = findDataInStream(&stream, "Message-ID: ");
        }
        catch (std::logic_error& e) {
            try {
                std::istringstream secondStream(message);
                messageId = findDataInStream(&secondStream, "Message-Id: ");
            }
            catch (std::logic_error& e) {
                std::cerr << "Message ID not found in the message." << std::endl;
                throw std::exception();
            }
        }
        return messageId;
    }

    std::string findIdInMarkerFile(std::string path)
    {
        std::ifstream stream(path);
        if (! stream.is_open()) return "";
        std::string messageId = findDataInFile(path, "Last retrieved message ID: ");
        return messageId;
    }

    std::string findDataInFile(std::string path, std::string prefix)
    {
        std::ifstream stream(path);
        std::string data;
        if (! stream.is_open())
        {
            std::cerr << "Unable to open file " << path << std::endl;
            throw std::exception();
        } 
        try {
            data = findDataInStream(&stream, prefix);
        }
        catch (std::logic_error& e) {
            std::cerr << "Prefix \"" + prefix + "\" not found in the file " + path + "." << std::endl;
            throw std::exception();
        }
        stream.close();
        return data;
    }
}


/* 
 * Functions that represents the pop3 client 
 * ---------------------------------------------------------------------------------------------------------------------*/

void logIn(Connection *connection, std::string username, std::string password)
{    
    std::string response;
    
    connection->write("USER " + username);
    response = connection->readLine();
    Response::checkIfStartsWithOk(response);

    connection->write("PASS " + password);
    response = connection->readLine();
    Response::checkIfStartsWithOk(response);
}

int getNumberOfEmails(Connection *connection)
{
    std::regex rgx("\\s\\d\\s");
    std::smatch match;
    
    connection->write("STAT");
    std::string response = connection->readLine();
    Response::checkIfStartsWithOk(response);

    regex_search(response, match, rgx);
    return std::stoi(match[0].str());
}

std::string getLastMessageOnServer(Connection *connection, int lastMessageNumber)
{
    connection->write("RETR " + std::to_string(lastMessageNumber));
    return connection->readMessage(); 
}

void generateOutputMessage(int lastMessageNumber, int actualMessageNumber, bool onlyNew)
{
    int messageCount;
    std::string outputEnd = "";
    
    if (not onlyNew) messageCount = lastMessageNumber;
    else 
    {
        messageCount = lastMessageNumber - actualMessageNumber;
        outputEnd += " new";
        if (messageCount == 0){
            std::cout << "No new messages have been found." << std::endl;
            return;
        }
    }
    outputEnd += messageCount == 1 ? " message." : " messages.";

    std::cout << "You retrieved " << messageCount << outputEnd << std::endl;
}

void storeEmails(Connection *connection, int lastMessageNumber, std::string outputPath, bool onlyNew)
{
    Response::checkDirectory(outputPath);
    if (lastMessageNumber <= 0)
    {   
        std::cout << "Your mailbox is empty." << std::endl;
        return; 
    }    

    std::string lastMessageId = Response::findIdInMessage(getLastMessageOnServer(connection, lastMessageNumber));
    std::string lastRetrievedMessageId = Response::findIdInMarkerFile(outputPath + "/" + MARKERFILE);

    int actualMessageNumber = lastMessageNumber;
    while (actualMessageNumber > 0)
    {
        connection->write("RETR " + std::to_string(actualMessageNumber));
        std::string message = connection->readMessage();
        
        std::string messageId = Response::findIdInMessage(message);

        if (onlyNew and messageId == lastRetrievedMessageId) break; 

        std::string filename = "{" + messageId.substr(1, messageId.length()-2) + "}.txt";

        std::ofstream outputFile(outputPath + "/" + filename);
        outputFile << message;
        outputFile.close();

        actualMessageNumber--;
    }

    std::ofstream updateMarkerFile(outputPath + "/" + MARKERFILE);
    updateMarkerFile << "Last retrieved message ID: " << lastMessageId << std::endl;
    updateMarkerFile.close();

    generateOutputMessage(lastMessageNumber, actualMessageNumber, onlyNew);
}

void deleteEmails(Connection *connection, int lastMessageNumber)
{
    for (int i=1; i<=lastMessageNumber; i++)
    {
        std::string retrieveInput = "DELE " + std::to_string(i);
        connection->write(retrieveInput);
        std::string response = connection->readLine();
        Response::checkIfStartsWithOk(response);
    }
}

void quitSession(Connection *connection)
{
    connection->write("QUIT");
    std::string response = connection->readLine();
    Response::checkIfStartsWithOk(response);
}


/* 
 * Main function - handles whole program a exits it with 1 when an error occures
 * ---------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[]) 
{    
    Connection connection;
    Arguments arguments;
    Auth auth;

    try
    {
        connection.loadOpenSSL();
        arguments.loadArguments(argc, argv);
        auth.loadAuth(arguments.authFilePath); 
        
        if (arguments.isT) connection.establishSecureConnection(&arguments);
        else if (arguments.isS) connection.establishSTLSConnection(&arguments);
        else connection.establishConnection(&arguments);

        logIn(&connection, auth.username, auth.password);

        int lastMessageNumber = getNumberOfEmails(&connection);

        storeEmails(&connection, lastMessageNumber, arguments.outDirPath, arguments.isNew);

        if (arguments.isDelete) deleteEmails(&connection, lastMessageNumber);

        quitSession(&connection);
    }
    catch(std::exception& e)
    {
        return -1;
    }
}
