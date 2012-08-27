#include <SFCGAL/Point.h>
#include <SFCGAL/GeometryVisitor.h>

#include <SFCGAL/Exception.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Exact_predicates_exact_constructions_kernel ExactKernel;

namespace SFCGAL {

///
///
///
Point::Point():
	Geometry(),
	_coordinate()
{
}

///
///
///
Point::Point( const Coordinate & coordinate ):
	_coordinate(coordinate)
{

}

///
///
///
Point::Point( const double & x, const double & y, const double & z ):
	Geometry(),
	_coordinate(x,y,z)
{

}


///
///
///
Point::Point( const Point & other ):
	Geometry(),
	_coordinate(other._coordinate)
{

}

///
///
///
Point& Point::operator = ( const Point & other )
{
	_coordinate = other._coordinate ;
	return *this ;
}

///
///
///
Point::~Point()
{

}


///
///
///
Point * Point::clone() const
{
	return new Point(*this);
}

///
///
///
std::string Point::geometryType() const
{
	return "Point";
}

///
///
///
GeometryType Point::geometryTypeId() const
{
	return TYPE_POINT ;
}

///
///
///
int Point::dimension() const
{
	return 0 ;
}

///
///
///
int Point::coordinateDimension() const
{
	return _coordinate.coordinateDimension() ;
}


///
///
///
bool Point::isEmpty() const
{
	return _coordinate.isEmpty() ;
}

///
///
///
bool Point::is3D() const
{
	return _coordinate.is3D() ;
}

///
///
///
void Point::accept( GeometryVisitor & visitor )
{
	return visitor.visit(*this);
}

///
///
///
void Point::accept( ConstGeometryVisitor & visitor ) const
{
	return visitor.visit(*this);
}

///
///
///
bool Point::operator < ( const Point & other ) const
{
	return _coordinate < other._coordinate ;
}

///
///
///
bool Point::operator == ( const Point & other ) const
{
	return _coordinate == other._coordinate ;
}

///
///
///
bool Point::operator != ( const Point & other ) const
{
	return _coordinate != other._coordinate ;
}


///
/// Private structures used to implement partial function specialization
template <typename K, int D>
struct do_toPoint_d
{
    static CGAL::Point_2<K> toPoint( const Point* p ) { return p->toPoint_2<K>(); }
};

template <typename K>
struct do_toPoint_d<K,3>
{
    static CGAL::Point_3<K> toPoint( const Point* p ) { return p->toPoint_3<K>(); }
};

template <typename K, int Dim>
typename TypeForKernel<K,Dim>::Point Point::toPoint_d() const
{
    return do_toPoint_d<K,Dim>::toPoint( this );
}
// template instanciations
template CGAL::Point_2<Kernel> Point::toPoint_d<Kernel,2>() const;
template CGAL::Point_3<Kernel> Point::toPoint_d<Kernel,3>() const;
template CGAL::Point_2<ExactKernel> Point::toPoint_d<ExactKernel,2>() const;
template CGAL::Point_3<ExactKernel> Point::toPoint_d<ExactKernel,3>() const;

}//SFCGAL

