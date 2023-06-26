#include <iostream>
#include <mpi.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>

#define NUM_OF_CLUSTERS 4

std::vector<int> convert_to_numbers(std::vector<unsigned char> &bytes) {
    std::vector<int> numbers(bytes.size());
    for (int i = 0; i < bytes.size(); i++) {
        numbers[i] = static_cast<int>(bytes[i]);
    }
    return numbers;
}

void load_numbers(std::vector<int> &numbers) {
    std::vector<unsigned char> bytes;
    std::ifstream input_file("numbers");
    if (input_file.is_open()) {
        unsigned char byte;
        while (input_file >> std::noskipws >> byte) {
            bytes.push_back(byte);
        }
        numbers = convert_to_numbers(bytes);
    }
    else {
        std::cerr << "Error: Missing file 'numbers'." << std::endl;
        exit(1);
    }
}

void handle_numbers_length(std::vector<int> &numbers, int num_of_processes) {
    if (numbers.size() < num_of_processes) {
        std::cerr << "Error: Input is shorter than selected number of processes." << std::endl;
        exit(2);
    }
    else if (numbers.size() > num_of_processes) {
        numbers.erase(numbers.begin() + num_of_processes, numbers.end());
    }
}

void distribute_between_processes(std::vector<int> &numbers, int* assigned_number) {
    MPI_Scatter(
        numbers.data(), 1, MPI_INT, assigned_number, 1, MPI_INT, 0, MPI_COMM_WORLD
    );
}

int select_nearest_centroid(float* centroids, int number) {
    float min_distance = std::abs(centroids[0] - number);
    int centroid_index = 0;
    for (int i=0; i<NUM_OF_CLUSTERS; i++) {
        float distance = std::abs(centroids[i] - number);
        if (distance < min_distance) {
            min_distance = distance;
            centroid_index = i;
        }
    }
    return centroid_index; 
}

void get_new_centroids(float* centroids, int* sums_of_clusters, int* size_of_clusters) {
    for (int i=0; i < NUM_OF_CLUSTERS; i++) {
        if (size_of_clusters[i] != 0) {
            centroids[i] = static_cast<float>(sums_of_clusters[i]) / size_of_clusters[i];
        }
    }
}

void sort_into_clusters(
    std::vector<std::vector<int>> &clusters,
    std::vector<int> &numbers,
    float* centroids
) {
    for (int i=0;i<numbers.size();i++) {
        int index = select_nearest_centroid(centroids, numbers[i]);
        clusters[index].push_back(numbers[i]);
        }
}

void print_cluster(float centroid, std::vector<int> &cluster) {
    std::string numbers = "";
    for (int i=0; i<cluster.size(); i++) {
        numbers += " " + std::to_string(cluster[i]);
    }
    printf("[%.3f]%s\n", centroid, numbers.c_str());
}

int main(int argc, char* argv[]) {
    int num_of_processes;
    int rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::vector<int> numbers;
    int assigned_number;
    
    float centroids[NUM_OF_CLUSTERS];
    float* new_centroids;

    int clusters_with_one_number[NUM_OF_CLUSTERS];
    int* sums_of_clusters;
    int* size_of_clusters;

    int continue_loop = 1;
    int nearest_centroid;
    
    if (rank == 0) {
        new_centroids = new float[NUM_OF_CLUSTERS];
        sums_of_clusters = new int[NUM_OF_CLUSTERS];
        size_of_clusters = new int[NUM_OF_CLUSTERS];

        load_numbers(numbers);
        handle_numbers_length(numbers, num_of_processes);
        std::copy(numbers.begin(), numbers.begin() + NUM_OF_CLUSTERS, centroids);
        std::copy(centroids, centroids + NUM_OF_CLUSTERS, new_centroids);
    }
    distribute_between_processes(numbers, &assigned_number);

    while (continue_loop) {
        MPI_Bcast(centroids, NUM_OF_CLUSTERS, MPI_FLOAT, 0, MPI_COMM_WORLD);
        nearest_centroid = select_nearest_centroid(centroids, assigned_number);

        std::fill(clusters_with_one_number, clusters_with_one_number + NUM_OF_CLUSTERS, 0);
        clusters_with_one_number[nearest_centroid] = assigned_number;
        MPI_Reduce(clusters_with_one_number, sums_of_clusters, NUM_OF_CLUSTERS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        
        std::fill(clusters_with_one_number, clusters_with_one_number + NUM_OF_CLUSTERS, 0);
        clusters_with_one_number[nearest_centroid] = 1;
        MPI_Reduce(clusters_with_one_number, size_of_clusters, NUM_OF_CLUSTERS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
                
        if (rank == 0) {
            get_new_centroids(new_centroids, sums_of_clusters, size_of_clusters);
            if (std::equal(centroids, centroids + NUM_OF_CLUSTERS, new_centroids)) {
                continue_loop = 0;
            } 
            else {
                std::copy(new_centroids, new_centroids + NUM_OF_CLUSTERS, centroids);
            }
        }
        MPI_Bcast(&continue_loop, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
        
    if (rank == 0) {
        std::vector<std::vector<int>> clusters(NUM_OF_CLUSTERS);
        sort_into_clusters(clusters, numbers, centroids);

        for (int i=0;i<NUM_OF_CLUSTERS;i++) {
            print_cluster(centroids[i], clusters[i]);
        }

        delete new_centroids;
        delete sums_of_clusters;
        delete size_of_clusters;
    }
    
    MPI_Finalize();
    return 0;
}
