// gcc -Wall copy.c -o copy
// strace ./copy copy.c
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define SIZE 128

int myopen(const char *filename, int flags, int buffersize);
ssize_t myread(int fd, void *buf);
ssize_t mywrite(int fd, const void *buf, size_t count);
int myclose(int fd);

// Registradores
// r: 64 bits
// e: 32 bits

// __asm__(AssemblerTemplate : OutputOperands : InputeOperands)

// Saída
// "=": sobrescreve valor existente
// "+": para leitura e escrita

// Entrada
// "r": registrador
// "m": memória

int myopen(const char *filename, int flags, int buffersize)
{
  // return open(filename, flags);
  int fd = 0;

  __asm__("mov %0, %%rdi" : : "r"(filename));
  __asm__("mov %0, %%esi" : : "r"(flags));
  __asm__("mov %0, %%edx" : : "r"(buffersize));
  __asm__("mov $2, %rax");
  __asm__("syscall");
  __asm__("mov %%eax, %0" : "=r"(fd) :);

  return fd;
}

ssize_t myread(int fd, void *buf)
{ 
  ssize_t resposta = 0;
  // return read(fd, buf, size)

  __asm__("mov %0, %%edi" : : "r"(fd));
  __asm__("mov %0, %%rsi" : : "r"(buf));
  __asm__("mov %0, %%edx" : : "r"(SIZE));
  __asm__("mov $0, %rax");
  __asm__("syscall");
  __asm__("mov %%rax, %0" : "=r"(resposta));

    
  return resposta;
}

ssize_t mywrite(int fd, const void *buf, size_t count)
{
  //return write(fd, buff, count);
  ssize_t ret = 0;
  __asm__("mov %0, %%edi" : : "r"(fd));
  __asm__("mov %0, %%rsi" : : "r"(buf));
  __asm__("mov %0, %%rdx" : : "r"(count));
  __asm__("mov $1, %rax");
  __asm__("syscall");
  __asm__("mov %%rax, %0" : "=r"(ret));


  return ret;
}

int myclose(int fd)
{
  int buabua = 0;
  __asm__("mov %0, %%edi" : : "r"(fd));
  __asm__("mov $3, %rax");
  __asm__("syscall");
  __asm__("mov %%eax, %0" : "=r"(buabua));

  return buabua;
}

int main(int argc, char **argv)
{

  int file;
  char buf[SIZE];
  ssize_t readCount;

  if (argc != 2)
  {
    fprintf(stderr, "correct usage: %s <filename>\n", argv[0]);
    return 1;
  }

  file = myopen(argv[1], O_RDONLY, SIZE);
  if (file < 0)
  {
    perror("file open");
    return 1;
  }

  while ((readCount = myread(file, buf)) > 0)
  {
    if ((mywrite(STDOUT_FILENO, buf, readCount) != readCount))
    {
      perror("write");
      return 1;
    }
  }

  myclose(file);
  return 0;
}
