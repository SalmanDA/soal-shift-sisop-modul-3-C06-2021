# soal-shift-sisop-modul-3-C06-2021

## Laporan Soal Nomor 1 :

### Penjelasan soal
Pada soal ini kami diminta untuk membuat program dengan basis server dan client. untuk feature nya ada register, login, add file, delete file, see file, find file.

### 1A
Untuk soal 1a diminta membuat fitur login dan register, tetapi ketika sudah ada client yang terkoneksi dengan server, maka client lainnya harus menunggu.

Untuk handle koneksi lebih dari satu diserver menggunakan : 

```
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
```

Dari code diatas saya menggunakan perhitungan agar terlihat apakah server sudah ada yang terkoneksi atau belum, kemudian kirim message ke UI / Terminal client.

Untuk pergantian dari user ke user ditandai oleh command exit seperti dibawah ini :

```
if (strcmp(buffer, "exit") == 0)
    {
        printf("Stopping service for user ...\n");
        stopApp(new_socket);
    }
```

```
void stopApp(int sock)
{
    usageCount--;
    printf("Current user request stop.\n");
    userNow++;
    send(socketNum[userNow], "available", strlen("available"), 1024);
    printf("User switch.\n");
}
```

Void stopApp berguna untuk menggeser giliran dan mengubah status user yang sedang mengantri.

Untuk handle koneksi lebih dari satu di sisi client seperti :

```
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
```

Jika available maka akan masuk ke menu auth, jika tidak user harus menunggu giliran.

Untuk login dan register di server menggunakan auth app void seperti dibawah ini :

```
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
```

Dapat dilihat pada kode diatas jika register akan membuat file akun.txt, ketika login akan membaca file akun.txt jika login berhasil maka akan masuk ke main menu atau menuApp void untuk mengakses fitur utama pada program, (di akhir void dilakukan pengiriman status auth untuk user).

Disisi client untuk login dan register :

```
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
```

Dari sisi client pertama input untuk id dan password, kemudian tinggal menunggu auth status / message dari server.
