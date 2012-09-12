#include <SFCGAL/PolyhedralSurface.h>
#include <SFCGAL/GeometryVisitor.h>

namespace SFCGAL {

///
///
///
PolyhedralSurface::PolyhedralSurface():
	Surface(),
	_polygons()
{

}

///
///
///
PolyhedralSurface::PolyhedralSurface( const std::vector< Polygon > & polygons ) :
	Surface()
{
	for ( size_t i = 0; i < polygons.size(); i++ ){
		_polygons.push_back( polygons[i].clone() ) ;
	}
}

///
///
///
PolyhedralSurface::PolyhedralSurface( PolyhedralSurface const& other ) :
	Surface(),
	_polygons(other._polygons)
{

}

///
///
///
PolyhedralSurface& PolyhedralSurface::operator = ( const PolyhedralSurface & other )
{
	_polygons = other._polygons ;
	return *this ;
}


///
///
///
PolyhedralSurface::~PolyhedralSurface()
{

}

///
///
///
PolyhedralSurface * PolyhedralSurface::clone() const
{
	return new PolyhedralSurface(*this);
}

///
///
///
std::string  PolyhedralSurface::geometryType() const
{
	return "PolyhedralSurface" ;
}

///
///
///
GeometryType  PolyhedralSurface::geometryTypeId() const
{
	return TYPE_POLYHEDRALSURFACE ;
}

///
///
///
int  PolyhedralSurface::dimension() const
{
	return 2 ;
}

///
///
///
int  PolyhedralSurface::coordinateDimension() const
{
	if ( isEmpty() ){
		return 0 ;
	}else{
		return _polygons.front().coordinateDimension() ;
	}
}

///
///
///
bool  PolyhedralSurface::isEmpty() const
{
	return _polygons.empty();
}

///
///
///
bool  PolyhedralSurface::is3D() const
{
	if ( isEmpty() ){
		return false ;
	}else{
		return _polygons.front().is3D() ;
	}
}


///
///
///
TriangulatedSurface  PolyhedralSurface::toTriangulatedSurface() const
{
	TriangulatedSurface result ;
	algorithm::triangulate( *this, result );
	return result ;
}

///
///
///
void  PolyhedralSurface::addPolygon( const Polygon & polygon )
{
	addPolygon( polygon.clone() );
}

///
///
///
void  PolyhedralSurface::addPolygon( Polygon* polygon )
{
	BOOST_ASSERT( polygon != NULL );
	_polygons.push_back( polygon );
}

///
///
///
void  PolyhedralSurface::addPolygons( const PolyhedralSurface & polyhedralSurface )
{
	for ( size_t i = 0; i < polyhedralSurface.numPolygons(); i++ ){
		addPolygon( polyhedralSurface.polygonN(i) );
	}
}

///
///
///
void PolyhedralSurface::accept( GeometryVisitor & visitor )
{
	return visitor.visit(*this);
}

///
///
///
void PolyhedralSurface::accept( ConstGeometryVisitor & visitor ) const
{
	return visitor.visit(*this);
}
}//SFCGAL



