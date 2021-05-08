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

    char menu[255], id[255], password[255];
    memset(menu, 0, sizeof(menu)); 
    char idPassword[1024] = {0};

    printf("--Main Menu--\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("Your Choice (1/2) : ");
    scanf("%s", menu);
    send(sock, menu, strlen(menu), 0);
    valread = read(sock, buffer, 1024);

    // printf("%s\n", buffer);

    if (strcmp(buffer, "register") == 0)
    {
        printf("\n--Register--\n");
        printf("id: ");
        scanf("%s", id);
        printf("\npassword: ");
        scanf("%s", password);
        sprintf(idPassword, "%s:%s\n", id, password);
        send(sock, idPassword, strlen(idPassword), 0);
        printf("\nregister success\n");
    }

    else if (strcmp(buffer, "login") == 0) {
        printf("\n--Login--\n");
    }

    else {
        printf("Wrong Choice !\n");
    }
    return 0;
}