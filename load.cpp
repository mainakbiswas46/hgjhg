#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include<bits/stdc++.h>
#include "COMMON_FUNCN.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "jsonstring.h"

using namespace rapidjson;
using namespace std;



#define FILENAME "cs_config.txt"

// Configuration
const string target_server_ip = "127.0.0.1";
const int target_server_port = 3030;
const int num_threads = 2;
const int num_requests_per_thread = 20;
const int pool_size = 20;

// Key-value pool
vector<pair<string, string>> key_value_pool;
vector<pair<string, string>> ip_port_pool;

// Function to initialize the key-value pool with random strings
void initialize_key_value_pool() {
    srand(time(0));
    for (int i = 0; i < pool_size; ++i) {
        string key = "key" + to_string(i);
        string value = "value" + to_string(i);
        key_value_pool.emplace_back(key, value);
    }
}
void initialize_ip_port_pool() {
   ip_port_pool.push_back({"127.0.0.1","3090"});
   ip_port_pool.push_back({"127.0.0.1","3091"});
   ip_port_pool.push_back({"127.0.0.1","3092"});
   ip_port_pool.push_back({"127.0.0.1","3093"});
   ip_port_pool.push_back({"127.0.0.1","3094"});
   ip_port_pool.push_back({"127.0.0.1","3095"});
   ip_port_pool.push_back({"127.0.0.1","3096"});
   ip_port_pool.push_back({"127.0.0.1","3097"});
   ip_port_pool.push_back({"127.0.0.1","3098"});
}

void request_for_get_delete(string type,string key,int sock_fd)
{
   Document document1;
   send_message(sock_fd,get_delete_CS(type,key));
   string value_json=receive_message(sock_fd);
   cout<<"recvd msg: "<<value_json<<endl;
   if(document1.ParseInsitu((char*)value_json.c_str()).HasParseError())
   {
       cout<<"error in request for client parsing string"<<endl;
   }
        
   else if(strcmp(document1["req_type"].GetString(),"data")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value=document1["message"].GetString();
            cout<<"value for the "<<key<<" is "<<value<<endl;


    }

    else if(strcmp(document1["req_type"].GetString(),"ack")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value=document1["message"].GetString();
            cout<<value<<endl;


    }
          


}




//*************************************************do put and update together**************************************//


void request_for_update_put(string type,string key,string value, int sock_fd)
{
  Document document1;
   send_message(sock_fd,put_update_CS(type,key,value));
   string value_json=receive_message(sock_fd);// 1) ack+no_slave_server_active
                                              // 2) ack+put_success  
                                              // 3) ack+commit_failed
   cout<<"recvd msg:: "<<value_json<<endl;
   if(document1.ParseInsitu((char*)value_json.c_str()).HasParseError())

   {
       cout<<"error in request for client parsing string"<<endl;
   }
        
   else if(strcmp(document1["req_type"].GetString(),"data")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value1=document1["message"].GetString();
            cout<<value1<<endl;



    }

    else if(strcmp(document1["req_type"].GetString(),"ack")==0)
   { 
            assert(document1.IsObject());
            assert(document1.HasMember("req_type"));
            assert(document1.HasMember("message"));

            string value1=document1["message"].GetString();
            cout<<value1<<endl;
    }
}
// Worker function to send requests
void worker_function(int worker_id) {
	string ip_address=ip_port_pool[worker_id].first;
	string port_number=ip_port_pool[worker_id].second;
	
	int sock_fd=initialize_socket(ip_address,port_number);

	cout<<"socket id is"<<sock_fd<<endl;
     
    ifstream file(FILENAME);
    string cs_ip,cs_port;

    if (file.is_open()) 
    {	    
	    getline(file, cs_ip);
	    getline(file, cs_port); 
	    file.close();
	}
    cout<<" cs ip "<<cs_ip<<" port"<<cs_port<<endl;
    connect_f(sock_fd,cs_ip,cs_port);
    cout<<"after connect_f "<<endl;
	cout<<receive_message(sock_fd)<<endl;
    cout<<"connected in client"<<endl;
    send_message(sock_fd,identity_string("client"));
    cout<<receive_message(sock_fd)<<endl;
    cout<<"in client ready to serve"<<endl;

    cout<<"command format : command_type:key:value"<<endl;
    for (int i = 0; i < num_requests_per_thread; ++i) {
        int random_index = rand() % pool_size;
        string key = key_value_pool[random_index].first;
        string value = key_value_pool[random_index].second;
        
        // Send requests in sequence
        request_for_update_put("put",key,value,sock_fd);
        request_for_get_delete("get",key,sock_fd);
        request_for_update_put("update",key,"updated",sock_fd); // Send the original value for update to keep it simple
        request_for_get_delete("get",key,sock_fd);
        request_for_get_delete("delete",key,sock_fd);
    }
}

// Main function to send requests using multiple threads
int main() {
    initialize_key_value_pool();
    initialize_ip_port_pool();
    vector<thread> threads;
    auto start_time = chrono::high_resolution_clock::now();

    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker_function, i);
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    double total_requests = num_threads * num_requests_per_thread * 5;
    double response_time_per_request = static_cast<double>(duration) / total_requests;
    double throughput = total_requests / (duration / 1000.0); // 5 requests per key

    cout << "Total time taken: " << duration << " ms" << endl;
    cout << "Throughput: " << throughput << " requests per second" << endl;
    cout << "Average response time per request: " << response_time_per_request << " ms" << endl;

    // Write overall result to file
    ofstream result_file("load_test_results.txt", ios::app);
    if (result_file.is_open()) {
        result_file <<total_requests << duration << "," << throughput << "," << response_time_per_request << "\n";
        result_file.close();
    }
    return 0;
}
