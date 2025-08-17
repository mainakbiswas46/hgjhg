import matplotlib.pyplot as plt

# Function to read results from a text file
def read_results(filename):
    runs = []
    response_times = []
    throughputs = []
    
    with open(filename, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if len(parts) == 3:
                run, response_time, throughput = parts
                runs.append(int(run))
                response_times.append(float(response_time))
                throughputs.append(float(throughput))
    
    return runs, response_times, throughputs

# Function to plot the graphs
def plot_graphs(runs, response_times, throughputs):
    plt.figure(figsize=(14, 6))

    # Plotting Response Time vs. Number of Requests
    plt.subplot(1, 2, 1)
    plt.plot(runs, response_times, marker='o', linestyle='-', color='b')
    plt.xlabel('Number of Requests')
    plt.ylabel('Response Time (ms)')
    plt.title('Response Time vs. Number of Requests')
    plt.grid(True)

    # Plotting Throughput vs. Number of Requests
    plt.subplot(1, 2, 2)
    plt.plot(runs, throughputs, marker='o', linestyle='-', color='r')
    plt.xlabel('Number of Requests')
    plt.ylabel('Throughput (requests per second)')
    plt.title('Throughput vs. Number of Requests')
    plt.grid(True)

    plt.tight_layout()
    plt.savefig('performance_metrics.png')
    # plt.show()  # Commented out because FigureCanvasAgg is non-interactive

# Main function to read results and plot graphs
def main():
    filename = 'load_test_results.txt'  # Replace with your result file
    runs, response_times, throughputs = read_results(filename)
    plot_graphs(runs, response_times, throughputs)

if __name__ == "__main__":
    main()

