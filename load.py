import requests
import multiprocessing
import time

# Configuration
target_server_ip = "127.0.0.1"
target_server_port = "3030"
num_processes = 1  # Number of concurrent processes (clients)
num_requests = 4  # Number of requests per process

# Base URL
BASE_URL = f"http://{target_server_ip}:{target_server_port}"

# Test function
def send_request(request_type, key, value=None):
    if request_type == "put":
        data = {"req_type": "put", "key": key, "value": value}
        response = requests.post(BASE_URL, json=data)
    elif request_type == "get":
        data = {"req_type": "get", "key": key}
        response = requests.get(BASE_URL, params=data)
    elif request_type == "delete":
        data = {"req_type": "delete", "key": key}
        response = requests.delete(BASE_URL, json=data)
    elif request_type == "update":
        data = {"req_type": "update", "key": key, "value": value}
        response = requests.put(BASE_URL, json=data)
    else:
        return

    # Output the response for debugging purposes
    print(f"Request: {request_type}, Key: {key}, Response: {response.status_code}, Message: {response.text}")

# Load generation worker function
def load_test_worker(worker_id):
    print(f"Worker {worker_id} started")
    for i in range(num_requests):
        request_type = ["put", "get", "update", "delete"][i % 4]
        key = f"key_{worker_id}_{i}"
        value = f"value_{worker_id}_{i}"

        if request_type in ["put", "update"]:
            send_request(request_type, key, value)
        else:
            send_request(request_type, key)

        time.sleep(0.01)  # Small delay to simulate real-world usage

    print(f"Worker {worker_id} finished")

# Main function to start load test
def start_load_test():
    processes = []

    for i in range(num_processes):
        process = multiprocessing.Process(target=load_test_worker, args=(i,))
        processes.append(process)
        process.start()

    for process in processes:
        process.join()

if __name__ == "__main__":
    start_load_test()

