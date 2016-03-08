#include <iostream>

#include <SFCGAL/visitor.h>

using namespace SFCGAL ;

/**
 * Visitor that retrieves the geometry type
 */
class GeometryTypeVisitor : public boost::static_visitor< std::string > {
public:
    template < typename K >
    std::string operator() ( const Point<K> & p ) const {
        return "Point" ;
    }
    template < typename K >
    std::string operator() ( const Triangle<K> & p ) const {
        return "Triangle" ;
    }

    template < typename OtherType >
    std::string operator() ( const OtherType & other ) const {
        return "UnknownType" ;
    }
} ;

int main( int argc, char* argv[] ){
    Point<Epeck> a(0.0,0.0,1.0);
    Point<Epeck> b(1.0,0.0,0.5);
    Point<Epeck> c(0.0,1.0,0.0);
    Triangle<Epeck> triangle(a,b,c) ;
    Polygon<Epeck> polygon ;

    GeometryCollection<Epeck> geometries ;
    geometries.push_back(a.clone());
    geometries.push_back(b.clone());
    geometries.push_back(triangle.clone());
    geometries.push_back(polygon.clone());

    for ( const Geometry<Epeck> & geometry : geometries ){
        GeometryTypeVisitor visitor;
        std::cout << SFCGAL::apply_visitor(visitor,geometry) << std::endl;
    }

    return 0 ;
}
