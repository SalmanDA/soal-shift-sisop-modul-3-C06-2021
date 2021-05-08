#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 8080

int server_fd, new_socket, valread;
struct sockaddr_in address;
int addrlen = sizeof(address);
FILE *fp;

int checkAccount(char idPassword[])
{
    FILE *file = fopen("akun.txt", "r");
    if (file != NULL)
    {
        char lineFile[256];
        while (fgets(lineFile, sizeof lineFile, file) != NULL)
        {
            if (strcmp(lineFile, idPassword) == 0)
            {
                fclose(file);
                return 1;
            }
        }
        fclose(file);
    }

    return 0;
}

void *app(void *arg)
{
    int new_socket;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char buffer[1024] = {0};
    char new_buffer[1024] = "login";
    while (1)
    {
        char idPassword[255] = {0};
        memset(buffer, 0, sizeof(buffer));
        valread = read(new_socket, buffer, 1024);

        if (strcmp(buffer, "register") == 0)
        {
            fp = fopen("akun.txt", "a");
            send(new_socket, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer));

            read(new_socket, buffer, 1024);
            fputs(buffer, fp);
            fclose(fp);
            printf("register success\n");
        }

        else if (strcmp(buffer, "login") == 0)
        {
            send(new_socket, new_buffer, strlen(new_buffer), 0);
            read(new_socket, buffer, 1024);
            char login_auth_msg[100];
            if (checkAccount(buffer))
                strcpy(login_auth_msg, "LoginSuccess");
            else
                strcpy(login_auth_msg, "LoginFailed:idPasswordWrong");
            puts(login_auth_msg);

            send(new_socket, login_auth_msg, strlen(login_auth_msg), 0);
            memset(buffer, 0, sizeof(buffer));
        }
    }
}

int main(int argc, char const *argv[])
{
    int opt = 1;
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[1024];
    for (int i = 0; i < 1024; i++)
    {
        int *new_val = &i;
        pthread_create(&threads[i], NULL, app, (void *)new_val);
    }

    for (int i = 0; i < 1024; i++)
        pthread_join(threads[i], NULL);

    return 0;
}