#include <iostream>

#include <SFCGAL/Geometry.h>
#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/triangulate2DZ.h>

using namespace SFCGAL ;

int main( int argc, char* argv[] ){
    Geometry<Epeck> geometry = io::readWkt("MULTIPOINT(3.0 4.0,6.0 3.0,5.0 9.5,5.5 9.7)");
    TriangulatedSurface<Epeck> tin = algorithm::triangulate2DZ<Epeck>(geometry);
    for ( const Triangle<Epeck> & triangle : tin ){
        std::cout << triangle << std::endl ;
    }
    return 0 ;
}

