#include <cstring> // Para usar memset
#include <iostream>
#include <netinet/in.h> // Para usar estruturas de endereço de internet
#include <ostream>
#include <sys/socket.h> // Para usar sockets
#include <unistd.h>     // Para usar close

const int PORT = 8080;

void handleRequest(int socket, const char *path) {
  const char *response;

  if (strcmp(path, "/about") == 0) {
    response = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "\r\n"
               "<h1>About Us</h1><p>This is the about page.</p>";
  } else if (strcmp(path, "/contact") == 0) {
    response = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "\r\n"
               "<h1>Contact Us</h1><p>This is the contact page.</p>";
  } else if (strcmp(path, "/") == 0) {
    response = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "\r\n"
               "<h1>Hello from server</h1>";
  } else {
    response = "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: text/html\r\n"
               "\r\n"
               "<h1>404 Not Found</h1><p>The page you are looking for does not "
               "exist.</p>";
  }

  send(socket, response, strlen(response), 0);
}

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};

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

  while (true) {
    // Aceitando uma conexão
    new_socket =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
      perror("Erro ao aceitar conexão");
      continue; // Continue to accept new connections
    }

    read(new_socket, buffer, 1024);
    std::cout << "Mensagem recebida do cliente: " << buffer << std::endl;

    // Parse the request to extract the path
    char method[10], path[100];
    sscanf(buffer, "%s %s", method, path); // Extract method and path

    // Handle the request
    handleRequest(new_socket, path);

    std::cout << "Mensagem enviada para o cliente" << std::endl;

    close(new_socket); // Fecha o socket do cliente após enviar a resposta
  }

  close(server_fd);
  return 0;
}
