#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080

struct sockaddr_in address;
int sock = 0, valread;

void write_file(int sockfd, char *filename)
{
    int n;
    FILE *fp;
    char buffer[1024] = {0};
    // valread = read(sockfd, buffer, 1024);
    // printf("%s\n", buffer);

    sleep(1);
    fp = fopen(filename, "w");
    // fprintf(fp, "%s",buffer);
    fprintf(fp, "testdata");
    fclose(fp);
    return;
}

void findApp(int sock)
{
    send(sock, "find", strlen("find"), 0);

    char filename[255];
    scanf("%s", filename);

    sleep(1);
    send(sock, filename, strlen(filename), 0);

    char buffer[100000] = {0};
    int valread;
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
}

void seeApp(int sock)
{
    send(sock, "see", strlen("see"), 0);

    char buffer[100000] = {0};
    int valread;
    valread = read(sock, buffer, 1024);

    printf("%s\n", buffer);
}

void deleteApp(int sock)
{
    send(sock, "delete", strlen("delete"), 0);
    char filename[255];
    scanf("%s", filename);

    sleep(1);
    send(sock, filename, strlen(filename), 0);
    printf("Deleted successfully.\n");
}

void downloadApp(int sock)
{
    send(sock, "download", strlen("download"), 0);

    char fileName[255];
    scanf("%s", fileName);

    sleep(1);
    send(sock, fileName, strlen(fileName), 0);

    char msg[1024] = {0};
    valread = read(sock, msg, 1024);

    char fileNameWithPath[255] = {0};
    strcpy(fileNameWithPath, "FILES/");
    strcat(fileNameWithPath, fileName);

    write_file(sock, fileNameWithPath);
    printf("File downloaded successfully.\n");

    // if (strcmp(msg, "found") == 0)
    // {
    //     sleep(1);
    //     FILE *file = fopen(fileNameWithPath, "w");
    //     printf("File downloaded successfully.\n");
    // }

    // else
    // {
    //     printf("File not found.\n");
    // }
}

void addApp(int sock)
{
    send(sock, "add", strlen("add"), 0);
    char publisher[255], tahunPublikasi[255], filePath[255];

    printf("\n--Add Files Application--\n");
    printf("Publisher : ");
    getchar();
    scanf("%[^\n]s", publisher);
    send(sock, publisher, strlen(publisher), 0);
    printf("Tahun Publikasi : ");
    getchar();
    scanf("%[^\n]s", tahunPublikasi);
    send(sock, tahunPublikasi, strlen(tahunPublikasi), 0);
    printf("File Path : ");
    getchar();
    scanf("%[^\n]s", filePath);
    send(sock, filePath, strlen(filePath), 0);

    sleep(1);

    printf("File has been saved to server.\n");
}

void menuApp(int sock)
{
    char menu[255], id[255], pass[255];
    printf("\n--Main Menu--\n");
    printf("1. add\n");
    printf("2. download\n");
    printf("3. delete\n");
    printf("4. see\n");
    printf("5. find\n");
    printf("6. exit\n");
    printf("Your choice [write menu name] : ");

    scanf("%s", menu);

    if (strcmp(menu, "exit") == 0)
    {
        send(sock, "exit", strlen("exit"), 0);
        exit(0);
    }

    if (strcmp(menu, "add") == 0)
    {
        addApp(sock);
    }

    if (strcmp(menu, "download") == 0)
    {
        downloadApp(sock);
    }

    if (strcmp(menu, "delete") == 0)
    {
        deleteApp(sock);
    }

    if (strcmp(menu, "see") == 0)
    {
        seeApp(sock);
    }

    if (strcmp(menu, "find") == 0)
    {
        findApp(sock);
    }

    menuApp(sock);
}

void authApp(int sock)
{
    char menu[255], id[255], password[255];

    printf("--Auth Menu--\n");
    printf("1. register\n");
    printf("2. login\n");
    printf("3. exit\n");
    printf("Your Choice (register / login / exit ) : ");
    scanf("%s", menu);

    if (strcmp(menu, "exit") == 0)
    {
        send(sock, "exit", strlen("exit"), 0);
        exit(0);
    }

    if (!strcmp(menu, "register") == 0 && !strcmp(menu, "login") == 0)
    {
        authApp(sock);
        return;
    }

    send(sock, menu, strlen(menu), 0);

    printf("Input your account information.\n");
    printf("id: ");
    scanf("%s", id);
    send(sock, id, strlen(id), 0);
    printf("\npassword: ");
    scanf("%s", password);
    send(sock, password, strlen(password), 0);

    printf("Waiting for server response ...\n");
    char authMsg[1024] = {0};
    valread = read(sock, authMsg, 1024);

    if (strcmp(authMsg, "registerSuccess") == 0)
    {
        printf("Register successful, your account has been created.\n");
        authApp(sock);
    }
    else if (strcmp(authMsg, "loginSuccess") == 0)
    {
        printf("Logged in.\n");
        menuApp(sock);
    }
    else
    {
        printf("Login Failed, check your id or password again!\n");
        authApp(sock);
    }
}

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

    mkdir("FILES", 0777);

    while (1)
    {
        char usageStatus[1024] = {0};
        printf("Checking usage status on server ...\n");
        valread = read(sock, usageStatus, 1024);

        if (strcmp(usageStatus, "available") == 0)
        {
            authApp(sock);
        }
        else
        {
            printf("Please wait until server is available ...\n");
        }
    }
    return 0;
}