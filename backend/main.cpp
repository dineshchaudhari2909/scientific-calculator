#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#include "arithmetic.h"

using namespace std;

const size_t MAX_REQUEST_SIZE = 16384;
const size_t MAX_BODY_SIZE = 8192;

#ifdef _WIN32
void initNetworking() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed with error: " << result << endl;
        exit(1);
    }
}

void cleanupNetworking() {
    WSACleanup();
}
#else
void initNetworking() {
    // No initialization needed for POSIX sockets
}

void cleanupNetworking() {
    // No cleanup needed for POSIX sockets
}
#endif

void sendResponse(
    socket_t client,
    const string& status,
    const string& contentType,
    const string& body)
{
    string response;

    response += "HTTP/1.1 " + status + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " +
                to_string(body.size()) +
                "\r\n";

    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

#ifdef _WIN32
    send(
        client,
        response.c_str(),
        static_cast<int>(response.size()),
        0
    );
#else
    send(
        client,
        response.c_str(),
        response.size(),
        0
    );
#endif
}

string getJsonValue(
    const string& json,
    const string& key)
{
    string searchKey = "\"" + key + "\"";

    size_t keyPosition = json.find(searchKey);

    if (keyPosition == string::npos)
    {
        throw invalid_argument("Missing parameter: " + key);
    }

    size_t colonPosition =
        json.find(':', keyPosition + searchKey.size());

    if (colonPosition == string::npos)
    {
        throw invalid_argument("Invalid JSON format");
    }

    size_t quoteStart =
        json.find('"', colonPosition + 1);

    if (quoteStart == string::npos)
    {
        throw invalid_argument("Invalid JSON format");
    }

    size_t quoteEnd =
        json.find('"', quoteStart + 1);

    if (quoteEnd == string::npos)
    {
        throw invalid_argument("Invalid JSON format");
    }

    return json.substr(
        quoteStart + 1,
        quoteEnd - quoteStart - 1
    );
}

bool validNumber(const string& value)
{
    if (value.empty())
    {
        return false;
    }

    size_t start = 0;
    if (value[0] == '-')
    {
        if (value.size() == 1)
        {
            return false;
        }
        start = 1;
    }

    for (size_t i = start; i < value.size(); ++i)
    {
        if (value[i] < '0' || value[i] > '9')
        {
            return false;
        }
    }

    return true;
}

void handleCalculate(
    socket_t client,
    const string& body)
{
    try
    {
        if (body.empty())
        {
            throw invalid_argument("Empty request body");
        }

        string a = getJsonValue(body, "a");
        string operation = getJsonValue(body, "operation");

        if (a.empty())
        {
            throw invalid_argument("Parameter 'a' cannot be empty");
        }

        if (operation.empty())
        {
            throw invalid_argument("Parameter 'operation' cannot be empty");
        }

        if (!validNumber(a))
        {
            throw invalid_argument("Invalid number format for parameter 'a'");
        }

        string result;

        if (operation == "+")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = add(a, b);
        }
        else if (operation == "-")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = subtract(a, b);
        }
        else if (operation == "*")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = multiply(a, b);
        }
        else if (operation == "/")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = divide(a, b);
        }
        else if (operation == "%")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = modulo(a, b);
        }
        else if (operation == "^")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = power(a, b);
        }
        else if (operation == "!")
        {
            result = factorial(a);
        }
        else if (operation == "gcd")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = gcd(a, b);
        }
        else if (operation == "lcm")
        {
            string b = getJsonValue(body, "b");

            if (b.empty())
            {
                throw invalid_argument("Parameter 'b' cannot be empty");
            }

            if (!validNumber(b))
            {
                throw invalid_argument("Invalid number format for parameter 'b'");
            }

            result = lcm(a, b);
        }
        else
        {
            throw invalid_argument(
                "Unsupported operation: " + operation
            );
        }

        string response =
            "{\"result\":\"" +
            result +
            "\"}";

        sendResponse(
            client,
            "200 OK",
            "application/json",
            response
        );
    }
    catch (const exception& e)
    {
        string response =
            "{\"error\":\"" +
            string(e.what()) +
            "\"}";

        sendResponse(
            client,
            "400 Bad Request",
            "application/json",
            response
        );
    }
}

void handleRequest(socket_t client)
{
    char buffer[MAX_REQUEST_SIZE];

#ifdef _WIN32
    int received = recv(
        client,
        buffer,
        sizeof(buffer) - 1,
        0
    );
#else
    ssize_t received = recv(
        client,
        buffer,
        sizeof(buffer) - 1,
        0
    );
#endif

    if (received <= 0)
    {
        return;
    }

    if (static_cast<size_t>(received) >= MAX_REQUEST_SIZE)
    {
        sendResponse(
            client,
            "413 Payload Too Large",
            "application/json",
            "{\"error\":\"Request too large\"}"
        );
        return;
    }

    buffer[received] = '\0';

    string request(buffer);

    // CORS preflight
    if (request.find("OPTIONS ") == 0)
    {
        sendResponse(
            client,
            "204 No Content",
            "text/plain",
            ""
        );

        return;
    }

    // POST /calculate
    if (request.find("POST /calculate ") == 0)
    {
        size_t bodyPosition =
            request.find("\r\n\r\n");

        if (bodyPosition == string::npos)
        {
            sendResponse(
                client,
                "400 Bad Request",
                "application/json",
                "{\"error\":\"Invalid request\"}"
            );

            return;
        }

        string body =
            request.substr(bodyPosition + 4);

        // Check for Content-Length header and read additional data if needed
        size_t contentLengthPos = request.find("Content-Length:");
        if (contentLengthPos != string::npos)
        {
            size_t contentLengthStart = contentLengthPos + 15;
            size_t contentLengthEnd = request.find("\r\n", contentLengthStart);
            if (contentLengthEnd != string::npos)
            {
                string contentLengthStr = request.substr(contentLengthStart, contentLengthEnd - contentLengthStart);
                try
                {
                    int contentLength = stoi(contentLengthStr);
                    int bodyReceived = static_cast<int>(body.size());

                    // If we haven't received the full body, read more
                    while (bodyReceived < contentLength && bodyReceived < static_cast<int>(MAX_BODY_SIZE))
                    {
                        char additionalBuffer[MAX_BODY_SIZE];
#ifdef _WIN32
                        int additionalReceived = recv(
                            client,
                            additionalBuffer,
                            sizeof(additionalBuffer) - 1,
                            0
                        );
#else
                        ssize_t additionalReceived = recv(
                            client,
                            additionalBuffer,
                            sizeof(additionalBuffer) - 1,
                            0
                        );
#endif

                        if (additionalReceived <= 0)
                        {
                            break;
                        }

                        additionalBuffer[additionalReceived] = '\0';
                        body += additionalBuffer;
                        bodyReceived += additionalReceived;
                    }
                }
                catch (...)
                {
                    // Invalid Content-Length, ignore
                }
            }
        }

        // Trim any trailing whitespace or newlines
        while (!body.empty() && (body.back() == '\r' || body.back() == '\n' || body.back() == ' '))
        {
            body.pop_back();
        }

        if (body.size() > MAX_BODY_SIZE)
        {
            sendResponse(
                client,
                "413 Payload Too Large",
                "application/json",
                "{\"error\":\"Request body too large\"}"
            );
            return;
        }

        handleCalculate(client, body);

        return;
    }

    // Health check
    if (request.find("GET /health ") == 0)
    {
        sendResponse(
            client,
            "200 OK",
            "application/json",
            "{\"status\":\"C++ backend running\"}"
        );

        return;
    }

    sendResponse(
        client,
        "404 Not Found",
        "text/plain",
        "Not Found"
    );
}

int main()
{
    initNetworking();

    // Get port from environment variable or use default
    const char* portEnv = std::getenv("PORT");
    int port = portEnv ? std::atoi(portEnv) : 8080;

    if (port <= 0 || port > 65535) {
        cerr << "Invalid port number. Using default 8080." << endl;
        port = 8080;
    }

    cerr << "Starting server on port " << port << endl;

    socket_t serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        IPPROTO_TCP
    );

    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "Socket creation failed." << endl;
        cleanupNetworking();
        return 1;
    }

    // Set socket options to reuse address
#ifdef _WIN32
    const char reuseAddr = 1;
#else
    int reuseAddr = 1;
#endif
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cerr << "Bind failed. Port " << port << " may be in use." << endl;
        closesocket(serverSocket);
        cleanupNetworking();
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR)
    {
        cerr << "Listen failed." << endl;
        closesocket(serverSocket);
        cleanupNetworking();
        return 1;
    }

    cerr << "Server successfully bound and listening" << endl;

    cout << "====================================" << endl;
    cout << " Scientific Calculator C++ Backend" << endl;
    cout << "====================================" << endl;
    cout << "Backend: http://0.0.0.0:" << port << endl;
    cout << "Health : http://0.0.0.0:" << port << "/health" << endl;
    cout << endl;
    cout << "Supported:" << endl;
    cout << "+   Addition" << endl;
    cout << "-   Subtraction" << endl;
    cout << "*   Multiplication" << endl;
    cout << "/   Division" << endl;
    cout << "%   Modulo" << endl;
    cout << "^   Power" << endl;
    cout << "!   Factorial" << endl;
    cout << "gcd Greatest Common Divisor" << endl;
    cout << "lcm Least Common Multiple" << endl;
    cout << endl;

    while (true)
    {
        socket_t clientSocket = accept(
            serverSocket,
            nullptr,
            nullptr
        );

        if (clientSocket == INVALID_SOCKET)
        {
            continue;
        }

        handleRequest(clientSocket);

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    cleanupNetworking();

    return 0;
}