#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080

int server_fd, new_socket, valread;
struct sockaddr_in address;
int addrlen = sizeof(address);
int usageCount = 0, currentUsage = 0, socketNum[1024];
int userNow = 0;

void stopApp(int sock)
{
    usageCount--;
    printf("Current user request stop.\n");
    userNow++;
    send(socketNum[userNow], "available", strlen("available"), 1024);
    printf("User switch.\n");
}

void send_file(FILE *fp, int sockfd)
{
    int n;
    char data[1024] = {0};

    while (fgets(data, 1024, fp) != NULL)
    {
        printf("%s\n", data);
        // if (send(sockfd, data, sizeof(data), 0) == -1)
        // {
        //     perror("[-]Error in sending file.");
        //     exit(1);
        // }
        // bzero(data, 1024);
        // send(sockfd, data, strlen(data), 0);
    }
    send(sockfd, "halo", sizeof("halo"), 0);
}

void menuApp(int sock, char *id, char *password)
{
    char buffer[1024] = {0};
    valread = read(sock, buffer, 1024);

    if (strcmp(buffer, "exit") == 0)
    {
        printf("Stopping service for user ...\n");
        stopApp(new_socket);
    }

    if (strcmp(buffer, "add") == 0)
    {
        printf("\nUser choose add application.\n");

        char publisher[255] = {0}, tahunPublikasi[255] = {0}, filePath[255] = {0};
        valread = read(sock, publisher, 1024);
        valread = read(sock, tahunPublikasi, 1024);
        valread = read(sock, filePath, 1024);

        FILE *file;
        file = fopen("files.tsv", "a");
        fprintf(file, "%s\t%s\t%s\n", publisher, tahunPublikasi, filePath);
        fclose(file);

        file = fopen("running.log", "a");
        char *filenameWithoutFolder = filePath + 6;
        fprintf(file, "Tambah: %s (%s:%s)\n", filenameWithoutFolder, id, password);
        fclose(file);

        file = fopen(filePath, "w");
        fprintf(file, "%s\t%s\n", publisher, tahunPublikasi);
        fclose(file);

        printf("File has been saved in this server.\n");
    }

    if (strcmp(buffer, "download") == 0)
    {
        char downloadFileName[255] = {0};
        int found = 0;
        char msg[255];
        valread = read(sock, downloadFileName, 1024);
        char downloadFileNameWithPath[255] = {0};

        FILE *file = fopen("files.tsv", "r");
        char line[255];
        while (fgets(line, 255 - 1, file))
        {
            line[strcspn(line, "\n")] = 0;

            char fileNameSearch[255] = {0};
            strcpy(fileNameSearch, strtok(line, "\t"));
            strcpy(fileNameSearch, strtok(NULL, "\t"));
            strcpy(fileNameSearch, strtok(NULL, "\t"));

            strcpy(downloadFileNameWithPath, "FILES/");
            strcat(downloadFileNameWithPath, downloadFileName);

            if (strcmp(downloadFileNameWithPath, fileNameSearch) == 0)
            {
                found = 1;
            }
        }

        if (found == 1)
        {
            sleep(1);
            FILE *fp = fopen(downloadFileNameWithPath, "r");
            send_file(fp, sock);
            printf("File download successfully.\n");
        }

        // if (found == 1)
        // {
        //     sprintf(msg, "found");
        //     send(sock, msg, strlen(msg), 0);
        //     printf("File download successfully.\n");
        // }
        // else
        // {
        //     sprintf(msg, "notfound");
        //     send(sock, msg, strlen(msg), 0);
        //     printf("File not found.\n");
        // }
    }

    if (strcmp(buffer, "delete") == 0)
    {
        char deleteFileName[255] = {0};
        valread = read(sock, deleteFileName, 1024);

        FILE *file, *fileTemp;
        file = fopen("files.tsv", "r+");
        fileTemp = fopen("temp.tsv", "w");

        char data[1024] = {0};
        char publisher[255], tahunPublikasi[255], filePath[255];

        while (fgets(data, 1024, file) != NULL)
        {
            sscanf(data, "%[^\t]\t%s\t%s", publisher, tahunPublikasi, filePath);

            char *fileName = filePath + 6;

            if (strcmp(fileName, deleteFileName) != 0)
            {
                fprintf(fileTemp, "%s", data);
            }

            bzero(data, 1024);
        }
        fclose(file);
        fclose(fileTemp);
        remove("files.tsv");
        rename("temp.tsv", "files.tsv");

        file = fopen("running.log", "a+");

        fprintf(file, "Hapus: %s (%s:%s)\n", deleteFileName, id, password);
        fclose(file);

        char fullPathFileName[300];
        sprintf(fullPathFileName, "FILES/%s", deleteFileName);

        char deletedPathFileName[300];
        sprintf(deletedPathFileName, "FILES/old-%s", deleteFileName);

        rename(fullPathFileName, deletedPathFileName);
    }

    if (strcmp(buffer, "see") == 0)
    {
        FILE *file;
        file = fopen("files.tsv", "r+");

        char lineFile[1024] = {0};
        char publisher[255], tahunPublikasi[255], filename[255];

        char data[100000];

        while (fgets(lineFile, 1024, file) != NULL)
        {
            sscanf(lineFile, "%[^\t]\t%s\t%s", publisher, tahunPublikasi, filename);
            char outputName[300], outputPublisher[300], outputYearOfPublish[300], outputExtension[300], outputFilepath[300];

            sprintf(outputPublisher, "Publisher: %s\n", publisher);
            sprintf(outputYearOfPublish, "Tahun publishing: %s\n", tahunPublikasi);
            sprintf(outputFilepath, "Filepath : %s\n", filename);

            char *extGet = strrchr(filename, '.');
            char *extensionType = extGet + 1;
            sprintf(outputExtension, "Ekstensi File : %s\n", extensionType);

            char pathWithoutExtensionFile[255];
            strcpy(pathWithoutExtensionFile, filename);
            char justNameOfFile[255];
            pathWithoutExtensionFile[strlen(pathWithoutExtensionFile) - strlen(extGet)] = '\0';
            sscanf(pathWithoutExtensionFile, "FILES/%s", justNameOfFile);
            sprintf(outputName, "Nama: %s\n", justNameOfFile);

            strcat(data, outputName);
            strcat(data, outputPublisher);
            strcat(data, outputYearOfPublish);
            strcat(data, outputExtension);
            strcat(data, outputFilepath);
            strcat(data, "\n");
        }
        send(sock, data, strlen(data), 0);
    }

    if (strcmp(buffer, "find") == 0)
    {
        char fileNameRequest[255] = {0};
        valread = read(sock, fileNameRequest, 1024);

        FILE *file;
        file = fopen("files.tsv", "r+");

        char line[1024] = {0};
        char publisher[255], tahunPublikasi[255], fileName[255];

        char data[100000];

        while (fgets(line, 1024, file) != NULL)
        {
            sscanf(line, "%[^\t]\t%s\t%s", publisher, tahunPublikasi, fileName);
            char outputName[300], outputPublisher[300], outputYearOfPublishing[300], outputExtension[300], outputFilePath[300];
            sprintf(outputPublisher, "Publisher: %s\n", publisher);
            sprintf(outputYearOfPublishing, "Tahun publishing: %s\n", tahunPublikasi);
            sprintf(outputFilePath, "Filepath : %s\n", fileName);

            char *extensionGet = strrchr(fileName, '.');
            char *extension = extensionGet + 1;
            sprintf(outputExtension, "Ekstensi File : %s\n", extension);

            char fullPathWithoutExt[100];
            strcpy(fullPathWithoutExt, fileName);
            char onlyName[100];
            fullPathWithoutExt[strlen(fullPathWithoutExt) - strlen(extensionGet)] = '\0';
            sscanf(fullPathWithoutExt, "FILES/%s", onlyName);
            sprintf(outputName, "Nama: %s\n", onlyName);

            if (strstr(onlyName, fileNameRequest) != 0)
            {
                strcat(data, outputName);
                strcat(data, outputPublisher);
                strcat(data, outputYearOfPublishing);
                strcat(data, outputExtension);
                strcat(data, outputFilePath);
                strcat(data, "\n");
            }
        }
        if (strlen(data) == 0)
        {
            send(sock, "File not found.", strlen("File not found."), 0);
        }
        else
        {
            send(sock, data, strlen(data), 0);
        }
    }

    menuApp(sock, id, password);
}

void *authApp(void *arg)
{
    new_socket = *(int *)arg;
    char buffer[1024] = {0};
    valread = read(new_socket, buffer, 1024);

    if (strcmp(buffer, "exit") == 0)
    {
        printf("Stopping service for user ...\n");
        stopApp(new_socket);
    }

    if (strcmp(buffer, "login") == 0 || strcmp(buffer, "register") == 0)
    {
        char id[1024] = {0}, password[1024] = {0};
        int findAccountStatus = 0;
        valread = read(new_socket, id, 1024);
        valread = read(new_socket, password, 1024);
        char authMsg[1024];

        if (strcmp(buffer, "register") == 0)
        {
            FILE *file;
            file = fopen("akun.txt", "a");
            fprintf(file, "%s:%s\n", id, password);

            sprintf(authMsg, "registerSuccess");
            send(new_socket, authMsg, strlen(authMsg), 0);
            fclose(file);
            printf("user register success.\n");
            authApp(&new_socket);
        }

        if (strcmp(buffer, "login") == 0)
        {
            char buffer[1024];
            FILE *file;
            file = fopen("akun.txt", "r");

            while (fgets(buffer, 1024, file) != NULL)
            {
                char temp_id[1024], temp_password[1024];
                char *temp = strtok(buffer, ":");
                strcpy(temp_id, temp);
                temp = strtok(NULL, "\n");
                strcpy(temp_password, temp);

                if (strcmp(temp_id, id) == 0 && strcmp(temp_password, password) == 0)
                {
                    findAccountStatus = 1;
                }
            }

            fclose(file);

            if (findAccountStatus == 1)
            {
                sprintf(authMsg, "loginSuccess");
                send(new_socket, authMsg, strlen(authMsg), 0);
                printf("user login success\n");
                menuApp(new_socket, id, password);
            }
            else
            {
                sprintf(authMsg, "loginFailed");
                send(new_socket, authMsg, strlen(authMsg), 0);
                printf("user login failed.\n");
                authApp(&new_socket);
            }
        }
    }
    else
    {
        authApp(&new_socket);
        pthread_cancel(pthread_self());
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

    mkdir("FILES", 0777);

    pthread_t threads[1024];

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        socketNum[currentUsage] = new_socket;
        printf("New user connected %d\n", usageCount);

        if (usageCount > 0)
        {
            printf("Sending usageStatus to client: %d\n", currentUsage);
            send(socketNum[currentUsage], "notAvailable", strlen("notAvailable"), 1024);
            pthread_create(&threads[currentUsage], NULL, authApp, &new_socket);
        }
        else
        {
            printf("Sending usageStatus to client: %d\n", currentUsage);
            send(socketNum[currentUsage], "available", strlen("available"), 1024);
            pthread_create(&threads[currentUsage], NULL, authApp, &new_socket);
        }
        usageCount++;
        currentUsage++;
    }
    return 0;
}