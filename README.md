# HTTP Web Server

## About

**webserv** is a comprehensive network programming project at École 42 that involves building a fully functional HTTP/1.1 web server from scratch in C++. This project explores network programming, socket management, HTTP protocol implementation, and concurrent connection handling while creating a production-ready web server capable of serving static content, handling CGI scripts, and managing multiple simultaneous client connections.

The project provides deep understanding of web server architecture, network protocols, and systems programming while implementing a server that can compete with industry-standard solutions like nginx in basic functionality.

## Learning Objectives

- **Network Programming**: Mastering socket programming and TCP/IP communication
- **HTTP Protocol**: Deep understanding of HTTP/1.1 specification
- **Concurrent Programming**: Managing multiple simultaneous connections
- **I/O Multiplexing**: Using select/poll/epoll for efficient connection handling
- **CGI Implementation**: Executing external scripts and managing processes
- **Configuration Parsing**: Reading and validating complex configuration files
- **Error Handling**: Robust error detection and HTTP status code management
- **Performance Optimization**: Creating efficient, non-blocking server architecture

## Core Features

### HTTP/1.1 Protocol Implementation
- **HTTP Methods**: GET, POST, DELETE support
- **HTTP Headers**: Request and response header parsing and generation
- **Status Codes**: Complete HTTP status code implementation
- **Persistent Connections**: Keep-alive connection support
- **Chunked Transfer**: Chunked encoding for dynamic content
- **Content Negotiation**: MIME type handling and content-type headers

### Server Configuration
- **Location Routing**: URI-based routing and configuration
- **Directory Indexes**: Configurable index files
- **Error Pages**: Custom error page configuration
- **Client Body Size Limits**: Request size restrictions

### Static File Serving
- **File System Access**: Serving files from configured directories
- **Directory Listings**: Auto-index generation for directories
- **MIME Type Detection**: Automatic content-type determination
- **Range Requests**: Partial content delivery support
- **File Caching**: Efficient file serving mechanisms
- **Error Handling**: 404, 403, and other error responses

### CGI Support
- **CGI Execution**: Running CGI scripts (Python, PHP, Perl, etc.)
- **Environment Variables**: Proper CGI environment setup
- **Process Management**: Fork/exec for CGI script execution
- **Input/Output Handling**: Passing request data to CGI and reading response
- **Timeout Management**: Preventing hanging CGI processes
- **Error Handling**: Managing CGI script errors and failures

### Connection Management
- **Non-blocking I/O**: Asynchronous socket operations
- **I/O Multiplexing**: Using select/poll/epoll for scalability
- **Multiple Connections**: Handling hundreds of simultaneous clients
- **Connection Pooling**: Efficient socket resource management
- **Timeout Handling**: Connection and request timeouts
- **Resource Limits**: Managing system resource usage

## HTTP Protocol Implementation

### Request Processing Pipeline
1. **Accept Connection**: Accept incoming TCP connection
2. **Read Request**: Read HTTP request from socket
3. **Parse Request**: Parse request line and headers
4. **Route Request**: Match request URI to location configuration
5. **Process Request**: Execute appropriate handler (static, CGI, etc.)
6. **Generate Response**: Create HTTP response with headers
7. **Send Response**: Write response to client socket
8. **Connection Management**: Keep-alive or close connection

### Request Parser
- **Request Line Parsing**: Method, URI, and HTTP version extraction
- **Header Parsing**: Reading and storing HTTP headers
- **Body Reading**: Handling request body for POST requests
- **Chunked Decoding**: Processing chunked transfer encoding
- **Validation**: Detecting malformed requests

### Response Generator
- **Response Headers**: Content-Type, Content-Length, Date, Server, etc.
- **Body Generation**: Creating response body from file or CGI output
- **Chunked Encoding**: Generating chunked responses when needed
- **Header Management**: Adding appropriate cache and connection headers

## Connection State Management

### Connection States
- **ACCEPTING**: Waiting for new connections
- **READING_REQUEST**: Reading HTTP request from client
- **PROCESSING**: Processing request (static file, CGI, etc.)
- **WRITING_RESPONSE**: Sending response to client
- **KEEP_ALIVE**: Connection idle, waiting for next request
- **CLOSING**: Closing connection

### Timeout Management
- **Connection Timeout**: Maximum idle time for connection
- **Request Timeout**: Maximum time to read complete request
- **Response Timeout**: Maximum time to send response
- **CGI Timeout**: Maximum execution time for CGI scripts
- **Keep-alive Timeout**: Idle timeout for persistent connections

## Error Handling and Status Codes

### Common HTTP Status Codes
- **200 OK**: Successful request
- **201 Created**: Resource created successfully
- **204 No Content**: Successful DELETE
- **301 Moved Permanently**: Permanent redirect
- **302 Found**: Temporary redirect
- **304 Not Modified**: Cached content valid
- **400 Bad Request**: Malformed request
- **403 Forbidden**: Access denied
- **404 Not Found**: Resource not found
- **405 Method Not Allowed**: HTTP method not supported
- **413 Payload Too Large**: Request body too large
- **500 Internal Server Error**: Server error
- **501 Not Implemented**: Feature not implemented
- **502 Bad Gateway**: CGI error
- **503 Service Unavailable**: Server overloaded
- **504 Gateway Timeout**: CGI timeout

## Performance Considerations

### Optimization Techniques
- **Non-blocking I/O**: Preventing blocking operations
- **Event-driven Architecture**: Efficient connection handling
- **Buffer Management**: Optimized buffer sizes and reuse
- **Connection Pooling**: Reusing socket resources
- **File Caching**: Caching frequently accessed files
- **Static File Optimization**: Sendfile for efficient file transfer

### Scalability Features
- **High Connection Limit**: Handling thousands of connections
- **Low Memory Footprint**: Efficient memory usage per connection
- **CPU Efficiency**: Minimal CPU overhead per request
- **Resource Limits**: Preventing resource exhaustion
- **Graceful Degradation**: Handling overload conditions

## Testing and Validation

### Performance Testing
- **Load Testing**: Testing with multiple concurrent connections
- **Stress Testing**: Testing server limits and recovery
- **Benchmark Comparison**: Comparing with nginx, Apache
- **Memory Profiling**: Checking for memory leaks
- **Connection Limits**: Testing maximum connection capacity

### Security Testing
- **Path Traversal**: Testing directory traversal prevention
- **Request Validation**: Testing malformed request handling
- **Resource Limits**: Testing DoS prevention
- **CGI Security**: Testing CGI input validation
- **Header Injection**: Testing header security

## Key Challenges & Solutions

### Non-blocking I/O Complexity
- **Challenge**: Managing asynchronous operations and partial reads/writes
- **Solution**: State machine approach with proper buffer management

### CGI Process Management
- **Challenge**: Managing child processes and preventing zombies
- **Solution**: Proper fork/exec, signal handling, and waitpid usage

### HTTP Protocol Edge Cases
- **Challenge**: Handling all HTTP protocol variations and edge cases
- **Solution**: Thorough specification study and extensive testing

### Configuration Parsing
- **Challenge**: Creating robust, flexible configuration file parser
- **Solution**: State machine parser with comprehensive error reporting

## Skills Demonstrated

- **Network Programming**: Advanced socket programming and TCP/IP
- **Protocol Implementation**: Deep understanding of HTTP specification
- **Concurrent Programming**: Non-blocking I/O and event-driven architecture
- **Systems Programming**: Process management and inter-process communication
- **C++ Development**: Modern C++ with STL and OOP principles
- **Performance Engineering**: Optimizing for scalability and efficiency
- **Testing**: Comprehensive testing strategies and validation
- **Documentation**: Configuration format and API documentation

## Real-World Applications

The knowledge gained from webserv is directly applicable to:
- **Web Development**: Understanding client-server architecture
- **Backend Engineering**: Building scalable server applications
- **DevOps**: Understanding web server configuration and deployment
- **Microservices**: Creating lightweight HTTP services
- **API Development**: Building RESTful API servers
- **Systems Programming**: Network programming for distributed systems

## Notes

The webserv project demonstrates the ability to implement complex network protocols while managing system resources efficiently. It showcases understanding of web technologies from the ground up, providing insight into how production web servers work internally.

The project emphasizes practical software engineering skills including robust error handling, performance optimization, and comprehensive testing. The implementation of features like CGI support and virtual hosts demonstrates professional-level web server capabilities.

Completing webserv provides essential foundation for backend development, systems programming, and understanding the infrastructure that powers the modern web.

---

*Developed as part of the École 42 curriculum - building a production-ready HTTP web server from scratch.*
