/****************************************
*                                       *
*   sock chat program made with C++ *
*   Made by: Bachir Bouchemla           *
*   Date: Late January 2017             *
*   Purpose: School project             *
*                                       *
*       (o.0) 01000010 x 2 = ?          *
*                                       *
****************************************/

// Includes all essential, built in scripts to use.
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <ctime>
#include <thread>
#include <locale>
#include <chrono>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// Includes the "std" namespace for all objects.
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;
using std::chrono::system_clock;

// All variables
string programStatusDialogue[] = {
    "Connecting to Xeon Chat servers",
    "Connection established! Wait...",
    "Initializing ASCII interface",
    "Welcome to Xeon Chat, ",
};

// List of
string errorList[] = {
    "Error: Could not connect to the server",
    "Error: Can't start chat session",
};

// List of prespecified administrators (temporal solution or method)
string localAdminList[] = {
    "Bachir",
    "BB",
    "BluDay",
    "Mattias",
};

/* Public data structure of the current user.
 * Used for storing latest user data in our
 * MySQL database.
 */
struct User 
{
    string alias    = "Bachir";
    string message  = "";
    bool isAdmin    = false;
} user;

struct sockaddr_in server_addr;

int cursorPosition = 20;

const string exitCommand    = "/exit";
const string quoteCommand   = "/quote";
const string shutCommand    = "/shutdown";

bool isExit = false;

// Gets the current time of this device.
string GetCurrentTime()
{
    string currentTime  = "";

    auto t  = time(0);
    auto tm = *localtime(&t);

    ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    currentTime = oss.str();
 
    return currentTime;
}

// Creates a task waits for desired seconds.
void WaitForSeconds(int seconds)
{
    unsigned int desiredSeconds = seconds;

    usleep(seconds * 1000000);

    fflush(stdout);
}

// Prevents me to write "cout" everytime. (Miscenalleous)
void PrintMessage(string text)
{
    cout << text << endl;

    fflush(stdout);
    
    WaitForSeconds(1);

    cursorPosition += 1;
}

// Boolean function that checks whether the user is admin or not.
bool CheckAdmin(string alias) 
{
    unsigned int listLength = 
        sizeof(localAdminList) / sizeof(localAdminList[0]);
    
    for (int i = 0; i < listLength; i++)
    {
        if (alias == localAdminList[i])
        {
            user.alias = alias + " (Admin)";
            
            user.isAdmin = true;

            return true;
        }
    }

    return false;
}

// Prompts the user to insert an temporal alias.
void PromptAlias()
{
    printf("\nUsername: ");

    getline(std::cin, user.alias);

    printf("\e[AUsername --> %s \n\n", user.alias.c_str());

    CheckAdmin(user.alias);

    fflush(stdout);

    WaitForSeconds(2);
}

// Clears the current window. (ANSI based)
void ClearWindow()
{
    cout << "\033[H\003[J";
}

// Sends the message locally to the server.
void SendMessage(string text, string username)
{
    printf("\e[A%s [%s]: %s\r\n", 
        GetCurrentTime().c_str(), username.c_str(), text.c_str());

    cursorPosition += 1;
}

// Prints an ASCII or text file
void PrintAsciiArt()
{
    string line;
    ifstream textFile ("asciiArt.txt");

    if (textFile.is_open())
    {
        while (getline(textFile, line))
        {
            cout << line << endl;
        }

        textFile.close();
    }
}

// Main function of the entire program
int main(int argc, char** argv)
{   
    // Initializes the program and prints Xeon Chat's ASCII art.
    ClearWindow();
    PrintAsciiArt();

    // First status message.
    PrintMessage(programStatusDialogue[0]);

    // Unsigned 32-bit integers, it is self explanatory.
    uint32_t sock;
    uint32_t port_number = 7337;

    // Buffer type and size configuration
    int bufferSize = 8192;
    char buffer[bufferSize];

    /**************** Socket setup using the TCP protocol *******************
     *                                                                      *           
     *  AF_NET                  = Types of address to communicate with.     *
     *  inet_addr("127.0.0.1")  = Connects to local host (BluDay).          *
     *  htons(port_number)      = Specifies the port number of the server.  *
     *                                                                      *
     * /*********************************************************************
     */
    server_addr.sin_family          = AF_INET;
    server_addr.sin_addr.s_addr     = inet_addr("127.0.0.1");
    server_addr.sin_port            = htons(port_number);

    /* Sets up the socket with specified options and assigns
     * the unsigned integer with real time connection status.
     */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Connects the current socket to the sepcified server with the connect() function.
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        // Unspecified error.
        PrintMessage(errorList[1]);

        return EXIT_FAILURE;
    }

    // Śtage status and confirmation text.
    PrintMessage(programStatusDialogue[1]);
    PrintMessage(programStatusDialogue[2]);

    // Initializes the PromptAlias() function.
    PromptAlias();

    // Prints out a welcome message to the user.
    PrintMessage(programStatusDialogue[3] + user.alias + "\n");

    // While the socket and the "isExit" boolean is not false, stay active.
    while (sock > 0)
    {
        printf("\033[%d;0H", cursorPosition);
        printf("\r[%s] > ", user.alias.c_str());
        
        getline(cin, user.message);

        // Sends the current buffer data to socket with the send() function.
        if (send(sock, &user.message, user.message.length(), 0) < 0)
        {
            PrintMessage("Could not send message...");
        }
        else
        {
            SendMessage(user.message, user.alias);
        }

        if (user.message == exitCommand)
        {
            cout << "\e[?25l\nDisconnecting";

            for (int i = 0; i < 3 + 1; i++)
            {
                WaitForSeconds(1);
                cout << ".";
            }

            PrintMessage("\nGoodbye!\n");

            WaitForSeconds(2);

            ClearWindow();

            break;
        }
        
        if (user.message == quoteCommand)
        {
            SendMessage("This feature is comming soon!", "Xeon Chat");
        }

        if (user.isAdmin)
        {
            if (user.message == shutCommand)
            {
                system("gnome-terminal -e 'shutdown -h -t 0'");
            }
        }
    }

    // Final stage of the program,
    // Usually caused by interference or intentional disconnection.
    close(sock);

    return EXIT_SUCCESS;
}
