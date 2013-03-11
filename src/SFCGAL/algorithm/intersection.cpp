/**
 *   SFCGAL
 *
 *   Copyright (C) 2012-2013 Oslandia <contact@oslandia.com>
 *   Copyright (C) 2012-2013 IGN (http://www.ign.fr)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <SFCGAL/all.h>
#include <SFCGAL/algorithm/intersection.h>
#include <SFCGAL/algorithm/intersects.h>
#include <SFCGAL/algorithm/intersection.h>
#include <SFCGAL/algorithm/triangulate.h>
#include <SFCGAL/algorithm/collect.h>
#include <SFCGAL/algorithm/collectionHomogenize.h>
#include <SFCGAL/tools/Registry.h>
#include <SFCGAL/detail/GeometrySet.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

//
// Intersection kernel

namespace SFCGAL {

typedef CGAL::Point_2<Kernel> Point_2;
typedef CGAL::Segment_2<Kernel> Segment_2;
typedef CGAL::Triangle_2<Kernel> Triangle_2;

namespace algorithm
{
#if 0
	inline std::auto_ptr<Geometry> _new_empty_geometry( const GeometryType& typeId )
	{
		return std::auto_ptr<Geometry>( tools::Registry::instance().newGeometryByTypeId( typeId ) );
	}

	static std::auto_ptr<Geometry> intersection_point_x_( const Point& pt, const Geometry& gb )
	{
		if ( intersects( pt, gb )) {
			return std::auto_ptr<Geometry>(new Point(pt));
		}
		return _new_empty_geometry( gb.geometryTypeId() );
	}

	static std::auto_ptr<Geometry> intersection_triangles_( const Triangle& tria, const Triangle& trib )
	{
		CGAL::Object obj = CGAL::intersection( tria.toTriangle_2(), trib.toTriangle_2() );
		if (obj.empty()) {
			return _new_empty_geometry( SFCGAL::TYPE_TRIANGLE );
		}
		return std::auto_ptr<Geometry>(Geometry::fromCGAL( obj ));
	}

	static std::auto_ptr<Geometry> intersection_polygons_( const Polygon& polya, const Polygon& polyb )
	{
		std::list<CGAL::Polygon_with_holes_2<Kernel> > opolys;
		if ( polya.hasInteriorRings() || polyb.hasInteriorRings() ) {
			CGAL::Polygon_with_holes_2<Kernel> pa = polya.toPolygon_with_holes_2();
			CGAL::Polygon_with_holes_2<Kernel> pb = polyb.toPolygon_with_holes_2();
			CGAL::intersection( pa,
					    pb,
					    std::back_inserter(opolys) );
		}
		else {
			// a little faster than using Polygon_with_holes when it is not needed
			CGAL::Polygon_2<Kernel> pa = polya.toPolygon_2();
			CGAL::Polygon_2<Kernel> pb = polyb.toPolygon_2();
			CGAL::intersection( pa,
					    pb,
					    std::back_inserter(opolys) );
		}

		if ( opolys.size() == 0 ) {
			return _new_empty_geometry( SFCGAL::TYPE_POLYGON );
		}
		if ( opolys.size() == 1 ) {
			return std::auto_ptr<Geometry>( new Polygon( *opolys.begin() ));
		}
		
		MultiPolygon* mp = new MultiPolygon;
		std::list<CGAL::Polygon_with_holes_2<Kernel> >::const_iterator it;
		for ( it = opolys.begin(); it != opolys.end(); ++it ) {
			mp->addGeometry( new Polygon( *it));
		}
		return std::auto_ptr<Geometry>(mp);
	}

	///
	/// intersections involving Box_d
	///
	template <int TA, int TB>
	static std::auto_ptr<Geometry> intersection_box_d_( const Geometry& ga, const Geometry& gb )
	{		
		using namespace SFCGAL::detail;
		std::vector<Object2Box> aboxes, bboxes;
		std::list<ObjectHandle> ahandles, bhandles;

		to_boxes<2>( ga, ahandles, aboxes );
		to_boxes<2>( gb, bhandles, bboxes );
		
		algorithm::intersection_cb<TA, TB, 2> cb;
		CGAL::box_intersection_d( aboxes.begin(), aboxes.end(), 
					  bboxes.begin(), bboxes.end(),
					  cb );

		if ( cb.base.geometries->size() == 1 ) {
			// copy
			std::auto_ptr<Geometry> g(*cb.base.geometries->begin());
			return g;
		}

		return collectionHomogenize( std::auto_ptr<Geometry>( cb.base.geometryCollection().release() ) );
	}

	std::auto_ptr<Geometry> intersection( const Geometry& ga, const Geometry& gb )
	{
		using namespace SFCGAL::detail;

		//
		// return EMPTY if bboxes do not overlap
		if ( !Envelope::overlaps( ga.envelope(), gb.envelope() )) {
			return _new_empty_geometry( gb.geometryTypeId() );
		}

		// deal with geometry collection
		// call intersection on each geometry of the collection
		const GeometryCollection* coll;
		if ( (coll = dynamic_cast<const GeometryCollection*>( &ga )) ) {
			GeometryCollection *ret = new GeometryCollection();
			for ( size_t i = 0; i < coll->numGeometries(); i++ ) {
				ret->addGeometry(intersection( coll->geometryN( i ), gb).release());
			}
			return std::auto_ptr<Geometry>( ret );
		}
		if ( (coll = dynamic_cast<const GeometryCollection*>( &gb )) ) {
			GeometryCollection *ret = new GeometryCollection();
			for ( size_t i = 0; i < coll->numGeometries(); i++ ) {
				ret->addGeometry(intersection( coll->geometryN( i ), ga).release());
			}
			return std::auto_ptr<Geometry>( ret );
		}

		switch ( ga.geometryTypeId() ) {
		case TYPE_POINT:
			return intersection_point_x_( static_cast<const Point&>(ga), gb );
		case TYPE_LINESTRING:
			switch ( gb.geometryTypeId() ) {
			case TYPE_LINESTRING:
				return intersection_box_d_<ObjectHandle::Segment, ObjectHandle::Segment>( ga, gb );
			case TYPE_TRIANGLE:
			case TYPE_TRIANGULATEDSURFACE:
				return intersection_box_d_<ObjectHandle::Segment, ObjectHandle::Triangle>( ga, gb );
			case TYPE_POLYGON:
			case TYPE_POLYHEDRALSURFACE:
			case TYPE_SOLID:
				// generic processing
				break;
			default:
				// symmetric call
				return intersection( gb, ga );
			}
			break;
		case TYPE_TRIANGLE:
			switch ( gb.geometryTypeId() ) {
			case TYPE_TRIANGLE:
				return intersection_triangles_( static_cast<const Triangle&>(ga), static_cast<const Triangle&>(gb) );
			case TYPE_TRIANGULATEDSURFACE:
				return intersection_box_d_<ObjectHandle::Triangle, ObjectHandle::Triangle>( ga, gb );
			case TYPE_POLYGON:
			case TYPE_POLYHEDRALSURFACE:
			case TYPE_SOLID:
				break;
			default:
				// symmetric call
				return intersection( gb, ga );
			}
			break;
		case TYPE_POLYGON:
			switch ( gb.geometryTypeId() ) {
			case TYPE_POLYGON:
				return intersection_polygons_( static_cast<const Polygon&>(ga), static_cast<const Polygon&>(gb) );
			case TYPE_TRIANGULATEDSURFACE:
			case TYPE_POLYHEDRALSURFACE:
			case TYPE_SOLID:
				{
					TriangulatedSurface surf;
					algorithm::triangulate2D( static_cast<const PolyhedralSurface&>(ga), surf);
					return intersection( ga, surf );
				} break;
			default:
				// symmetric call
				return intersection( gb, ga );
			}
		case TYPE_TRIANGULATEDSURFACE:
			switch ( gb.geometryTypeId() ) {
			case TYPE_TRIANGULATEDSURFACE:
				return intersection_box_d_<ObjectHandle::Triangle, ObjectHandle::Triangle>( ga, gb );
			case TYPE_POLYHEDRALSURFACE:
			case TYPE_SOLID:
				break;
			default:
				// symmetric call
				return intersection( gb, ga );
			}
			break;
		case TYPE_POLYHEDRALSURFACE:
			{
				TriangulatedSurface surf;
				algorithm::triangulate2D( static_cast<const PolyhedralSurface&>(ga), surf);
				return intersection( surf, gb );
			} break;
		default:
			// TODO
			break;
		}

		if ( gb.geometryTypeId() == TYPE_POLYHEDRALSURFACE || gb.geometryTypeId() == TYPE_POLYGON ) {
			TriangulatedSurface surf;
			algorithm::triangulate2D( gb, surf);
			return intersection( ga, surf );
		}
		
		throw std::runtime_error( "intersection() not supported on " + ga.geometryType() + " x " + gb.geometryType() );
		// null object
		return std::auto_ptr<Geometry>();
	}
#endif
	using namespace SFCGAL::detail;

	void intersection( const PrimitiveHandle<3>& pa, const PrimitiveHandle<3>& pb,
			   GeometrySet<3>& output, dim_t<3> );
	void intersection( const PrimitiveHandle<2>& pa, const PrimitiveHandle<2>& pb,
			   GeometrySet<2>& output, dim_t<2> );
	//
	// We deal here with symmetric call
	template <int Dim>
	void dispatch_intersection_sym( const PrimitiveHandle<Dim>& pa, const PrimitiveHandle<Dim>& pb,
					     GeometrySet<Dim>& output )
	{
		// assume types are ordered by dimension within the boost::variant
		if ( pa.handle.which() >= pb.handle.which() ) {
			intersection( pa, pb, output, dim_t<Dim>() );
		}
		else {
			intersection( pb, pa, output, dim_t<Dim>() );
		}
	}

	template <int Dim>
	struct intersection_cb
	{
		intersection_cb( GeometrySet<Dim>& out ) : output(out) {}

		void operator()( const typename PrimitiveBox<Dim>::Type& a,
				 const typename PrimitiveBox<Dim>::Type& b )
		{
			dispatch_intersection_sym<Dim>( *a.handle(), *b.handle(), output );
		}

		GeometrySet<Dim>& output;
	};

	template <int Dim>
	void intersection( const GeometrySet<Dim>& a, const GeometrySet<Dim>& b, GeometrySet<Dim>& output )
	{
		typename SFCGAL::detail::HandleCollection<Dim>::Type ahandles, bhandles;
		typename SFCGAL::detail::BoxCollection<Dim>::Type aboxes, bboxes;
		a.computeBoundingBoxes( ahandles, aboxes );
		b.computeBoundingBoxes( bhandles, bboxes );

		intersection_cb<Dim> cb( output );
		CGAL::box_intersection_d( aboxes.begin(), aboxes.end(),
					  bboxes.begin(), bboxes.end(),
					  cb );
	}

	template void intersection<2>( const GeometrySet<2>& a, const GeometrySet<2>& b, GeometrySet<2>& );
	template void intersection<3>( const GeometrySet<3>& a, const GeometrySet<3>& b, GeometrySet<3>& );

	std::auto_ptr<Geometry> intersection( const Geometry& ga, const Geometry& gb )
	{
		using SFCGAL::detail::GeometrySet;

		detail::GeometrySet<2> gsa( ga ), gsb( gb ), output;
		algorithm::intersection( gsa, gsb, output );

		detail::GeometrySet<2> filtered;
		output.filterCovered( filtered );
		return filtered.recompose();
	}
}
}
