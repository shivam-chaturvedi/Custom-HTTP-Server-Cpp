# Basic HTTP Server in C++

This repository contains a simple HTTP server implementation in C++ that listens on port 8000 and serves static HTML files. The server is designed to run on Linux systems, utilizing Linux-specific libraries.

## Features

- **Serve Static Files:** Handles HTTP requests and serves static HTML and other files.
- **Error Handling:** Returns a 404 error message if the requested file is not found.
- **Simple Implementation:** Provides a clear example of basic HTTP server functionality.

## Prerequisites

- Linux-based operating system (e.g., Ubuntu, Fedora)
- C++ compiler (e.g., `g++`)
- Basic understanding of TCP/IP networking

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/shivam-chaturvedi/Custom-HTTP-Server-Cpp
cd basic-http-server
```

### 2. Build the Server

Compile the server using `g++`. Ensure you have the necessary development tools installed:

```bash
g++ -o basic_http_server basic_http_server.cpp
```

### 3. Run the Server

Run the server executable. The server listens on port 8000:

```bash
./basic_http_server
```

You should see the following output:

```
Server started on localhost:8000
```

### 4. Test the Server

Open your web browser and navigate to:

```
http://localhost:8000
```

If you have an `index.html` file in the same directory as the server executable, it will be served. Otherwise, the server will return a 404 error.

## Code Overview

- **`basic_http_server.cpp`:** Contains the implementation of the HTTP server. Key components include:
  - **Socket Creation:** `socket()`, `setsockopt()`
  - **Binding and Listening:** `bind()`, `listen()`
  - **Handling Requests:** `accept()`, `read()`, `send()`
  - **File Handling:** `std::ifstream`

## Notes

- This server is intended for educational purposes and provides a basic example of how to build an HTTP server in C++.
- The server runs exclusively on Linux as it relies on Linux-specific system libraries (`sys/socket.h`, `unistd.h`, etc.).

## Contributing

Feel free to submit pull requests, report issues, or suggest improvements. For significant changes or features, please open an issue to discuss your ideas.

