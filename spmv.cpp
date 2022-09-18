#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;
// arr_insert()
// split up a coordinate line and insert into an matrix hashmap
void arr_insert(std::string coord_str, std::unordered_map<int, std::unordered_map<int, int>>& arr, mapped_matrix<int>& boost_arr)
{
    // first split the string by space
    int space_idx = coord_str.find(' ');
    std::string row = coord_str.substr(0,space_idx);
    std::string col = coord_str.substr(space_idx+1, coord_str.size()-space_idx);
    
    // if the col string has another space, it is not a coordinate
    if (col.find(' ') != std::string::npos) {
        std::cout << "FIRST LINE: " << coord_str << std::endl;
        return;
    }
    // TODO: Seed rand()
    //double v1 = ((double) rand() / RAND_MAX)*4.8 + 0.1;
    // insert into the matrix data structure
    int rownum = std::stoi(row);
    int colnum = std::stoi(col); 
    //arr[rownum][colnum] = 1;
    //std::cout << rownum << " X " << colnum << std::endl;
    boost_arr(rownum,colnum) = 1;
    // since symmetric
    //arr[colnum][rownum] = 1;
    boost_arr(colnum,rownum) = 1;

    //std::cout << coord_str << ":" << row << "," << col << std::endl;
}


void print_row(int rownum, std::unordered_map<int, std::unordered_map<int, int>>& arr) {
    for (auto i = arr[rownum].begin(); i != arr[rownum].end(); i++) {
        std::cout << rownum << " " << i->first<< ": " << i->second << std::endl;
    }
}


int main() {

    //2D array of matrix values; access w/ arr[row][col]
    std::unordered_map<int, std::unordered_map<int, int>> arr;
    int sqrtsize = 4163763+1;
    int msize = sqrtsize*sqrtsize;
    // TODO change msize
    mapped_matrix<int> boost_arr(sqrtsize, sqrtsize, msize);
    // TODO remember to convert all ints to doubles (floats?)
    mapped_vector<int> v(sqrtsize,sqrtsize);
    for (int i = 0; i < sqrtsize; i++) {
        v(i) = (float) rand() / RAND_MAX;
    }

    std::ifstream mtx("NLR.mtx");

    if (mtx.is_open()) {
        int a = 0;
        std::string line;
        while (std::getline(mtx, line)) {
        
            if (line[0] == '%') {
                std::cout << "SKIP" << std::endl;
                continue;
            }

            if (line == "\n") {
                break;
            }

            arr_insert(line, arr, boost_arr);
            a++;
            if (a % 1000000 == 0 || a < 100 || a > 12200000) {

                std::cout << a << std::endl;
            }
        }
        mtx.close();
    }
    std::cout << "DONE LOADING" << std::endl;
    print_row(4161248, arr);
    mapped_vector<double> temp = prod(boost_arr, v); // TODO refactor
    std::cout << "DONE: v(0) = " << v(0) << std::endl;

    std::cout << "DONE" << std::endl;
    return 0;
}
