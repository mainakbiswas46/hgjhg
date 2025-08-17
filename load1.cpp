#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

// Configuration
const string target_server_ip = "127.0.0.1";
const int target_server_port = 3030;

// Function to send requests
void send_request(const string& request_type, const string& key, const string& value = "") {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(target_server_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, target_server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported" << endl;
        return;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return;
    }

    // Prepare JSON data
    string json_data;
    if (request_type == "put" || request_type == "update") {
        json_data = "{\"req_type\": \"" + request_type + "\", \"key\": \"" + key + "\", \"value\": \"" + value + "\"}";
    } else if (request_type == "get" || request_type == "delete") {
        json_data = "{\"req_type\": \"" + request_type + "\", \"key\": \"" + key + "\"}";
    } else {
        close(sock);
        return;
    }

    // Send data to server
    send(sock, json_data.c_str(), json_data.length(), 0);
    cout << "Request sent: " << json_data << endl;

    // Receive response from server
    int valread = read(sock, buffer, 1024);
    if (valread > 0) {
        cout << "Response: " << buffer << endl;
    } else {
        cout << "No response from server" << endl;
    }

    // Close the socket
    close(sock);
}

// Main function to send 5 requests
int main() {
    cout << "Sending 5 requests to the server..." << endl;

    send_request("put", "key1", "value1");
    send_request("get", "key1");
    send_request("update", "key1", "new_value1");
    send_request("get", "key1");
    send_request("delete", "key1");

    cout << "Requests completed." << endl;
    return 0;
}

