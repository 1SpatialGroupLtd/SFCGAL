#include <SFCGAL/Polygon.h>
#include <SFCGAL/GeometryVisitor.h>

#include <SFCGAL/Triangle.h>

namespace SFCGAL {

///
///
///
Polygon::Polygon():
	Surface(),
	_rings(1, LineStringPtr( new LineString() ) )
{

}

///
///
///
Polygon::Polygon( const std::vector< LineString > & rings ):
	Surface()
{
	if ( rings.empty() ){
		_rings.resize(1, LineStringPtr( new LineString() ) );
	}else{
		_rings.resize(rings.size());
		for ( size_t i = 0; i < rings.size(); i++ ){
			_rings[i] = LineStringPtr( rings[i].clone() ) ;
		}
	}
}

///
///
///
Polygon::Polygon( const LineString & exteriorRing ):
	Surface(),
	_rings(1,LineStringPtr(exteriorRing.clone()))
{

}

///
///
///
Polygon::Polygon( const Triangle & triangle ):
	Surface(),
	_rings(1,LineStringPtr(new LineString()))
{
	if ( ! triangle.isEmpty() ){
		for ( size_t i = 0; i < 4; i++ ){
			exteriorRing().addPoint( triangle.vertex(i) );
		}
	}
}

///
///
///
Polygon::Polygon( Polygon const& other ):
	Surface(other)
{
	(*this) = other ;
}

///
///
///
Polygon& Polygon::operator = ( const Polygon & other )
{
	_rings.resize( other.numRings() );
	for ( size_t i = 0; i < other.numRings(); i++ ){
		_rings[i] = LineStringPtr( other.ringN(i).clone() );
	}
	return *this ;
}

///
///
///
Polygon::~Polygon()
{

}

///
///
///
int Polygon::coordinateDimension() const
{
	return _rings[0]->coordinateDimension() ;
}


///
///
///
std::string Polygon::geometryType() const
{
	return "Polygon" ;
}

///
///
///
GeometryType Polygon::geometryTypeId() const
{
	return TYPE_POLYGON ;
}

///
///
///
Polygon * Polygon::clone() const
{
	return new Polygon(*this);
}

///
///
///
bool   Polygon::isEmpty() const
{
	return exteriorRing().isEmpty() ;
}

///
///
///
bool   Polygon::is3D() const
{
	return exteriorRing().is3D() ;
}

///
///
///
void Polygon::reverse()
{
	for ( size_t i = 0; i < numRings(); i++ ){
		ringN(i).reverse();
	}
}


///
///
///
void Polygon::accept( GeometryVisitor & visitor )
{
	return visitor.visit(*this);
}

///
///
///
void Polygon::accept( ConstGeometryVisitor & visitor ) const
{
	return visitor.visit(*this);
}

}//SFCGAL

