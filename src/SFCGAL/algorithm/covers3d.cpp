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
#include <SFCGAL/algorithm/covers.h>
#include <SFCGAL/detail/GetPointsVisitor.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
// unpublished code ...
#if 0
//#include <CGAL/Point_inside_polyhedron_3.h>
#else
// use a non-robust version for now
#include <CGAL/Polyhedral_mesh_domain_3.h>
#endif

#include <SFCGAL/algorithm/intersects.h>
#include <SFCGAL/all.h>
#include <SFCGAL/detail/GeometrySet.h>

#include <SFCGAL/io/GeometryStreams.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

namespace SFCGAL {
namespace algorithm
{
	//
	// Test points inside a volume. Optimisation preventing recomputing of AABB trees needed by point_inside_polyhedron
	//
	bool covers3D( const Solid& solid, std::vector<const Point*>& pts )
	{
		typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
		typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron, Kernel> Mesh_domain;
		
		std::auto_ptr<Polyhedron> ext_shell_poly = solid.exteriorShell().toPolyhedron_3<Kernel, Polyhedron>();
		BOOST_ASSERT( ext_shell_poly.get() != 0 );
		Mesh_domain ext_domain( *ext_shell_poly );
		Mesh_domain::Is_in_domain is_in_ext( ext_domain );
		
		//		CGAL::Point_inside_polyhedron_3<Polyhedron, Kernel> point_inside_ext( *ext_shell_poly );
		
		for ( size_t j = 0; j < pts.size(); ++j ) {
			//			if ( !point_inside_ext( pts[j]->toPoint_3<Kernel>() )) {
			if ( ! is_in_ext( pts[j]->toPoint_3() )) {
				return false;
			}
		}
		
		//
		// test whether points are inside interior shells or not
		//
		for ( size_t i = 0; i < solid.numInteriorShells(); ++i ) {
			std::auto_ptr<Polyhedron> shell_poly = solid.interiorShellN(i).toPolyhedron_3<Kernel, Polyhedron>();
			Mesh_domain shell_domain( *shell_poly );
			Mesh_domain::Is_in_domain is_in_shell( shell_domain );
			//			CGAL::Point_inside_polyhedron_3<Polyhedron, Kernel> point_inside( *shell_poly );
			for ( size_t j = 0; j < pts.size(); ++j ) {
				//if ( point_inside( pts[j]->toPoint_3<Kernel>() )) {
				if ( is_in_shell( pts[j]->toPoint_3() )) {
					// FIXME : process nested holes
					return false;
				}
			}
		}
		
		return true;
	}

	static bool covers3D_solid_x_( const Solid& solid, const Geometry& ga )
	{
		//
		// Now consider the geometry as a bunch of points
		detail::GetPointsVisitor visitor;
		ga.accept( visitor );
		return covers3D( solid, visitor.points );
	}

	using namespace SFCGAL::detail;

	bool covers3D( const Geometry& ga, const Geometry& gb )
	{
		if ( ga.isEmpty() || gb.isEmpty() ) {
			return false;
		}
		GeometrySet<3> gsa( ga );
		GeometrySet<3> gsb( gb );

		return covers( gsa, gsb );
	}
}
}
