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
#include <boost/test/unit_test.hpp>

#include <cmath>

#include <SFCGAL/all.h>
#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/triangulate.h>

using namespace SFCGAL ;

// always after CGAL
using namespace boost::unit_test ;

BOOST_AUTO_TEST_SUITE( SFCGAL_algorithm_TriangulateTest )

BOOST_AUTO_TEST_CASE( testTriangulateTriangle )
{
	std::auto_ptr< Geometry > geometry( io::readWkt("POLYGON((0.0 0.0,1.0 0.0,0.5 0.5,0.0 0.0))") );
	TriangulatedSurface triangulatedSurface ;
	algorithm::triangulate( geometry->as< Polygon >(), triangulatedSurface );
	BOOST_CHECK_EQUAL( triangulatedSurface.numGeometries(), 1U );
	TriangulatedSurface surf2D;
	algorithm::triangulate2D( geometry->as< Polygon >(), surf2D );
	BOOST_CHECK_EQUAL( surf2D.numGeometries(), 1U );
}


BOOST_AUTO_TEST_CASE( testTriangulateSquare2d )
{
	std::auto_ptr< Geometry > geometry( io::readWkt("POLYGON((0.0 0.0,1.0 0.0,1.0 1.0,0.0 1.0,0.0 0.0))") );
	TriangulatedSurface triangulatedSurface ;
	algorithm::triangulate( geometry->as< Polygon >(), triangulatedSurface );
	BOOST_CHECK_EQUAL( triangulatedSurface.numGeometries(), 2U );
	TriangulatedSurface surf2D;
	algorithm::triangulate2D( geometry->as< Polygon >(), surf2D );
	BOOST_CHECK_EQUAL( surf2D.numGeometries(), 2U );
}

BOOST_AUTO_TEST_CASE( testTriangulateVerticalSquare )
{
	std::string wkt( "POLYGON((0.0 0.0 0.0,0.0 0.0 1.0,0.0 1.0 1.0,0.0 1.0 0.0,0.0 0.0 0.0))" );
	std::auto_ptr< Geometry > geometry( io::readWkt(wkt) );
	BOOST_CHECK( geometry->is3D() );
	TriangulatedSurface triangulatedSurface ;
	algorithm::triangulate( geometry->as< Polygon >(), triangulatedSurface );
	BOOST_CHECK_EQUAL( triangulatedSurface.numGeometries(), 2U );
}

BOOST_AUTO_TEST_CASE( testTriangulateConcavePolygon )
{
	std::string wkt("POLYGON((0.2 0.5 0, 0.2 0.5 -1, 0.8 0.5 -1, 0.8 0.5 0, 0.5 0.5 -0.5, 0.2 0.5 0))");
	std::auto_ptr< Geometry > geometry( io::readWkt(wkt) );
	BOOST_CHECK( geometry->is3D() );
	TriangulatedSurface triangulatedSurface ;
	algorithm::triangulate( geometry->as< Polygon >(), triangulatedSurface );
	BOOST_CHECK_EQUAL( triangulatedSurface.numGeometries(), 3U );
}

BOOST_AUTO_TEST_SUITE_END()

