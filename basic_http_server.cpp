#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<fstream>
#include<string>

/**
 * @file basic_http_server.cpp
 * @brief A basic HTTP server capable of serving static files.
 *
 * This server listens on port 8000, accepts connections, and serves HTML and other static files.
 * If the requested file is not found, it returns a 404 error message.
 */

// Constants for the server configuration
#define PORT 8000                // Server port to listen on.
#define BUFFER_SIZE 1024          // Buffer size for reading incoming requests.
#define MAX_CONNECTIONS 10        // Maximum number of backlog connections the server can handle.

/**
 * @brief Extracts the HTTP method from the client's request.
 *
 * The HTTP method is the first word in the request string (e.g., "GET").
 *
 * @param req The raw HTTP request as a string.
 * @return A string containing the HTTP method (e.g., "GET", "POST").
 */

std::string get_method(const std::string& req) {
    int space_pos = req.find(" ");
    return req.substr(0, space_pos); // Return the method (GET, POST, etc.)
}

/**
 * @brief Extracts the requested file path from the HTTP request.
 *
 * This function parses the HTTP request and retrieves the path that the client is requesting.
 * For example, in "GET /index.html HTTP/1.1", it will return "index.html".
 *
 * @param req The raw HTTP request as a string.
 * @return The requested file path as a string. Returns an empty string if an error occurs.
 */

std::string get_path(const std::string& req) {
    try {
        int first_space = req.find(" ") + 2; // Ignore the "/" after the method
        int next_space = req.find(" ", first_space); // Find the next space after the path
        return req.substr(first_space, next_space - first_space); // Extract path
    } catch (const std::exception& e) {
        std::cerr << "Error extracting path: " << e.what() << std::endl;
        return "";
    }
}

/**
 * @brief Reads and returns the content of the requested file.
 *
 * This function reads the file corresponding to the provided `filepath` and returns its content as a string.
 * If the file is not found, it throws a runtime exception.
 *
 * @param filepath The path to the requested file.
 * @return The file's content as a string.
 */

std::string render(const std::string& filepath) {
    std::string html;
    try {
        std::ifstream fin(filepath); // Open the file for reading
        if (!fin) {
            throw std::runtime_error("File not found: " + filepath); // Throw an error if the file is not found
        }

        char ch;
        while (fin.get(ch)) {
            html += ch; // Append each character to the HTML string
        }
        fin.close();
    } catch (const std::exception& e) {
        std::cerr << "Error rendering file: " << e.what() << std::endl;
    }

    return html;
}

int main() {
    // Create a TCP socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: IPv4, SOCK_STREAM: TCP
    if (server_fd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // Set socket options to allow reusing the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options" << std::endl;
        close(server_fd);
        return -1;
    }

    // Define the server address (IPv4, listen on any interface, port 8000)
    sockaddr_in address;
    address.sin_family = AF_INET;              // IPv4
    address.sin_addr.s_addr = INADDR_ANY;      // Listen on all interfaces
    address.sin_port = htons(PORT);            // Convert port to network byte order
    int addrlen = sizeof(address);

    // Bind the socket to the address and port
    if (bind(server_fd, (sockaddr*)&address, addrlen) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        std::cerr << "Error in listen" << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "Server started on localhost:" << PORT << std::endl;

    // Server loop to accept and handle client requests
    while (true) {
        // Accept a new client connection
        int new_sock = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_sock < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            continue; // Skip to the next iteration if connection fails
        }

        // Read the client's request into a buffer
        char buffer[BUFFER_SIZE] = {0};
        int read_data_len = read(new_sock, buffer, BUFFER_SIZE);

        // Convert the buffer into a string for easier manipulation
        std::string request(buffer);

        // Get the HTTP method and the requested file path
        std::string method = get_method(request);
        std::string path = get_path(request);

        // Set default path to "index.html" if no specific file is requested
        if (path.empty()) {
            path = "index.html";
        }

        // Initialize status and load the requested file
        std::string status = "200";
        std::string html = render(path);

        // If the file is not found, return a 404 error message
        if (html.empty()) {
            html = "<h1 style='color:red;'>404 Page Not Found</h1>";
            status = "404";
            path = "index.html"; // Ensure extension type is HTML
        }

        // Extract the file extension to set the content type (e.g., HTML, CSS, JS)
        int dot_pos = path.find(".");
        dot_pos++; // Move past the dot
        int next_space = path.find(" ", dot_pos);
        std::string ext = path.substr(dot_pos, next_space - dot_pos);

        // Prepare the HTTP response headers and body
        std::string response =
            "HTTP/1.1 " + status + " OK\r\n"
            "Content-Type: text/" + ext + "\r\n"
            "Connection: close\r\n\r\n" +
            html;

        // Send the response to the client
        send(new_sock, response.c_str(), response.length(), 0);

        // Close the socket for this client
        close(new_sock);
    }

    // Close the main server socket when shutting down
    close(server_fd);

    return 0;
}
