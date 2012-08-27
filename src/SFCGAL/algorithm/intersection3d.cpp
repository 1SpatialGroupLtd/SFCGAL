#include <SFCGAL/all.h>
#include <SFCGAL/algorithm/intersection.h>
#include <SFCGAL/algorithm/intersects.h>
#include <SFCGAL/algorithm/covers.h>
#include <SFCGAL/algorithm/detail/intersects.h>
#include <SFCGAL/algorithm/collect.h>

#include <CGAL/Triangle_3_Triangle_3_intersection.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <CGAL/Point_inside_polyhedron_3.h>
#include <CGAL/intersection_of_Polyhedra_3.h>
#include <CGAL/intersection_of_Polyhedra_3_refinement_visitor.h>
#include <CGAL/corefinement_operations.h>

#include <CGAL/IO/Polyhedron_iostream.h>

//
// Intersection kernel
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel ExactKernel;

typedef CGAL::Point_2<Kernel> Point_2;
typedef CGAL::Segment_2<Kernel> Segment_2;
typedef CGAL::Triangle_2<Kernel> Triangle_2;
typedef CGAL::Point_3<Kernel> Point_3;
typedef CGAL::Segment_3<Kernel> Segment_3;
typedef CGAL::Triangle_3<Kernel> Triangle_3;

template <class Refs>
struct Halfedge_with_mark : public CGAL::HalfedgeDS_halfedge_base<Refs> {
  Halfedge_with_mark()
    : CGAL::HalfedgeDS_halfedge_base<Refs>(), m(false)
  {}

  bool m;   // A boundary marker for faces with different status
  void set_mark() 
  {
    m = true;
  }
};

// An items type using my halfedge.
struct Items_with_mark_on_hedge : public CGAL::Polyhedron_items_3 
{
  template <class Refs, class Traits>
  struct Halfedge_wrapper {
      typedef Halfedge_with_mark<Refs> Halfedge;
  };
};

typedef CGAL::Polyhedron_3<Kernel,Items_with_mark_on_hedge> Polyhedron;
typedef CGAL::Node_visitor_refine_polyhedra<Polyhedron, Kernel, CGAL::Tag_true> Split_visitor;
typedef std::vector<Kernel::Point_3> Polyline_3;

struct Is_not_marked{
  bool operator()(Polyhedron::Halfedge_const_handle h) const{
      return !h->m;
  }
};

namespace SFCGAL {
namespace algorithm
{
	static std::auto_ptr<Geometry> intersection_point_x_( const Point& pt, const Geometry& gb )
	{
		if ( intersects3D( pt, gb )) {
			return std::auto_ptr<Geometry>(new Point(pt));
		}
		return std::auto_ptr<Geometry>(new GeometryCollection());
	}

	static std::auto_ptr<Geometry> intersection_triangles_( const Triangle& tria, const Triangle& trib )
	{
		CGAL::Object obj = CGAL::intersection( tria.toTriangle_3<Kernel>(), trib.toTriangle_3<Kernel>() );
		if (obj.empty()) {
			return std::auto_ptr<Geometry>(new GeometryCollection());
		}
		return std::auto_ptr<Geometry>(Geometry::fromCGAL<Kernel>( obj ));
	}

	///
	/// intersections involving Box_d
	///
	static std::auto_ptr<Geometry> intersection_box_d_( const Geometry& ga, const Geometry& gb )
	{
		std::vector<detail::Object3Box> aboxes, bboxes;
		std::list<detail::ObjectHandle> ahandles, bhandles;

		detail::to_boxes<3>( ga, ahandles, aboxes );
		detail::to_boxes<3>( gb, bhandles, bboxes );
		
		detail::intersection_cb<Kernel,3> cb;
		CGAL::box_intersection_d( aboxes.begin(), aboxes.end(), 
					  bboxes.begin(), bboxes.end(),
					  cb );

		if ( cb.geometries->numGeometries() == 1 ) {
			// copy
			std::auto_ptr<Geometry> g(cb.geometries->geometryN(0).clone());
			delete cb.geometries;
			return g;
		}
		return std::auto_ptr<Geometry>( cb.geometries );
	}

	
	///
	static std::auto_ptr<Geometry> planar_polyhedron_to_geometry( const Polyhedron& poly )
	{
		std::ofstream resf("res.off");
		resf << poly;

		// extract the boundary
		std::list<Point_3> boundary;
		for ( Polyhedron::Halfedge_const_iterator it = poly.halfedges_begin(); it != poly.halfedges_end(); ++it ) {
			if ( !it->is_border() )
				continue;
			Point_3 p1 = it->prev()->vertex()->point();
			Point_3 p2 = it->vertex()->point();
			// TODO: test for colinearity
			// Temporary vertice may have been introduced during triangulations
			// and since we expect here a planar surface, it is safe to simplify the boundary
			// by eliminating colinear points.
			if ( boundary.size() == 0 ) {
				boundary.push_back( p1 );
				boundary.push_back( p2 );
			}
			else if ( boundary.back() == p1 ) {
				boundary.push_back( p2 );
			}
			else if ( boundary.front() == p2 ) {
				boundary.push_front( p1 );
			}
		}
		if ( boundary.size() == 4 ) {
			// It is a triangle
			
			Point p[3];
			std::list<Point_3>::const_iterator it = boundary.begin();
			for ( size_t i = 0; i < 3; ++i, ++it ) {
				p[i] = *it;
			}
			return std::auto_ptr<Geometry>(new Triangle( p[0], p[1], p[2] ));
		}
		
		// Else it is a polygon
		LineString ls;
		for ( std::list<Point_3>::const_iterator it = boundary.begin(); it != boundary.end(); ++it ) {
			ls.addPoint( *it );
		}
		return std::auto_ptr<Geometry>(new Polygon( ls ));
	}

	///
	/// Builds a PolyhedralSurface or TIN if the given Polyhedron is not closed.
	/// Builds a Solid if the given Polyhedron is closed.
	static std::auto_ptr<Geometry> surface_polyhedron_to_geometry( const Polyhedron& poly )
	{
		std::ofstream resf("res.off");
		resf << poly;

		if ( poly.is_pure_triangle() && !poly.is_closed() ) {
			// Build a TIN
			TriangulatedSurface* ret_geo = new TriangulatedSurface();
			for ( Polyhedron::Facet_const_iterator fit = poly.facets_begin(); fit != poly.facets_end(); ++fit ) {
				Point pts[3];
				int i = 0;
				Polyhedron::Halfedge_around_facet_const_circulator hit = fit->facet_begin();
				do {
					pts[i++] = Point( hit->vertex()->point() );
					++i;
				} while ( ++hit != fit->facet_begin());
				ret_geo->addTriangle( Triangle( pts[0], pts[1], pts[2] ));
			}
			return std::auto_ptr<Geometry>( ret_geo );
		}

		// else
		
		// Build a PolyhedralSurface
		std::auto_ptr<PolyhedralSurface> polyhedral( new PolyhedralSurface());

		for ( Polyhedron::Facet_const_iterator fit = poly.facets_begin(); fit != poly.facets_end(); ++fit ) {
			LineString ls;
			Polyhedron::Halfedge_around_facet_const_circulator hit = fit->facet_begin();
			do {
				ls.addPoint(Point( hit->vertex()->point() ));
			} while ( ++hit != fit->facet_begin());
			
			polyhedral->addPolygon( Polygon(ls ));
		}

		if ( poly.is_closed()) {
			// return a solid
			Solid* solid = new Solid( *polyhedral );
			return std::auto_ptr<Geometry>( solid );
		}

		return std::auto_ptr<Geometry>( polyhedral );
	}

	static std::auto_ptr<Geometry> intersection3D_polyhedra( Polyhedron& polya, Polyhedron& polyb, const TriangulatedSurface& tinb, bool is_volume )
	{
		Split_visitor visitor( NULL, true );

		std::list<Polyline_3> polylines;
		CGAL::Intersection_of_Polyhedra_3<Polyhedron,Kernel, Split_visitor> intersect_polys(visitor);
		intersect_polys( polya, polyb, std::back_inserter(polylines) );
		//		std::cout << "# of polylines: " << polylines.size() << std::endl;
		#if 0
		for ( std::list<Polyline_3>::const_iterator it = polylines.begin(); it != polylines.end(); ++it ) {
			std::cout << "polyline: ";
			for ( size_t j = 0; j < it->size(); ++j ) {
				std::cout << (*it)[j] << ",";
			}
			std::cout << std::endl;
		}
		#endif

		std::ofstream pa("polya.off");
		std::ofstream pb("polyb.off");
		pa << polya;
		pb << polyb;

		if ( polylines.size() == 0 ) {
			// no intersection
			return std::auto_ptr<Geometry>( new GeometryCollection() );
		}

		std::list<Polyhedron> decomposition;

		Is_not_marked criterion;
		CGAL::internal::extract_connected_components( polya, criterion, std::back_inserter(decomposition));

		CGAL::Point_inside_polyhedron_3<Polyhedron, Kernel> point_inside_q( const_cast<Polyhedron&>(polyb) );

		//		std::cout << "# of decomposition: " << decomposition.size() << std::endl;
		for ( std::list<Polyhedron>::iterator it = decomposition.begin(); it != decomposition.end(); ++it ) {
			// take a point on the component and tests if its inside the other polyhedron
			//
			CGAL::Point_3<Kernel> test_point;
			//			std::cout << "# of facets: " << it->size_of_facets() << std::endl;
			//			std::cout << "# of vertices: " << it->size_of_vertices() << std::endl;
			if ( it->size_of_facets() == 1 ) {
				//
				// If we only have a facet, take the centroid (cannot take a point on the intersection)
				CGAL::Point_3<Kernel> q[3];
				Polyhedron::Point_iterator pit = it->points_begin();
				for ( size_t i = 0; i < 3; ++i, ++pit ) {
					q[i] = *pit;
				}
				test_point = CGAL::centroid( q[0], q[1], q[2] );
			}
			else {
				//
				// Take a point not an the border
				for ( Polyhedron::Halfedge_iterator hit = it->halfedges_begin(); hit != it->halfedges_end(); ++hit ) {
					if ( !hit->is_border_edge() ) {
						// take the center of the edge
						test_point = CGAL::midpoint( hit->vertex()->point(), hit->prev()->vertex()->point() );
						//						std::cout << "take the center of the edge as test_point " << test_point << std::endl;
						break;
					}
				}
			}
			
			//			std::cout << "test_point = " << test_point << std::endl;
			// point inside volume or on surface
			bool point_inside_volume =  point_inside_q( test_point );
			bool point_on_surface = false;
			if ( !point_inside_volume ) {
				point_on_surface = intersects3D( Point(test_point), tinb );
			}
			//			std::cout << "inside = " << point_inside_volume << " on_surface = " << point_on_surface << std::endl;
			if ( point_inside_volume || point_on_surface ) {
				if ( is_volume ) {
					return surface_polyhedron_to_geometry( *it );
				}
				return planar_polyhedron_to_geometry( *it );
			}
		}

		// else, consider the polylines
		if ( polylines.size() == 1 ) {
			const Polyline_3& lit = *(polylines.begin());
			if ( lit.size() == 1 ) {
				// it's a point
				return std::auto_ptr<Geometry>(new Point(lit[0]));
			}
			else if ( lit.size() == 2 ) {
				// it's a segment
				return std::auto_ptr<Geometry>(new LineString(lit[0], lit[1]));
			}
			BOOST_THROW_EXCEPTION( Exception( "Polylines with more than 2 points !" ));
		}
		// else it's a multipoint or a multilinestring
		GeometryCollection* coll = new GeometryCollection();
		for ( std::list<Polyline_3>::const_iterator lit = polylines.begin(); lit != polylines.end(); ++lit ) {
			if ( lit->size() == 1 ) {
				coll->addGeometry( new Point((*lit)[0]));
			}
			else if ( lit->size() == 2 ) {
				// it's a segment
				coll->addGeometry(new LineString((*lit)[0], (*lit)[1] ));
			}
			else {
				BOOST_THROW_EXCEPTION( Exception( "Polylines with more than 2 points !" ));
			}
		}
		return std::auto_ptr<Geometry>( coll );
	}

	static std::auto_ptr<Geometry> intersection3D_solid_( const Geometry& ga, const Solid& solid )
	{
		// FIXME : only consider the exteriorShell for now
		// for interior shells, one way would be to substract intersection with each of them
		//		std::cout << "polyb to Polyhedron" << std::endl;
		TriangulatedSurface exTri;
		algorithm::triangulate( solid.exteriorShell(), exTri );
		std::auto_ptr<Polyhedron> polyb = exTri.toPolyhedron_3<Kernel, Polyhedron>();

		switch ( ga.geometryTypeId() ) {
		case TYPE_LINESTRING:
			// TODO
			break;
		case TYPE_TRIANGLE: {
			const Triangle& tri = static_cast<const Triangle&>(ga);
			TriangulatedSurface surf;
			surf.addTriangle( tri );
			std::auto_ptr<Polyhedron> polya = surf.toPolyhedron_3<Kernel, Polyhedron>();

			return intersection3D_polyhedra( *polya, *polyb, exTri, false );
		} break;
		case TYPE_POLYGON: {
			TriangulatedSurface surf;
			algorithm::triangulate( static_cast<const Polygon&>(ga), surf );
			std::auto_ptr<Polyhedron> polya = surf.toPolyhedron_3<Kernel, Polyhedron>();

			return intersection3D_polyhedra( *polya, *polyb, exTri, false );
		} break;
		case TYPE_TIN: {
			const TriangulatedSurface& surf = static_cast<const TriangulatedSurface&>(ga);
			std::auto_ptr<Polyhedron> polya = surf.toPolyhedron_3<Kernel, Polyhedron>();
			return intersection3D_polyhedra( *polya, *polyb, exTri, /* is_volume */true );
		} break;
		default:
			break;
		}

		return std::auto_ptr<Geometry>();
	}

	static std::auto_ptr<Geometry> intersection3D_coref_( const Solid& solida, const Solid& solidb )
	{
		typedef CGAL::Polyhedron_corefinement<Polyhedron> Corefinement;

		// FIXME
		// We only consider for now exterior shells

		std::auto_ptr<Polyhedron> polya = solida.exteriorShell().toPolyhedron_3<Kernel, Polyhedron>();
		std::auto_ptr<Polyhedron> polyb = solidb.exteriorShell().toPolyhedron_3<Kernel, Polyhedron>();

		Corefinement coref;
		CGAL::Emptyset_iterator no_polylines;
		// vector of <Polyhedron, tag>
		std::vector<std::pair<Polyhedron*, int> > result;
		coref( *polya, *polyb, no_polylines, std::back_inserter(result), Corefinement::Intersection_tag );

		// empty intersection
		if (result.size() == 0) {
			return std::auto_ptr<Geometry>(new GeometryCollection());
		}

		// else, we have an intersection
		Polyhedron* res_poly = result[0].first;
		return std::auto_ptr<Geometry>( surface_polyhedron_to_geometry( *res_poly ));
	}

	std::auto_ptr<Geometry> intersection3D( const Geometry& ga, const Geometry& gb )
	{
		// deal with geometry collection
		// call intersection on each geometry of the collection
		const GeometryCollection* coll;
		if ( (coll = dynamic_cast<const GeometryCollection*>( &ga )) ) {
			GeometryCollection *ret = new GeometryCollection();
			for ( size_t i = 0; i < coll->numGeometries(); i++ ) {
				ret->addGeometry(intersection3D( coll->geometryN( i ), gb).release());
			}
			return std::auto_ptr<Geometry>( ret );
		}
		if ( (coll = dynamic_cast<const GeometryCollection*>( &gb )) ) {
			GeometryCollection *ret = new GeometryCollection();
			for ( size_t i = 0; i < coll->numGeometries(); i++ ) {
				ret->addGeometry(intersection3D( coll->geometryN( i ), ga).release());
			}
			return std::auto_ptr<Geometry>( ret );
		}


		//
		// test first if one geometry is covered by the other one.
		// In this case, the resulting intersection is the geometry covered by the other one.
		//		std::cout << "covers3D test ..." << std::endl;
		if ( algorithm::covers3D( ga, gb )) {
			return std::auto_ptr<Geometry>( ga.clone() );
		}
		if ( algorithm::covers3D( gb, ga )) {
			return std::auto_ptr<Geometry>( gb.clone() );
		}

		switch ( ga.geometryTypeId() ) {
		case TYPE_POINT:
			return intersection_point_x_( static_cast<const Point&>(ga), gb );
		case TYPE_LINESTRING:
			switch ( gb.geometryTypeId() ) {
			case TYPE_LINESTRING:
			case TYPE_TRIANGLE:
			case TYPE_TIN:
				return intersection_box_d_( ga, gb );
			case TYPE_POLYGON:
			case TYPE_POLYHEDRALSURFACE:
			case TYPE_SOLID:
				// generic processing
				break;
			default:
				// symmetric call
				return intersection3D( gb, ga );
			}
			break;
		case TYPE_TRIANGLE:
			switch ( gb.geometryTypeId() ) {
			case TYPE_TRIANGLE:
				return intersection_triangles_( static_cast<const Triangle&>(ga), static_cast<const Triangle&>(gb) );
			case TYPE_TIN:
				return intersection_box_d_( ga, gb );
			case TYPE_SOLID:
				return intersection3D_solid_( ga, static_cast<const Solid&>(gb) );
			case TYPE_POLYGON:
			case TYPE_POLYHEDRALSURFACE:
				break;
			default:
				// symmetric call
				return intersection3D( gb, ga );
			}
		case TYPE_POLYGON:
			{
				TriangulatedSurface surf;
				algorithm::triangulate( static_cast<const Polygon&>(ga), surf);
				return intersection3D( surf, gb );
			} break;

		case TYPE_TIN:
			switch ( gb.geometryTypeId() ) {
			case TYPE_TIN:
				return intersection_box_d_( ga, gb );
			case TYPE_POLYHEDRALSURFACE:
				break;
			case TYPE_SOLID:
				return intersection3D_solid_( ga, static_cast<const Solid&>(gb) );
			default:
				// symmetric call
				return intersection3D( gb, ga );
			}
		case TYPE_POLYHEDRALSURFACE:
			{
				TriangulatedSurface surf;
				algorithm::triangulate( static_cast<const PolyhedralSurface&>(ga), surf);
				return intersection3D( surf, gb );
			} break;
		case TYPE_SOLID:
			switch ( gb.geometryTypeId() ) {
			case TYPE_SOLID:
				return intersection3D_coref_( static_cast<const Solid&>(ga), static_cast<const Solid&>(gb));
				break;
			default:
				// symmetric call
				return intersection3D( gb, ga );
			}
			break;
		default:
			// TODO
			break;
		}

		if ( gb.geometryTypeId() == TYPE_POLYHEDRALSURFACE || gb.geometryTypeId() == TYPE_POLYGON ) {
			TriangulatedSurface surf;
			algorithm::triangulate( static_cast<const PolyhedralSurface&>(gb), surf);
			return intersection3D( ga, surf );
		}
		
		throw std::runtime_error( "intersection3D() not supported on " + ga.geometryType() + " x " + gb.geometryType() );
		// null object
		return std::auto_ptr<Geometry>();
	}
}
}
