#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <linux/limits.h>

int mycwd() {
  // char *getcwd(char buf[.size], size_t size);
  char cwd[PATH_MAX];
  if(getcwd(cwd, sizeof(cwd))!= NULL){
    printf("Diretório atual: %s\n", cwd);
    return 0;
  } else {
    perror("getcwd() error\n");
    return 1;
  }
}

int mymkdir() {
  // int mkdir(const char *pathname, mode_t mode);
  char path[PATH_MAX];
  printf("Entre com um nome de diretório para ser criado: ");
  scanf("%s", path);
  if(mkdir(path, 0777) == 0){
  printf("Diretório %s criado com sucesso\n", path);
  return 0;
  } else {
    perror("mkdir() error");
    return 1;
  }
}

int myrmdir() {
  // int rmdir(const char *pathname);
  char path[PATH_MAX];
  printf("Entre com o diretório a ser deletado: ");
  scanf("%s", path);
  if(rmdir(path) == 0){
    printf("Diretório %s removido com sucesso!\n", path);
    return 0;
  } else {
    perror("rmdir() error");
    return 1;
  }
}

int mycd() {
  // int chdir(const char *path);
  char path[PATH_MAX];
  printf("Entre com o caminho do diretório: ");
  scanf("%s", path);
  if (chdir(path) == 0){
    printf("Diretório alterado para '%s'\n",path);
    return 0;
  } else {
    perror("chdir() error");
    return 1;
  }
  
}

int mystat() {
  // int stat(const char *restrict pathname, struct stat *restrict statbuf);
  struct stat daniel;
  char path[PATH_MAX];
  printf(" Entre com o caminho do arquivo/diretório: ");
  scanf("%s", path);
  if (stat(path, &daniel) == 0){
    printf("Informação de arquivo par: %s\n", path);
    printf("Tamanho: %ld bytes\n", daniel.st_size);
    printf("Permissões: %o\n", daniel.st_mode & 0777);
    printf("Ultima vez acessado: %ld\n", daniel.st_atime);
    printf("Ultima vez modificado: %ld\n", daniel.st_mtime);
    
    return 0;
  } else {
    perror("stat() error");
    return 1;
  }
}

int myls() {
  // DIR *opendir(const char *name);
  // struct dirent *readdir(DIR *dirp);
  // int closedir(DIR *dirp);
    char path[PATH_MAX];
    printf("Entre com o caminho do diretório (ou . para o atual): ");
    scanf("%s", path);
    
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir() error");
        return 1;
    }
    
    printf("Conteudos do diterório '%s':\n", path);
    struct dirent *roberto;
    while ((roberto = readdir(dir)) != NULL) {
        printf("%s\n", roberto->d_name);
    }
    
    closedir(dir);
    return 0;
}


int main(int argc, char** argv) {
  int test = 0;
  while(test == 0) {
    char in[60];
    printf("myshell> ");
    scanf("%s", &in);
  
    if(strcmp(in, "exit") == 0) {
      test = 1;
    } else if(strcmp(in, "cwd") == 0) {
      mycwd();
    } else if(strcmp(in, "mkdir") == 0) {
      mymkdir();
    } else if(strcmp(in, "rmdir") == 0) {
      myrmdir();
    } else if(strcmp(in, "cd") == 0) {
      mycd();
    } else if(strcmp(in, "stat") == 0) {
      mystat();
    } else if(strcmp(in, "ls") == 0) {
      myls();
    } else {
      printf("Comando não encontrado\n");
    }
  }
}
