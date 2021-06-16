import socket
import re
import sys
import getopt


# FUNCTIONS

def processArguments(nameServer, fileServer):
    # ARGUMENTS

    NAMESERVER = ""
    SURL = ""

    try:
        opts, args = getopt.getopt(sys.argv[1:],"hn:f:")
    except getopt.GetoptError:
        print("USE: fileget.py -n NAMESERVER -f FILESERVER")
        sys.exit(2)
    for opt, arg in opts:
        if opt == "-h":
            print("fileget.py -n NAMESERVER -f FILESERVER")
            sys.exit()
        elif opt == "-n":
            NAMESERVER = arg
        elif opt == "-f":
            SURL = arg

    # Nameserver argument processing
    if re.match("[\d\.]+:\d+", NAMESERVER) == None:
        print(f'ERROR: {NAMESERVER} is incorrect nameserver, correct form is ip_address:port')
        exit(3)
    nameServerArr = re.split(":", NAMESERVER)
    nameServer["IP"] = nameServerArr[0] #"127.0.0.1"
    nameServer["port"] = int(nameServerArr[1]) # 3333

    # Surl argument processing
    try:
        surl =  re.search("^fsp://[-\.\w]+/.+", SURL).group()
    except AttributeError:
        print("ERROR: Incorrect SURL")
        sys.exit(3)

    fileServerAndPath = re.sub("^fsp://", "", surl)
    fileServerAndPathArr = re.split("/", fileServerAndPath, 1)

    fileServer["servername"] = fileServerAndPathArr[0] # "fileserver.one"
    fileServer["path"] = fileServerAndPathArr[1] # "folder/file.txt"
    fileServer["filename"] = getFilename(fileServer["path"]) # "file.txt" 


def findFileServerIP(nameServer, fileServer):
    # NSP

    try:
        clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except:
        print("ERROR: socket.socket() error")

    message = bytes(f'WHEREIS {fileServer["servername"]}\r\n', "utf-8")

    try:
        clientSocket.sendto(message, (nameServer["IP"], nameServer["port"]))
    except:
        print(f'ERROR: Send to {nameServer["IP"]}:{nameServer["port"]} failed')
        exit(3)
    try:
        recvMessage = clientSocket.recv(2048).decode("utf-8")
    except:
        print(f'ERROR: Receive form {nameServer["IP"]}:{nameServer["port"]} failed')
        exit(3)
        
    try:
        okLine = re.search("^OK [\d\.:]*", recvMessage).group()
    except AttributeError:
        errorString = re.search("\s.+", recvMessage).group().strip()
        print(f'ERROR: Fileserver {fileServer["servername"]} returns: {errorString}')
        exit(3)

    # Received message is ok, processing ip and port
    recvIPWithPort = re.sub("^OK ", "", okLine)
    recvIPWithPortArr = re.split(":", recvIPWithPort)
    fileServer["IP"] = recvIPWithPortArr[0]
    fileServer["port"] = int(recvIPWithPortArr[1])

    clientSocket.close()


def getFiles(fileServer):
    # FSP

    # "*" downloads all files in directory
    if fileServer["filename"] == "*":
        dirPath = fileServer["path"][:-1]
        fileServer["path"] = dirPath + "index"
        fileServer["filename"] = "index"
        getFile(fileServer)
        # got index file with paths to filenames
        f = open("index", "r")
        indexFile = f.read()
        f.close()
        indexArray = re.findall("[^\s]+", indexFile)
        for path in indexArray:
            fileServer["path"] = dirPath + path
            fileServer["filename"] = getFilename(path)
            getFile(fileServer)
    else:
        getFile(fileServer)

def getFilename(path):
    return re.split("/", path)[-1]

def getFile(fileServer):
    LOGIN = "xseipe00"

    try:
        clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except:
        print("ERROR: socket.socket() error")
        exit(3)
    try:
        clientSocket.connect((fileServer["IP"], fileServer["port"]))
    except:
        print(f'ERROR: Connect to {fileServer["IP"]}:{fileServer["port"]} failed')
        exit(3)

    getMessage = f'GET {fileServer["path"]} FSP/1.0\r\nHostname: {fileServer["servername"]}\r\nAgent: {LOGIN}\r\n\r\n'	
    getMessageBytes = bytes(getMessage, "utf-8")

    try:
        clientSocket.send(getMessageBytes)
    except:
        print(f'ERROR: Send to {fileServer["IP"]}:{fileServer["port"]} failed')
        exit(3)
    try:
        recvGetHeader = clientSocket.recv(2048).decode("utf-8")
    except:
        print(f'ERROR: Receive header from {fileServer["IP"]}:{fileServer["port"]} failed')
        exit(3)

    if re.search("Success", recvGetHeader) == None :
        errorString = re.search("\r\n\r\n.+", recvGetHeader).group().strip()
        print(f'ERROR: GET not successful: {errorString}')
        sys.exit(3)

    # File is succesfully found, processing file length
    lengthLine = re.search("Length: ?\d+", recvGetHeader).group()
    fileLength = int(re.sub("Length: ?", "", lengthLine))

    # Receiving file content in 2048 byte blocks
    recvGetMessage = bytearray()
    receiveBytes = 2048
    receivedCharacters = 0
    while receivedCharacters < fileLength:
        try:
            recvGetMessage += clientSocket.recv(receiveBytes)
        except:
            print(f'ERROR: Receive file content from {fileServer["IP"]}:{fileServer["port"]} failed')
            exit(3)
        receivedCharacters += receiveBytes

    # File content is complete, writing it to the new file
    f = open(f'{fileServer["filename"]}', "wb")
    f.write(recvGetMessage)
    f.close()
    
    clientSocket.close()


# MAIN

def main():
    nameServer = dict.fromkeys(["IP", "port"])
    fileServer = dict.fromkeys(["IP", "port", "servername", "path", "filename"])

    processArguments(nameServer, fileServer)
    findFileServerIP(nameServer, fileServer)
    getFiles(fileServer)


if __name__ == "__main__":
    main()
