#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <time.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

bool DEBUG = true;

int MAX_COLS = 10;

// arr_insert()
// split up a coordinate line and insert into an matrix hashmap
void arr_insert(std::string coord_str, std::unordered_map<int, std::unordered_map<int,double>>& arr, Eigen::SparseMatrix<double>& arr_e, int& nrows, int& ncols, int& nonzeros)
{
    if (DEBUG) std::cout << "INSERTING: " << coord_str << std::endl;
    // first split the string by space
    int space_idx = coord_str.find(' ');
    std::string row = coord_str.substr(0,space_idx);
    std::string col = coord_str.substr(space_idx+1, coord_str.size()-space_idx);
    
    // if the col string has another space, it is not a coordinate
    // since other comments are parsed, this is the size specification
    // populate size variables
    int second_space = col.find(' ');
    if (second_space != std::string::npos) {
        std::cout << "FIRST LINE: " << coord_str << std::endl;
        nrows = std::stoi(row);
        ncols = std::stoi(col.substr(0,second_space));
        nonzeros = std::stoi(col.substr(second_space+1,col.size()-second_space));

        
        return;
    }
    // TODO: Seed rand()
    //double v1 = ((double) rand() / RAND_MAX)*4.8 + 0.1;
    // insert into the matrix data structure
    int rownum = std::stoi(row)-1;
    int colnum = std::stoi(col)-1;
    

    arr[rownum][colnum] = ((double) rand() / RAND_MAX)*4.8 + 0.1;
    //std::cout << rownum << " X " << colnum << std::endl;
    arr_e.coeffRef(rownum,colnum) = arr[rownum][colnum];
    // since symmetric
    arr[colnum][rownum] = ((double) rand() / RAND_MAX)*4.8 + 0.1;
    arr_e.coeffRef(colnum,rownum) = arr[colnum][rownum];

    //std::cout << coord_str << ":" << row << "," << col << std::endl;
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

void populate_vector(std::vector<double>& vec, int& ncols, Eigen::VectorXd& boost_vec) {
    // populate a random vector
    std::cout << "Populating vector" << std::endl;
    for (int i = 0; i < ncols; i++) {
         // if (i > MAX_COLS) break;
         vec.push_back(((double) rand() / RAND_MAX)*4.8 + 0.1);
         boost_vec(i) = vec[i];
         if (DEBUG){
             std::cout << vec[i] << std::endl;
             std::cout << boost_vec(i) << std::endl;
         }
    }
}



void multiply(std::unordered_map<int, std::unordered_map<int,double>>& arr, std::vector<double>& vec, std::vector<double>& result) {

    int debug_interval = 1000000;
    // for each row in the matrix, multiply each column i with the ith entry in vec, place into the ith entry of result
    for (auto i = arr.begin(); i != arr.end(); i++) {
        if (i->first % debug_interval == 0) std::cout << i->first << ": " << std::endl;
        for (auto j = arr[i->first].begin(); j != arr[i->first].end(); j++) {
            int row = i->first;
            int col = j->first;
            double val = arr[row][col];
            double vprod = val*vec[col];
            result[row] = result[row] + vprod;
            if (row % debug_interval == 0) {
                //std::cout << val << "xVec[" << col << "]=" << vprod <<std::endl;
            }
       }
        //std::cout << '\n';
    }
        //std::cout << rownum << " " << i->first<< ": " << i->second << std::endl;
}



std::unordered_map<int, std::unordered_map<int,double>> arr;
std::vector<double> vec;


int main() {
// TODO: Take into account 1 offset in coordinates
    int nrows, ncols, nonzeros;

    //2D array of matrix values; access w/ arr[row][col]

    //compressed_matrix<double> boost_arr(4163763,4163763,12487976);
    //mapped_vector<double> boost_vec(4163763,4163763);
    int t = 100000;
    //compressed_matrix<double> boost_arr(t,t,t*t);
    //vector<double> boost_vec(t,t);
    
    Eigen::SparseMatrix<double> arr_e(5,5);
    Eigen::VectorXd vec_e(5,1);

    // TODO: test arr_e.makeCompressed();
    /*
    int sqrtsize = 4163763+1;
    int msize = sqrtsize*sqrtsize;
    // TODO change msize
    compressed_matrix<int> boost_arr(sqrtsize, sqrtsize, msize);
    // TODO remember to convert all ints to doubles (doubles?)
    mapped_vector<int> v(sqrtsize,sqrtsize);
    for (int i = 0; i < sqrtsize; i++) {
        v(i) = (double) rand() / RAND_MAX;
    }*/

    //std::ifstream mtx("NLR.mtx");
    std::ifstream mtx("mini.mtx");

    if (mtx.is_open()) {
        int a = 0;
        std::string line;
        while (std::getline(mtx, line)) {
     //       if (a > MAX_COLS) break;

            if (line[0] == '%') {
                std::cout << "SKIP" << std::endl;
                continue;
            }

            if (line == "\n") {
                break;
            }

            arr_insert(line, arr, arr_e, nrows, ncols, nonzeros);
            a++;
            if (a % 1000000 == 0 || a < 100) {

                std::cout << a << std::endl;
            }
        }
        mtx.close();
    }

    populate_vector(vec, ncols, vec_e);
    if (DEBUG) print_matrix(arr, nrows, ncols);

    std::cout << "FL PARSED: " << nrows << "-" << ncols << "-" << nonzeros << std::endl;
    std::cout << "DONE LOADING" << std::endl;
    //print_row(4161248, arr);
    //mapped_vector<double> temp = prod(boost_arr, v); // TODO refactor
   
    // Multiplication begins here
    
    std::cout << arr_e << std::endl;
    std::vector<double> result(ncols, 0.0); //(length, initvalue) constructor
    multiply(arr, vec, result);
    std::cout << "VECVEC " << vec_e(1) << std::endl;
 //   std::cout << "ARR " << arr_e.coeff(1,2) << std::endl;
    std::cout << "New" << std::endl;
    Eigen::VectorXd boost_product = arr_e*vec_e;//prod(arr_e, vec_e);
    std::cout << "\nMultiplication complete.\n";

    if (DEBUG) {
        for(int i = 0; i < result.size(); i++) {
            std::cout << result[i] << " " << std::endl;
            std::cout << boost_product(i) << " " << std::endl;
        }
    }


    return 0;
}
