#include <SFCGAL/Geometry.h>

#include <SFCGAL/Point.h>
#include <SFCGAL/GeometryVisitor.h>
#include <SFCGAL/io/WktWriter.h>
#include <SFCGAL/detail/GetPointsVisitor.h>

#include <SFCGAL/algorithm/BoundaryVisitor.h>
#include <SFCGAL/detail/EnvelopeVisitor.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Exact_predicates_exact_constructions_kernel ExactKernel;

namespace SFCGAL {

///
///
///
Geometry::~Geometry()
{

}

template <typename Kernel>
Geometry* Geometry::fromCGAL( const CGAL::Object& obj )
{
	typedef CGAL::Point_2<Kernel> Point_2;
	typedef CGAL::Segment_2<Kernel> Segment_2;
	typedef CGAL::Triangle_2<Kernel> Triangle_2;
	
	if ( const Point_2* p = CGAL::object_cast<Point_2>(&obj) ) {
		return new Point(*p);
	}
	else if ( const Segment_2* s = CGAL::object_cast<Segment_2>(&obj) ) {
		return new LineString( s->source(), s->target() );
	}
	else if ( const Triangle_2* t = CGAL::object_cast<Triangle_2>(&obj) ) {
		return new Triangle( t->vertex(0), t->vertex(1), t->vertex(2) );
	}
	else if ( const std::vector<Point_2>* v = CGAL::object_cast<std::vector<Point_2> >(&obj)) {
		MultiPoint* mp = new MultiPoint();
		for ( size_t i = 0; i < v->size(); ++i ) {
			mp->addGeometry( Point((*v)[i]) );
		}
		return mp;
	}
	// else
	std::cout << "No conversion found! " << obj.type().name() << std::endl;
	return 0;
}

template Geometry* Geometry::fromCGAL<Kernel>( const CGAL::Object& );
template Geometry* Geometry::fromCGAL<ExactKernel>( const CGAL::Object& );


///
///
///
std::string Geometry::asText( const int & numDecimals ) const
{
	std::ostringstream oss;
	if ( numDecimals >= 0 ){
		oss << std::fixed ;
		oss.precision( numDecimals );
	}
	io::WktWriter writer( oss );
	writer.write( *this );
	return oss.str();
}

///
///
///
Envelope   Geometry::envelope() const
{
	Envelope box ;
	detail::EnvelopeVisitor envelopeVisitor( box );
	accept(envelopeVisitor);
	return box ;
}

///
///
///
Geometry* Geometry::boundary() const
{
	algorithm::BoundaryVisitor visitor ;
	accept(visitor);
	return visitor.releaseBoundary() ;
}

///
///
///
Geometry::Geometry()
{

}

///
///
///
Geometry::Geometry( Geometry const& other )
{

}

}//SFCGAL

