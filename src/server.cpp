/****************************************
*										*
*	server chat program made with C++	*
*	Made by: Bachir Bouchemla			*
*	Date: Late January 2017				*
*	Purpose: School project				*
*										*
*		(o.0) 01000010 x 2 = ?			*
*										*
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
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <locale>
#include <chrono>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_CLIENTS 32

// Includes the "std" namespace for all objects.
using namespace std;

string programStatusDialogue[] = {
	"Creating a server for Xeon Chat...",
    "Server and socket successfully initialized!",
    "Server bind complete!",
    "Searching for active servers...",
    "Connected!\n\n",
    "Disconnecting! Goodbye..."
};

string errorList[] = {
	"Error: Could not create a server.",
    "Error: Could not bind, a connection already exists.",
    "Error: Could not connect to server",
    "Error: Could not create thread..."
};

string command = "";

bool isExit = false;

const string exitCommand   = "/exit";
const string kickCommand   = "/kick";

typedef struct
{
    char username[64];
    uint32_t user_client;
    uint32_t user_id;
    struct sockaddr_in addr;
} client;

client *clients[MAX_CLIENTS];

void *Type(void*);

// Gets the current time of this device.
string GetCurrentTime()
{
	string currentTime 	= "";

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

// (Miscenalleous) Prevents me to write "cout" everytime.
void PrintMessage(string text)
{
    cout << text << endl;

    fflush(stdout);
    
    WaitForSeconds(1);
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

// Clears the current window
void ClearWindow()
{
	cout << "\033[H\033[J";
}

void *Type(void*)
{
    while (!isExit)
    {
        printf("\033[0;0H");
        printf("\r[%s] > ", "Command");

        getline(std::cin, command);

        if (command != "")
            SendMessage(command, "Xeon Chat");
        else if (command == exitCommand)
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

            isExit = true;
        }
        else if (command == kickCommand)
            SendMessage("This feature is comming soon!", "Xeon Chat");
    }
}

void AddClient(client* c)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            clients[i] = c;
            return;
        }
    }
}

void DeleteClient(client* c)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            if (clients[i] -> user_id == user_id)
            {
                clients[i] = NULL;
                return;
            }
        }
    }
}

void SendMessage(string text, string username)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            write(clients[i] -> user_client, message, sizeof(message));
        }
    }

    printf("%s [%s]: %s\r\n", 
        GetCurrentTime().c_str(), username.c_str(), text.c_str());

    fflush(stdout);

    WaitForSeconds(1);
}

void *HandleClient(void* client_arg)
{
    uint32_t client = 0;

    char buffer[1024];

    client c = (client*)client_arg;

    while ((client = read(c -> user_client, buffer, sizeof(buffer))) > 0)
    {

    }
}

int main(int argc, char** argv)
{
    ClearWindow();
    PrintAsciiArt();

    uint32_t server, client;
    uint32_t port_number = 7337;

    struct sockaddr_in server_sock, client_sock;

    server = socket(AF_INET, SOCK_STREAM, 0);

    PrintMessage(programStatusDialogue[0]);

    if (server < 0)
    {
        PrintMessage(errorList[0]);

        return EXIT_FAILURE;
    }

    PrintMessage(programStatusDialogue[1]);

    server_sock.sin_family          = AF_INET;
    server_sock.sin_addr.s_addr     = htons(INADDR_ANY);
    server_sock.sin_port            = htons(port_number);

    if ((bind(server, (struct sockaddr*)&server_sock, sizeof(server_sock))) < 0)
    {
        PrintMessage(errorList[1]);

        return EXIT_FAILURE;
    } 

    PrintMessage(programStatusDialogue[2]);
    PrintMessage(programStatusDialogue[3]);

    if (listen(server, 1) < 0)
    {
        PrintMessage(errorList[3]); 

        return EXIT_FAILURE;
    }

    while (1)
    {
        socklen_t client_size = sizeof(client_sock);
        client = accept(server, (struct sockaddr*)&client_sock, &client_size);

        client *c = (client*)malloc(sizeof(client));
        c -> addr = client_sock;
        c -> user_id = user_id++;
        c -> user_client = client;

        AddClient(c);

        PrintMessage(programStatusDialogue[4]);

        pthread_t typer;
        pthread_t connection;

        pthread_create(&connection, NULL, &HandleClient, void(*)c)

        if (pthread_create(&typer, NULL, Type, NULL) < 0)
            PrintMessage(errorList[3]);
    }

    PrintMessage(programStatusDialogue[5]);

    close(server);

    return EXIT_SUCCESS;
}