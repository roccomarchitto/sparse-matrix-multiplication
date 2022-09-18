#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/numeric/ublas/matrix.hpp>

// arr_insert()
// split up a coordinate line and insert into an matrix hashmap
void arr_insert(std::string coord_str, std::unordered_map<int, std::unordered_map<int, int>>& arr)
{
    // first split the string by space
    int space_idx = coord_str.find(' ');
    std::string row = coord_str.substr(0,space_idx);
    std::string col = coord_str.substr(space_idx+1, coord_str.size()-space_idx);
    
    // if the col string has another space, it is not a coordinate
    if (col.find(' ') != std::string::npos) {

        std::cout << "FIRST LINE: " << coord_str << std::endl;
    }

    // insert into the matrix data structure
    int rownum = std::stoi(row);
    int colnum = std::stoi(col); 
    arr[rownum][colnum] = 1;
    // since symmetric
    arr[colnum][rownum] = 1;

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

    std::ifstream mtx("NLR.mtx");

    if (mtx.is_open()) {
        int a = 0;
        std::string line;
        while (std::getline(mtx, line)) {

            if (line[0] == '%') {
                std::cout << "SKIP" << std::endl;
                continue;
            }

            arr_insert(line, arr);
            a++;
            if (a % 1000000 == 0 || a < 100) {
                std::cout << a << std::endl;
            }
        }
        mtx.close();
    }
    print_row(4161248, arr);
    


    std::cout << "DONE" << std::endl;
    return 0;
}
