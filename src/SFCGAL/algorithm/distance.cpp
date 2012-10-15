#include <SFCGAL/algorithm/distance.h>

#include <SFCGAL/Kernel.h>
#include <SFCGAL/all.h>
#include <SFCGAL/Exception.h>

#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2_algorithms.h>

#include <SFCGAL/transform/AffineTransform3.h>
#include <SFCGAL/algorithm/intersects.h>


typedef SFCGAL::Kernel::Point_2                                   Point_2 ;
typedef SFCGAL::Kernel::Vector_2                                  Vector_2 ;
typedef SFCGAL::Kernel::Segment_2                                 Segment_2 ;

typedef CGAL::Polygon_2< SFCGAL::Kernel >                         Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel >              Polygon_with_holes_2 ;



namespace SFCGAL {
namespace algorithm {

///
///
///
double distance( const Geometry & gA, const Geometry& gB )
{
	switch ( gA.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointGeometry( gA.as< Point >(), gB ) ;
	case TYPE_LINESTRING:
		return distanceLineStringGeometry( gA.as< LineString >(), gB ) ;
	case TYPE_POLYGON:
		return distancePolygonGeometry( gA.as< Polygon >(), gB ) ;
	case TYPE_TRIANGLE:
		return distanceTriangleGeometry( gA.as< Triangle >(), gB ) ;
	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry( gA, gB );
	}
	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distancePointGeometry( const Point & gA, const Geometry& gB )
{
	switch ( gB.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointPoint( gA, gB.as< Point >() );
	case TYPE_LINESTRING:
		return distancePointLineString( gA, gB.as< LineString >() );
	case TYPE_POLYGON:
		return distancePointPolygon( gA, gB.as< Polygon >() );
	case TYPE_TRIANGLE:
		return distancePointTriangle( gA, gB.as< Triangle >() );

	//collection dispatch
	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry( gB, gA );
	}
	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distancePointPoint( const Point & gA, const Point& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	return CGAL::sqrt(
		CGAL::to_double(
			CGAL::squared_distance( gA.toPoint_2(), gB.toPoint_2() )
		)
	);
}

///
///
///
double distancePointLineString( const Point & gA, const LineString& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	BOOST_ASSERT( gB.numPoints() >= 2 );
	size_t numSegments = gB.numPoints() - 1 ;

	double dMin = SFCGAL::NaN() ;
	for ( size_t i = 0; i < numSegments; i++ ){
		double d = distancePointSegment( gA, gB.pointN(i), gB.pointN(i+1) );
		if ( i == 0 || d < dMin ){
			dMin = d ;
		}
	}
	return dMin ;
}

///
///
///
double distancePointPolygon( const Point & gA, const Polygon& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	if ( intersects(gA,gB) ){
		return 0.0 ;
	}

	double dMin = 0.0 ;
	//check if the point is in the polygon
	for ( size_t i = 0; i < gB.numRings(); i++ ){
		double d = distancePointLineString( gA, gB.ringN(i) );
		if ( i == 0 || d < dMin ){
			dMin = d ;
		}
	}
	return dMin ;
}

///
///
///
double distancePointTriangle( const Point & gA, const Triangle& gB )
{
	return distancePointPolygon( gA, gB.toPolygon() );
}



///
///
///
double distanceLineStringGeometry( const LineString & gA, const Geometry& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	switch ( gB.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointLineString( gB.as< Point >(), gA ); //symetric
	case TYPE_LINESTRING:
		return distanceLineStringLineString( gA, gB.as< LineString >() );
	case TYPE_POLYGON:
		return distanceLineStringPolygon( gA, gB.as< Polygon >() );
	case TYPE_TRIANGLE:
		return distanceLineStringTriangle( gA, gB.as< Triangle >() );

	//collection dispatch
	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry( gB, gA );
	}
	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distanceLineStringLineString( const LineString & gA, const LineString& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	BOOST_ASSERT( gA.numPoints() >= 2 );
	BOOST_ASSERT( gB.numPoints() >= 2 );
	size_t nsA = gA.numPoints() - 1 ;
	size_t nsB = gB.numPoints() - 1 ;

	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < nsA; i++ ){
		for ( size_t j = 0; j < nsB; j++ ){
			dMin = std::min(
				dMin,
				distanceSegmentSegment(
					gA.pointN(i), gA.pointN(i+1),
					gB.pointN(j), gB.pointN(j+1)
				)
			) ;
		}
	}
	return dMin ;
}


///
///
///
double distanceLineStringPolygon( const LineString & gA, const Polygon& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	if ( intersects(gA, gB) ){
		return 0.0 ;
	}
	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gB.numRings(); i++ ){
		double d = distanceLineStringLineString( gA, gB.ringN(i) );
		if ( d < dMin )
			dMin = d ;
	}
	return dMin ;
}


///
///
///
double distanceLineStringTriangle( const LineString & gA, const Triangle& gB )
{
	return distanceLineStringPolygon( gA, gB.toPolygon() );
}





///
///
///
double distancePolygonGeometry( const Polygon & gA, const Geometry& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	switch ( gB.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointPolygon( gB.as< Point >(), gA );           //symetric
	case TYPE_LINESTRING:
		return distanceLineStringPolygon( gB.as< LineString >(), gA ); //symetric
	case TYPE_POLYGON:
		return distancePolygonPolygon( gA, gB.as< Polygon >() );
	case TYPE_TRIANGLE:
		return distancePolygonTriangle( gA, gB.as< Triangle >() );

	//collection dispatch
	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry( gB, gA );
	}
	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distancePolygonPolygon( const Polygon & gA, const Polygon& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	if ( intersects(gA, gB) ){
		return 0.0 ;
	}
	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gA.numRings(); i++ ){
		for ( size_t j = 0; j < gB.numRings(); j++ ){
			double d = distanceLineStringLineString( gA.ringN(i), gB.ringN(j) );
			if ( d < dMin )
				dMin = d ;
		}
	}
	return dMin ;
}

///
///
///
double distancePolygonTriangle( const Polygon & gA, const Triangle& gB )
{
	return distancePolygonPolygon( gA, gB.toPolygon() );
}

///
///
///
double distanceTriangleGeometry( const Triangle & gA, const Geometry& gB )
{
	return distancePolygonGeometry( gA.toPolygon(), gB );
}


///
///
///
double distanceGeometryCollectionToGeometry( const Geometry & gA, const Geometry& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gA.numGeometries(); i++ ){
		dMin = std::min( dMin, distance( gA.geometryN(i), gB ) );
	}
	return dMin ;
}




//--- private



///
///
///
double distancePointSegment( const Point & p, const Point & a, const Point & b )
{
	BOOST_ASSERT( ! p.isEmpty() );
	BOOST_ASSERT( ! a.isEmpty() );
	BOOST_ASSERT( ! b.isEmpty() );

	return CGAL::sqrt(
		CGAL::to_double(
			CGAL::squared_distance(
				p.toPoint_2(),
				Segment_2(
					a.toPoint_2(),
					b.toPoint_2()
				)
			)
		)
	);
}

///
///
///
double distanceSegmentSegment( const Point & a, const Point & b, const Point & c, const Point & d )
{
	BOOST_ASSERT( ! a.isEmpty() );
	BOOST_ASSERT( ! b.isEmpty() );
	BOOST_ASSERT( ! c.isEmpty() );
	BOOST_ASSERT( ! d.isEmpty() );

	return CGAL::sqrt( CGAL::to_double(
		CGAL::squared_distance(
			CGAL::Segment_2< Kernel >( a.toPoint_2(), b.toPoint_2() ),
			CGAL::Segment_2< Kernel >( c.toPoint_2(), d.toPoint_2() )
		)
	) );
}


}//namespace algorithm
}//namespace SFCGAL
