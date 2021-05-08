#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080

struct sockaddr_in address;
int sock = 0, valread;

int main()
{
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    while (1)
    {
        char menu[255], id[255], password[255];
        memset(menu, 0, sizeof(menu));
        char idPassword[1024] = {0};

        printf("--Main Menu--\n");
        printf("1. register\n");
        printf("2. login\n");
        printf("Your Choice (register / login / exit ) : ");
        scanf("%s", menu);
        send(sock, menu, strlen(menu), 0);
        valread = read(sock, buffer, 1024);

        if (strcmp(buffer, "register") == 0)
        {
            printf("\n--Register--\n");
            printf("id: ");
            scanf("%s", id);
            printf("\npassword: ");
            scanf("%s", password);
            sprintf(idPassword, "%s:%s\n", id, password);
            send(sock, idPassword, strlen(idPassword), 0);
            printf("\nregister success\n\n");
            memset(buffer, 0, sizeof(buffer));
        }

        else if (strcmp(buffer, "login") == 0)
        {
            printf("\n--Login--\n");
            printf("id: ");
            scanf("%s", id);
            printf("\npassword: ");
            scanf("%s", password);
            sprintf(idPassword, "%s:%s\n", id, password);
            send(sock, idPassword, strlen(idPassword), 0);
            read(sock, buffer, 1024);
            if (strcmp(buffer, "LoginSuccess") == 0)
                printf("Logged in\n\n");
            else if (strcmp(buffer, "LoginFailed:idPasswordWrong") == 0)
                printf("Login failed, check your id or password\n\n");

            memset(buffer, 0, sizeof(buffer));
        }
    }
    return 0;
}