#include <cstring> // Para usar memset
#include <iostream>
#include <netinet/in.h> // Para usar estruturas de endereço de internet
#include <ostream>
#include <sys/socket.h> // Para usar sockets
#include <unistd.h>     // Para usar close

const int PORT = 8080;

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

  const char *hello = "HTTP/1.1 400 OK\r\n"
                      "\r\n"
                      "Hello from server";

  // Criando o descritor do socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Falha ao criar socket");
    return -1;
  }

  // Configurando o socket para reuso de endereço e porta
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Erro ao configurar socket");
    close(server_fd);
    return -1;
  }

  // Configurando o endereço e a porta do socket
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Associando o socket a porta
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Erro no bind");
    close(server_fd);
    return -1;
  }

  // Iniciando a escuta no socket
  if (listen(server_fd, 3) < 0) {
    perror("Erro ao escutar");
    close(server_fd);
    return -1;
  }

  std::cout << "Servidor em espera de conexões..." << std::endl;

  // Aceitando uma conexão
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    perror("Erro ao aceitar conexão");
    close(server_fd);
    return -1;
  }

  read(new_socket, buffer, 1024);
  std::cout << "Mensagem recebida do cliente: " << buffer << std::endl;

  // Enviando resposta ao cliente
  send(new_socket, hello, strlen(hello), 0);
  std::cout << "Mensagem enviada para o cliente" << std::endl;

  close(new_socket);
  close(server_fd);
  return 0;
}
