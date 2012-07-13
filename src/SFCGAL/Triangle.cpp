#include <SFCGAL/Triangle.h>
#include <SFCGAL/GeometryVisitor.h>

namespace SFCGAL {

///
///
///
Triangle::Triangle() :
	Surface()
{

}

///
///
///
Triangle::Triangle( const Point & p, const Point & q, const Point & r ) :
	Surface()
{
	_vertices[0] = p ;
	_vertices[1] = q ;
	_vertices[2] = r ;
}


///
///
///
Triangle::Triangle( const Triangle & other )
	: Surface()
{
	_vertices[0] = other._vertices[0] ;
	_vertices[1] = other._vertices[1] ;
	_vertices[2] = other._vertices[2] ;
}




///
///
///
Triangle& Triangle::operator = ( const Triangle & other )
{
	_vertices[0] = other._vertices[0] ;
	_vertices[1] = other._vertices[1] ;
	_vertices[2] = other._vertices[2] ;
	return *this ;
}

///
///
///
Triangle::~Triangle()
{

}



///
///
///
Triangle *   Triangle::clone() const
{
	return new Triangle( *this );
}


///
///
///
std::string    Triangle::geometryType() const
{
	return "Triangle" ;
}


///
///
///
GeometryType   Triangle::geometryTypeId() const
{
	return TYPE_TRIANGLE ;
}


///
///
///
int  Triangle::coordinateDimension() const
{
	return _vertices[0].coordinateDimension() ;
}


///
///
///
bool  Triangle::isEmpty() const
{
	return _vertices[0].isEmpty();
}


///
///
///
bool  Triangle::is3D() const
{
	return _vertices[0].is3D() ;
}


///
///
///
void Triangle::accept( GeometryVisitor & visitor )
{
	return visitor.visit(*this);
}

///
///
///
void Triangle::accept( ConstGeometryVisitor & visitor ) const
{
	return visitor.visit(*this);
}




}//SFCGAL


