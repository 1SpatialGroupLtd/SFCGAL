#include <SFCGAL/algorithm/distance3d.h>

#include <SFCGAL/all.h>
#include <SFCGAL/Exception.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <SFCGAL/transform/AffineTransform3.h>
#include <SFCGAL/algorithm/intersects.h>
#include <SFCGAL/algorithm/triangulate.h>


typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel ;
typedef Kernel::FT                                        squared_distance_t ;

typedef Kernel::Point_3                                   Point_3 ;
typedef Kernel::Vector_3                                  Vector_3 ;
typedef Kernel::Segment_3                                 Segment_3 ;
typedef Kernel::Triangle_3                                Triangle_3 ;
typedef Kernel::Plane_3                                   Plane_3 ;

namespace SFCGAL {
namespace algorithm {

///
///
///
double distance3D( const Geometry & gA, const Geometry& gB )
{
	switch ( gA.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointGeometry3D( gA.as< Point >(), gB );
	case TYPE_LINESTRING:
		return distanceLineStringGeometry3D( gA.as< LineString >(), gB );
//	case TYPE_POLYGON:
//	case TYPE_TRIANGLE:
//	case TYPE_SOLID:

	//collection dispatch
	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry3D( gB, gA );
	}

	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance3D(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distancePointGeometry3D( const Point & gA, const Geometry& gB )
{
	switch ( gB.geometryTypeId() ){
	case TYPE_POINT:
		return distancePointPoint3D( gA, gB.as< Point >() );
	case TYPE_LINESTRING:
		return distancePointLineString3D( gA, gB.as< LineString >() );
	case TYPE_TRIANGLE:
		return distancePointTriangle3D( gA, gB.as< Triangle >() );
	case TYPE_POLYGON:
		return distancePointPolygon3D( gA, gB.as< Polygon >() );
	case TYPE_SOLID:
		return distancePointSolid3D( gA, gB.as< Solid >() );

	case TYPE_MULTIPOINT:
	case TYPE_MULTILINESTRING:
	case TYPE_MULTIPOLYGON:
	case TYPE_MULTISOLID:
	case TYPE_GEOMETRYCOLLECTION:
	case TYPE_TRIANGULATEDSURFACE:
	case TYPE_POLYHEDRALSURFACE:
		return distanceGeometryCollectionToGeometry3D( gB, gA );
	}

	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance3D(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distancePointPoint3D( const Point & gA, const Point& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	return CGAL::sqrt(
		CGAL::to_double(
			CGAL::squared_distance( gA.toPoint_3< Kernel >(), gB.toPoint_3< Kernel >() )
		)
	);
}

///
///
///
double distancePointLineString3D( const Point & gA, const LineString& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	BOOST_ASSERT( gB.numPoints() >= 2 );

	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gB.numPoints(); i++ ){
		dMin = std::min( dMin, distancePointSegment3D( gA, gB.pointN(i), gB.pointN(i+1) ) );
	}
	return dMin ;
}


///
///
///
double distancePointTriangle3D( const Point & gA, const Triangle& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}
	return distancePointTriangle3D( gA, gB.vertex(0), gB.vertex(1), gB.vertex(2) );
}


///
///
///
double distancePointPolygon3D( const Point & gA, const Polygon & gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}
	TriangulatedSurface triangulateSurfaceB ;
	triangulate( gB, triangulateSurfaceB ) ;
	return distanceGeometryCollectionToGeometry3D( triangulateSurfaceB, gA );
}



///
///
///
double distancePointSolid3D( const Point & gA, const Solid& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	if ( intersects3D(gA,gB) ){
		return 0.0 ;
	}

	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gB.numShells(); i++ ){
		dMin = std::min( dMin, distanceGeometryCollectionToGeometry3D( gB.shellN(i), gA ) );
	}
	return dMin ;
}





///
///
///
double distanceLineStringGeometry3D( const LineString & gA, const Geometry& gB )
{
	switch ( gB.geometryTypeId() ){
		case TYPE_POINT:
			return distancePointLineString3D( gB.as< Point >(), gA ); //symetric
		case TYPE_LINESTRING:
			return distanceLineStringLineString3D( gA, gB.as< LineString >() );
		case TYPE_TRIANGLE:
			return distanceLineStringTriangle3D( gA, gB.as< Triangle >() );
		case TYPE_POLYGON:
			return distanceLineStringPolygon3D( gA, gB.as< Polygon >() );

		case TYPE_MULTIPOINT:
		case TYPE_MULTILINESTRING:
		case TYPE_MULTIPOLYGON:
		case TYPE_MULTISOLID:
		case TYPE_GEOMETRYCOLLECTION:
			return distanceGeometryCollectionToGeometry3D( gB.as< GeometryCollection >(), gA );
	}

	BOOST_THROW_EXCEPTION(Exception(
		( boost::format("distance3D(%s,%s) is not implemented") % gA.geometryType() % gB.geometryType() ).str()
	));
}

///
///
///
double distanceLineStringLineString3D( const LineString & gA, const LineString& gB )
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
				distanceSegmentSegment3D(
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
double distanceLineStringTriangle3D( const LineString & gA, const Triangle& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	BOOST_ASSERT( gA.numPoints() >= 2 );

	double dMin = std::numeric_limits< double >::infinity() ;

	const Point & tA = gB.vertex(0) ;
	const Point & tB = gB.vertex(1) ;
	const Point & tC = gB.vertex(2) ;

	for ( size_t i = 0; i < gA.numSegments(); i++ ){
		dMin = std::min( dMin, distanceSegmentTriangle3D( gA.pointN(i), gA.pointN(i+1), tA, tB, tC ) );
	}
	return dMin ;
}

///
///
///
double distanceLineStringPolygon3D( const LineString & gA, const Polygon & gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}
	TriangulatedSurface triangulateSurfaceB ;
	triangulate( gB, triangulateSurfaceB ) ;
	return distanceGeometryCollectionToGeometry3D( triangulateSurfaceB, gA );
}


///
///
///
double distanceGeometryCollectionToGeometry3D( const Geometry & gA, const Geometry& gB )
{
	if ( gA.isEmpty() || gB.isEmpty() ){
		return std::numeric_limits< double >::infinity() ;
	}

	double dMin = std::numeric_limits< double >::infinity() ;
	for ( size_t i = 0; i < gA.numGeometries(); i++ ){
		dMin = std::min( dMin, distance3D( gA.geometryN(i), gB ) );
	}
	return dMin ;
}


///
///
///
double distancePointSegment3D( const Point & p, const Point & a, const Point & b )
{
	BOOST_ASSERT( ! p.isEmpty() );
	BOOST_ASSERT( ! a.isEmpty() );
	BOOST_ASSERT( ! b.isEmpty() );

	return CGAL::sqrt(
		CGAL::to_double(
			CGAL::squared_distance(
				p.toPoint_3< Kernel >(),
				Segment_3(
					a.toPoint_3< Kernel >(),
					b.toPoint_3< Kernel >()
				)
			)
		)
	);
}

/*
 * CGAL pre-implentation
 */
squared_distance_t distancePointTriangle3D(
	const Point_3 & p,
	const Point_3& a,
	const Point_3& b,
	const Point_3& c
)
{
	/*
	 * project P on ABC plane as projP.
	 */
	Point_3 projP = Plane_3( a, b, c ).projection( p );

	squared_distance_t dMin ;
	if ( Triangle_3( a, b, c ).has_on( projP ) ){
		// Is projP is in the triangle, return distance from P to its projection
		// on the plane
		dMin = CGAL::squared_distance( p, projP ) ;
	}else{
		// Else, the distance is the minimum from P to triangle sides
		dMin = CGAL::squared_distance( p, Segment_3(a,b) ) ;
		dMin = std::min( dMin, CGAL::squared_distance( p, Segment_3(b,c) ) );
		dMin = std::min( dMin, CGAL::squared_distance( p, Segment_3(c,a) ) );
	}

	return dMin ;
}

///
///
///
double distancePointTriangle3D( const Point & p_, const Point& a_, const Point& b_, const Point& c_ )
{
	BOOST_ASSERT( ! p_.isEmpty() );
	BOOST_ASSERT( ! a_.isEmpty() );
	BOOST_ASSERT( ! b_.isEmpty() );
	BOOST_ASSERT( ! c_.isEmpty() );

	Point_3 p = p_.toPoint_3< Kernel >();
	Point_3 a = a_.toPoint_3< Kernel >();
	Point_3 b = b_.toPoint_3< Kernel >();
	Point_3 c = c_.toPoint_3< Kernel >();

	squared_distance_t dMin = distancePointTriangle3D(p,a,b,c);
	return CGAL::sqrt( CGAL::to_double( dMin ) ) ;
}



///
///
///
double distanceSegmentSegment3D( const Point & a, const Point & b, const Point & c, const Point & d )
{
	BOOST_ASSERT( ! a.isEmpty() );
	BOOST_ASSERT( ! b.isEmpty() );
	BOOST_ASSERT( ! c.isEmpty() );
	BOOST_ASSERT( ! d.isEmpty() );

	return CGAL::sqrt( CGAL::to_double(
		CGAL::squared_distance(
			CGAL::Segment_3< Kernel >( a.toPoint_3< Kernel >(), b.toPoint_3< Kernel >() ),
			CGAL::Segment_3< Kernel >( c.toPoint_3< Kernel >(), d.toPoint_3< Kernel >() )
		)
	) );
}



///
///
///
double distanceSegmentTriangle3D( const Point & sA_, const Point & sB_,
	const Point & tA_, const Point & tB_, const Point & tC_
)
{
	typedef Kernel::FT squared_distance_t ;

	Point_3 sA = sA_.toPoint_3< Kernel >();
	Point_3 sB = sB_.toPoint_3< Kernel >();
	Segment_3 sAB( sA, sB );

	Point_3 tA = tA_.toPoint_3< Kernel >();
	Point_3 tB = tB_.toPoint_3< Kernel >();
	Point_3 tC = tC_.toPoint_3< Kernel >();
	Triangle_3 tABC( tA, tB, tC );

	/*
	 * If [sAsB] intersects the triangle (tA,tB,tC), distance is 0.0
	 */
	if ( CGAL::intersection( sAB, tABC ).empty() ){
		return 0.0 ;
	}

	/*
	 * else, distance is the min of the following values :
	 * - distance from sA to the Triangle
	 * - distance from sB to the Triangle
	 * - distance from sAB to the side of the Triangles
	 */
	squared_distance_t dMin = distancePointTriangle3D( sA, tA, tB, tC );
	dMin = std::min( dMin, distancePointTriangle3D( sB, tA, tB, tC ) );

	dMin = std::min( dMin, CGAL::squared_distance( sAB, Segment_3( tA, tB ) ) ) ;
	dMin = std::min( dMin, CGAL::squared_distance( sAB, Segment_3( tB, tC ) ) ) ;
	dMin = std::min( dMin, CGAL::squared_distance( sAB, Segment_3( tC, tA ) ) ) ;

	return CGAL::sqrt( CGAL::to_double( dMin ) ) ;
}



}//namespace algorithm
}//namespace SFCGAL
