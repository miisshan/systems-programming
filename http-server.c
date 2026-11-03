#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#define PORT 8080
#define BUFFER_LENGTH 1024

int main(int argc, char *argv[])
{
    WSADATA wsaData;

    int WSA_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock_handle = INVALID_SOCKET;

    struct sockaddr_in server_address = {0};

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    int bind_result = SOCKET_ERROR;
    int listen_result = SOCKET_ERROR;
    int bytes_received = 0;

    char buffer[BUFFER_LENGTH] = {0};
    char method[10] = {0};
    char target[20] = {0};

    if (WSA_result == 0)
    {
        printf("Winsock initialized succesfully!\n");
        sock_handle = socket(AF_INET, SOCK_STREAM, 0);

        if (sock_handle == INVALID_SOCKET)
        {
            printf("Socket creation failed!\n");
        }
        else
        {
            printf("Socket created successfully!\n");
            bind_result = bind(sock_handle, (struct sockaddr *)&server_address, sizeof(server_address));
            if (bind_result == SOCKET_ERROR)
            {
                printf("Binding Failed : %d\n", WSAGetLastError());
            }
            else
            {
                printf("Bind successful \n");

                listen_result = listen(sock_handle, 5);
                if (listen_result == SOCKET_ERROR)
                {
                    printf("Listen failed : %d \n", WSAGetLastError());
                }
                else
                {
                    printf("Listening successful\n");
                    SOCKET client_socket = accept(sock_handle, NULL, NULL);
                    if (client_socket == INVALID_SOCKET)
                    {
                        printf("Accept failed : %d \n", WSAGetLastError());
                    }
                    else
                    {
                        printf("Client connected\n");
                        bytes_received = recv(client_socket, buffer, BUFFER_LENGTH - 1, 0);
                        if (bytes_received > 0)
                        {
                            buffer[bytes_received] = '\0';
                            printf("%s", buffer);

                            char request_line[BUFFER_LENGTH] = {0};
                            int request_line_found = 0;

                            for (int i = 0; i < bytes_received; i++)
                            {
                                if (buffer[i] == '\r')
                                {
                                    request_line[i] = '\0';
                                    request_line_found = 1;
                                    break;
                                }
                                request_line[i] = buffer[i];
                            }
                            if (request_line_found == 1)
                            {
                                for (int i = 0; i < sizeof(method) / sizeof(char); i++)
                                {
                                    if (request_line[i] == ' ')
                                    {
                                        request_line[i] = '\0';
                                        break;
                                    }
                                    method[i] = request_line[i];
                                }
                                if (strcmp(method, "GET") == 0)
                                {
                                    for (int i = sizeof(method) / sizeof(char) - 1; i < sizeof(request_line) / sizeof(char); i++)
                                    {
                                        if (request_line[i] == '/' || request_line[i] == ' ')
                                        {
                                            continue;
                                        }
                                        else
                                        {
                                            target[i] = request_line[i];
                                            if (request_line[i] == '\r')
                                            {
                                                target[i] = '\0';
                                                break;
                                            }
                                        }
                                    }
                                    printf("The target is %s", target);
                                }
                                printf("The method is %s", method);
                            }
                            else
                            {
                                printf("Failed to find the request\n");
                            }
                        }
                        else
                        {
                            if (bytes_received == 0)
                            {
                                printf("Client disconnected gracefully\n");
                            }
                            else
                            {
                                printf("Bytes not received: %d", WSAGetLastError());
                            }
                        }
                        closesocket(client_socket);
                    }
                }
            }
        }
    }
    else
    {
        printf("Winsock initialization failed!");
    }

    if (WSA_result == 0)
    {
        if (sock_handle != INVALID_SOCKET)
        {
            closesocket(sock_handle);
        }

        WSACleanup();
    }

    return 0;
}