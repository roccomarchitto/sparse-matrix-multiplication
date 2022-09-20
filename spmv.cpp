// Standard libraries
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <time.h>
#include <chrono>

// Eigen3 optimized linear algebra library
#include <Eigen/Dense>
#include <Eigen/Sparse>

// OpenMP parallelization
#include <omp.h>


typedef Eigen::Triplet<double> T;

bool DEBUG = false; // turn on for verbose output (only use for small matrices!)

// parse a coordinate line and insert random values into the matrix hashmap
void arr_insert(std::string coord_str, std::unordered_map<int, std::unordered_map<int,double>>& arr, int& nrows, int& ncols, int& nonzeros, std::vector<Eigen::Triplet<double>>& tripletList);


// debugging tools
void print_row(int rownum, std::unordered_map<int, std::unordered_map<int,double>>& arr);


void print_matrix(std::unordered_map<int, std::unordered_map<int,double>>& arr, int& nrows, int& ncols);


// populate the custom and test vectors w/ random values
void populate_vector(std::vector<double>& vec, int& ncols, Eigen::VectorXd& test_vec);


// perform parallel SPMV w/ OpenMP
void multiply(std::unordered_map<int, std::unordered_map<int,double>>& arr, std::vector<double>& vec, std::vector<double>& result);


// load, multiply, and test SPMV for a specific file
// returns custom speed
int run_once(std::string mtxfile);



int main() {

    // Run SPMV for the three specified files
    // Assumptions: mtx header is matrix coordinate pattern symmetric
    std::vector<int> execution_times;
    execution_times.push_back(run_once("delaunay_n19.mtx"));
    execution_times.push_back(run_once("NLR.mtx"));
    execution_times.push_back(run_once("channel-500x100x100-b050.mtx"));

    std::cout << "Elapsed times (ms) for optimized SPMV runs (in the above order...):\n";
    for (int i = 0; i < execution_times.size(); i++) {
        std::cout << execution_times.at(i) << std::endl;
    }

    return 0;
}


void arr_insert(std::string coord_str, std::unordered_map<int, std::unordered_map<int,double>>& arr, int& nrows, int& ncols, int& nonzeros, std::vector<Eigen::Triplet<double>>& tripletList)
{
    if (DEBUG) std::cout << "INSERTING: " << coord_str << std::endl;
    
    // first parse the coord string by space
    int space_idx = coord_str.find(' ');
    std::string row = coord_str.substr(0,space_idx);
    std::string col = coord_str.substr(space_idx+1, coord_str.size()-space_idx);
    
    // if the col string has another space, it is not a coordinate
    // since other comments are parsed, this is the size specification
    // populate size variables if so
    int second_space = col.find(' ');
    if (second_space != std::string::npos) {
        nrows = std::stoi(row);
        ncols = std::stoi(col.substr(0,second_space));
        nonzeros = std::stoi(col.substr(second_space+1,col.size()-second_space));
        return;
    }
    // TODO: Seed rand()
    
    // insert into the matrix data structure, keeping note that indices are 1 off in the matrix market files
    int rownum = std::stoi(row)-1;
    int colnum = std::stoi(col)-1;
    

    // update the array
    arr[rownum][colnum] = ((double) rand() / RAND_MAX)*4.8 + 0.1;
    // since symmetric
    arr[colnum][rownum] = ((double) rand() / RAND_MAX)*4.8 + 0.1;

    // push back to tripletList for Eigen3 testing
    tripletList.push_back(T(rownum,colnum,arr[rownum][colnum]));
    if (colnum != rownum) tripletList.push_back(T(colnum,rownum,arr[colnum][rownum]));

}



void print_row(int rownum, std::unordered_map<int, std::unordered_map<int,double>>& arr) {
    for (auto i = arr[rownum].begin(); i != arr[rownum].end(); i++) {
        std::cout << rownum << " " << i->first<< ": " << i->second << std::endl;
    }
}



void print_matrix(std::unordered_map<int, std::unordered_map<int,double>>& arr, int& nrows, int& ncols) {
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < nrows; j++) {
            if (arr.find(i) != arr.end()) {
                if (arr[i].find(j) != arr[i].end()) {
                    std::cout << arr[i][j] << "\t";
                } else {
                    std::cout << "0\t";
                }
            } else {
                std::cout << "0\t";
            }
        }
        std::cout << '\n';
    }

}



void populate_vector(std::vector<double>& vec, int& ncols, Eigen::VectorXd& test_vec) {
    // populate a random vector along with the test vector
    for (int i = 0; i < ncols; i++) {
         vec.push_back(((double) rand() / RAND_MAX)*4.8 + 0.1);
         test_vec(i) = vec[i];
         if (DEBUG){
             std::cout << vec[i] << std::endl;
         }
    }
}



void multiply(std::unordered_map<int, std::unordered_map<int,double>>& arr, std::vector<double>& vec, std::vector<double>& result) {

// for each row in the matrix, multiply each column i with the ith entry in vec, place into the ith entry of result
    
#pragma omp parallel
{
    for (auto i = arr.begin(); i != arr.end(); i++) {
    #pragma omp single nowait
        {
            int row = i->first;
            for (auto j = arr[i->first].begin(); j != arr[i->first].end(); j++) {
                    int col = j->first;
                    double val = arr[row][col];
                    double vprod = val*vec[col];
                    result[row] = result[row] + vprod;
            }
        }
    }
}

}



int run_once(std::string mtxfile) {

    std::cout << "Executing SPMV for file: " << mtxfile << std::endl;

    int nrows, ncols, nonzeros;
    
    std::vector<Eigen::Triplet<double>> tripletList;
    
    std::unordered_map<int, std::unordered_map<int,double>> arr;
    std::vector<double> vec;
    
    std::ifstream mtx(mtxfile);


    if (mtx.is_open()) {
        int a = 0; // simple line counter
        std::string line;
        while (std::getline(mtx, line)) {
            if (line[0] == '%') {
                continue;
            }
            if (line == "\n") {
                break;
            }
            arr_insert(line, arr, nrows, ncols, nonzeros, tripletList);
            a++;
        }
        mtx.close();
    }

    Eigen::SparseMatrix<double> arr_e(ncols,ncols);
    Eigen::VectorXd vec_e(ncols,1);
    tripletList.reserve(nonzeros);
    arr_e.setFromTriplets(tripletList.begin(), tripletList.end());
    
    populate_vector(vec, ncols, vec_e);
    
    if (DEBUG) print_matrix(arr, nrows, ncols);

       
    // Multiplication begins here
    
    std::vector<double> result(ncols, 0.0); //(length, initvalue) constructor for the custom result vector
    
    // Multiply w/ timings
    auto start_time = std::chrono::high_resolution_clock::now();
    multiply(arr, vec, result);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ttime = end_time - start_time;

    //std::cout << "Elapsed time w/ custom OpenMP solution (ms): " << ttime/std::chrono::milliseconds(1) << std::endl;
    
    // Perform the Eigen3 test multiplication and check each value individually
    Eigen::VectorXd test_product = arr_e*vec_e;
    for(int i = 0; i < result.size(); i++) {
        if (abs(result[i]-test_product[i]) > 0.000001) {
            std::cout << "ERROR: Mismatch between Eigen SPMV and OpenMP custom SPMV: " << result[i] << " (Eigen) vs. " << test_product[i] << std::endl;
            exit(1);
        }
    }
    std::cout << "Tests passed for file " << mtxfile << std::endl << std::endl;

    if (DEBUG) { // print the vector on debug
        std::cout << "\nResult:\n";
        for (int i = 0; i < result.size(); i++) {
            std::cout << result[i] << std::endl;
        }
    }

    // Return elapsed time
    return ttime/std::chrono::milliseconds(1);

}
