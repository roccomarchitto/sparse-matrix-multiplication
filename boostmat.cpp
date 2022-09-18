#include <iostream>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace boost::numeric::ublas;

int main () {
    int rows = 4;
    int cols = 4;
    mapped_matrix<double> m(rows,cols,3*3);
    mapped_vector<double> v(4,4);
    
    v(1) = 3;
    v(0) = 6;
    m(1,1) = 5;
    m(1,0) = 1;
    m(2,1) = 1;
    
    mapped_vector<double> temp = prod(m,v);

    for(int row = 0; row < m.size1(); row++) {
     //   for(int col = 0; col < m.size2(); col++) {
     //      std::cout << m(row,col) << "    ";
            std::cout << temp(row) << std::endl;
     //   }
     //   std::cout << std::endl;
    }
    return 0;
}
