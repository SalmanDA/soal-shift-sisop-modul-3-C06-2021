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

### 1B
Pada soal 1B diminta untuk membuat folder FILES untuk menyimpan file, kami menggunakan mkdir untuk membuat foldernya.

```
mkdir("FILES", 0777);
```

### 1C
Pada soal 1C diminta untuk membuat fitur add file kepada server dengan perintah add.

Untuk sisi client sebagai berikut :

```
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
```

Dari kode diatas, untuk files diberi input 3 data, kemudian dikirim melalui socket keserver.

Dari sisi server :

```
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
```

Dari kode diatas server menerima data file, kemudian menulis pada files.tsv dan runnning.log dan save file di folder FILES.

### 1D
Untuk soal 1D diminta untuk mendownload file yang berada pada FILES di server dengan mengecek ke files.tsc apakah file yang ingin didownload valid apa tidak.

### 1E
Untuk soal 1E diminta untuk perintah delete, akan tetapi tidak menghapus file melainkan mengganti nama menjadi old-[$FILES]

Dari sisi client hanya mengirim nama file yang akan dihapus :

```
void deleteApp(int sock)
{
    send(sock, "delete", strlen("delete"), 0);
    char filename[255];
    scanf("%s", filename);

    sleep(1);
    send(sock, filename, strlen(filename), 0);
    printf("Deleted successfully.\n");
}
```

Dari sisi server :

```
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
```

Dari sisi server menerima nama file yang ingin dihapus, kemudian membuka files.tsv dan mengambil data row selain yang ingin didelete untuk dipindah sementara ke temp.tsv , setelah itu temp.tsv di rename menjadi files.tsv setelah files.tsv yang lama dihapus.

### 1F
Untuk soal 1F diminta untuk fitur see melihat seluruh data files yang ada di files.tsv.

Dari sisi client hanya mengirimkan jenis command "see":

```
void seeApp(int sock)
{
    send(sock, "see", strlen("see"), 0);

    char buffer[100000] = {0};
    int valread;
    valread = read(sock, buffer, 1024);

    printf("%s\n", buffer);
}
```

Dari sisi server :

```
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
```

Dari kode server diatas, untuk melihat seluruh data files.tsv looping per line kemudian setiap datanya dikirimkan melalui socket ke UI / Terminal client.

### 1F
Untuk soal 1F diminta untuk fitu search atau find dengan string, kemudian server memindai nama file yang mengandung string yang diinput user dan dioutput isi detail data filenya seperti nama, publisher, year, extension, path.

Dari sisi client mengirim command dan filename ke socket server :

```
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
```

Dari sisi server :

```
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
 ```
 
Pada kode diatas, looping per line dari files.tsv kemudian ambil detail data files per line, jika mengandung string dengan fungsi strstr maka output hasilnya ke client, jika tidak maka beri info ke user bahwa file tidak ditemukan.

### 1H
Untuk soal 1H diminta membuat running log pada saat add dan delete file (ini sudah tertera di penjelasan dan code add & delete).

### Kendala Nomor 1 :
- Pada saat download isi file belum terpindah.
