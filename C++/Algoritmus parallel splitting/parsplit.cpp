#include <iostream>
#include <mpi.h>
#include <fstream>
#include <vector>


std::vector<int> convert_to_numbers(std::vector<unsigned char> &bytes) {
    std::vector<int> numbers(bytes.size());
    for (int i = 0; i < bytes.size(); i++) {
        numbers[i] = static_cast<int>(bytes[i]);
    }
    return numbers;
}

void load_numbers(std::vector<int> &numbers, int* numbers_size) {
    std::vector<unsigned char> bytes;
    std::ifstream input_file("numbers");
    if (input_file.is_open()) {
        unsigned char byte;
        while (input_file >> std::noskipws >> byte) {
            bytes.push_back(byte);
        }
        *numbers_size = bytes.size();
        numbers = convert_to_numbers(bytes);
    }
    else {
        std::cerr << "Error: Missing file 'numbers'." << std::endl;
        exit(1);
    }
}

void count_median(std::vector<int> &numbers, int* median) {
    *median = numbers[(numbers.size() - 1) / 2];
}

void count_block_size(int numbers_size, int* block_size, int num_of_processes) {
    *block_size = numbers_size / num_of_processes;
    if (numbers_size % num_of_processes != 0) {
        std::cerr << "Error: Input size not divisible by selected number of processes." << std::endl;
        exit(2);
    }
}

void distribute_between_processes(std::vector<int> &numbers, std::vector<int> &block, int block_size) {
    MPI_Scatter(
        numbers.data(),
        block_size,
        MPI_INT,
        block.data(),
        block_size,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
}

void sort_block_into_groups(
        std::vector<int> &sub_less, 
        std::vector<int> &sub_equal, 
        std::vector<int> &sub_greater, 
        std::vector<int> &block, 
        int block_size,
        int median
    ) {
    for(int i = 0; i < block_size; i++) {
        if (block[i] < median) {
            sub_less.push_back(block[i]);
        }
        else if (block[i] == median) {
            sub_equal.push_back(block[i]);
        }
        else {
            sub_greater.push_back(block[i]);
        }
    }
}

void agreggate_groups(std::vector<int> &sub_group, std::vector<int> &group, int rank, int num_of_processes) {
    int sub_group_size = sub_group.size();
    int offset = 0;
    MPI_Exscan(&sub_group_size, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    std::vector<int> lengths(num_of_processes);
    std::vector<int> offsets(num_of_processes, 0);
    MPI_Gather(&offset, 1, MPI_INT, offsets.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&sub_group_size, 1, MPI_INT, lengths.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        int group_size = lengths.back() + offsets.back();
        group.resize(group_size);
    }

    MPI_Gatherv(
        sub_group.data(),
        sub_group.size(),
        MPI_INT,
        group.data(),
        lengths.data(),
        offsets.data(),
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
}

void print_group(std::vector<int> &group, std::string name) {
    std::cout << name << ": [";
    if (group.size() > 0) std::cout << group[0];
    for (int i=1; i<group.size(); i++) {
        std::cout << ", " << group[i];
    }
    std::cout << "]" << std::endl;
}

void print_output(std::vector<int> &less, std::vector<int> &equal, std::vector<int> &greater) {
    print_group(less, "less");
    print_group(equal, "equal");
    print_group(greater, "greater");
}

int main(int argc, char* argv[]) {
    int num_of_processes;
    int rank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int numbers_size;
    std::vector<int> numbers;
    int median;
    int block_size;
    
    if (rank == 0) {
        load_numbers(numbers, &numbers_size);
        count_median(numbers, &median);
        count_block_size(numbers.size(), &block_size, num_of_processes);
    }
    
    MPI_Bcast(&numbers_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) numbers.resize(numbers_size);
    MPI_Bcast(numbers.data(), numbers_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Bcast(&median, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    std::vector<int> block(block_size);
    distribute_between_processes(numbers, block, block_size);

    std::vector<int> sub_less(0);
    std::vector<int> sub_equal(0);
    std::vector<int> sub_greater(0);
    sort_block_into_groups(
        sub_less, 
        sub_equal, 
        sub_greater, 
        block, 
        block_size,
        median
    );
    
    std::vector<int> less;
    std::vector<int> equal;
    std::vector<int> greater;
    agreggate_groups(sub_less, less, rank, num_of_processes);
    agreggate_groups(sub_equal, equal, rank, num_of_processes);
    agreggate_groups(sub_greater, greater, rank, num_of_processes);

    if (rank == 0) print_output(less, equal, greater);

    MPI_Finalize();
    return 0;
}
