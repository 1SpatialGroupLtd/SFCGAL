/**
 *   SFCGAL
 *
 *   Copyright (C) 2012-2013 Oslandia <infos@oslandia.com>
 *   Copyright (C) 2012-2013 IGN (http://www.ign.fr)
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *   
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.

 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <SFCGAL/algorithm/isValid.h>
#include <SFCGAL/algorithm/difference.h>
#include <SFCGAL/algorithm/volume.h>
#include <SFCGAL/Point.h>
#include <SFCGAL/detail/tools/Registry.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/io/vtk.h>

#include <boost/test/unit_test.hpp>

using namespace SFCGAL;
using namespace boost::unit_test ;

BOOST_AUTO_TEST_SUITE( SFCGAL_algorithm_DifferenceTest )

BOOST_AUTO_TEST_CASE( testDifferenceXPoint )
{
    // The same point
    BOOST_CHECK( algorithm::difference( Point( 0,0 ), Point( 0,0 ) )->isEmpty() );
    // A different point
    BOOST_CHECK( *algorithm::difference( Point( 1,0 ), Point( 0,0 ) ) == Point( 1,0 ) );

    // check difference(X, point) == X
    std::vector<std::string> typeNames;

    for ( size_t i = 0; i < typeNames.size(); ++i ) {
        std::auto_ptr<Geometry> newGeo( tools::Registry::instance().newGeometryByTypeName( typeNames[i] ) );
        std::auto_ptr<Geometry> diffGeo = algorithm::difference( *newGeo, Point( 0, 0 ) );
        BOOST_CHECK( *newGeo == *diffGeo );
    }
}

BOOST_AUTO_TEST_CASE( testDifferenceXLineString )
{
    // Point x Linestring intersecting
    BOOST_CHECK( algorithm::difference( Point( 0,0 ), *io::readWkt( "LINESTRING(0 0,1 1)" ) )->isEmpty() );
    // Point x linestring not intersecting
    BOOST_CHECK( *algorithm::difference( Point( 0,0 ), *io::readWkt( "LINESTRING(0 1,1 1)" ) ) == Point( 0, 0 ) );

    // two linestrings with two segments overlapping
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(0.5 0,0.7 0)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "MULTILINESTRING((0 0,0.5 0),(0.7 0,1 0))" ) );
    }
    // two linestrings with two opposite segments overlapping
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(0.7 0,0.5 0)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "MULTILINESTRING((0 0,0.5 0),(0.7 0,1 0))" ) );
    }
    // two linestrings with two segments crossing
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(-1 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(0 -1,0 1)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *ls1 );
    }
    // two linestrings with two segments partly overlapping
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(-1 0,0.7 0)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "LINESTRING(0.7 0,1 0)" ) );
    }
    // two linestrings with a segment covering another one
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(-1 0,2 0)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( diff->isEmpty() );
    }
    // two linestrings that do not intersect
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(0 1,1 1)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *ls1 );
    }
    // two linestrings with more than one segment
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "LINESTRING(0 0,1 0,1 1)" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "LINESTRING(0.3 0,1 0,1 0.4)" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "MULTILINESTRING((0 0,0.3 0),(1 0.4,1 1))" ) );
    }

    // two identical polygons 
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1))" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1))" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "GEOMETRYCOLLECTION EMPTY" ) );
    }


    // two cubes
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( 
                "SOLID((((0 0 0, 0 1 0, 1 1 0, 1 0 0, 0 0 0)),\
                 ((0 0 0, 0 0 1, 0 1 1, 0 1 0, 0 0 0)),\
                 ((0 0 0, 1 0 0, 1 0 1, 0 0 1, 0 0 0)),\
                 ((1 1 1, 0 1 1, 0 0 1, 1 0 1, 1 1 1)),\
                 ((1 1 1, 1 0 1, 1 0 0, 1 1 0, 1 1 1)),\
                 ((1 1 1, 1 1 0, 0 1 0, 0 1 1, 1 1 1))))" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( 
                "SOLID((((0 0 0, 0 1 0, 1 1 0, 1 0 0, 0 0 0)),\
                 ((0 0 0, 0 0 1, 0 1 1, 0 1 0, 0 0 0)),\
                 ((0 0 0, 1 0 0, 1 0 1, 0 0 1, 0 0 0)),\
                 ((1 1 1, 0 1 1, 0 0 1, 1 0 1, 1 1 1)),\
                 ((1 1 1, 1 0 1, 1 0 0, 1 1 0, 1 1 1)),\
                 ((1 1 1, 1 1 0, 0 1 0, 0 1 1, 1 1 1))))" );
        std::auto_ptr<Geometry> diff = algorithm::difference3D( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "GEOMETRYCOLLECTION EMPTY" ) );
    }
    // two cubes
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( 
                "SOLID((((0 0 0, 0 1 0, 1 1 0, 1 0 0, 0 0 0)),\
                 ((0 0 0, 0 0 1, 0 1 1, 0 1 0, 0 0 0)),\
                 ((0 0 0, 1 0 0, 1 0 1, 0 0 1, 0 0 0)),\
                 ((1 1 1, 0 1 1, 0 0 1, 1 0 1, 1 1 1)),\
                 ((1 1 1, 1 0 1, 1 0 0, 1 1 0, 1 1 1)),\
                 ((1 1 1, 1 1 0, 0 1 0, 0 1 1, 1 1 1))))" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( 
                "SOLID((((0 0 0.5, 0 1 0.5, 1 1 0.5, 1 0 0.5, 0 0 0.5)),\
                 ((0 0 0.5, 0 0 1, 0 1 1, 0 1 0.5, 0 0 0.5)),\
                 ((0 0 0.5, 1 0 0.5, 1 0 1, 0 0 1, 0 0 0.5)),\
                 ((1 1 1, 0 1 1, 0 0 1, 1 0 1, 1 1 1)),\
                 ((1 1 1, 1 0 1, 1 0 0.5, 1 1 0.5, 1 1 1)),\
                 ((1 1 1, 1 1 0.5, 0 1 0.5, 0 1 1, 1 1 1))))" );
        std::auto_ptr<Geometry> diff = algorithm::difference3D( *ls1, *ls2 );
        io::vtk(*diff, "difference.vtk");
        BOOST_CHECK( algorithm::volume(*diff) == Kernel::FT(0.5) );
    }

    // two polygons, one of wich is invalid for CGAL but valid for SFS 
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1))" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1),(-0.5 -0.5,-0.5 0.5,0.5 0.5,1 -0.5,-0.5 -0.5))" );
        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
        BOOST_CHECK( *diff == *io::readWkt( "POLYGON((-0.5 -0.5,1 -0.5,0.5 0.5,-0.5 0.5,-0.5 -0.5))" ) );
    }

    // two polygons, for the second one touching holes is invalid for CGAL but valid for SFS 
    {
        std::auto_ptr<Geometry> ls1 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1))" );
        //std::auto_ptr<Geometry> ls2 = io::readWkt( "POLYGON((-1/1 -1/1,1/1 -1/1,1/1 1/1,-1/1 1/1,-1/1 -1/1),(-1/2 -1/2,-1/2 1/2,-1/10 1/2,1/10 -1/2,-1/2 -1/2),(1/10 -1/2,1/10 1/2,1/2 1/2,1/2 -1/2,1/10 -1/2))" );
        std::auto_ptr<Geometry> ls2 = io::readWkt( "POLYGON((-1 -1,1 -1,1 1,-1 1,-1 -1),(-0.5 -0.5,-0.5 0.5,0 0,-0.5 -0.5),(0.5 0.5,0.5 -0.5,0 0,0.5 0.5))" );
        std::cerr << "ls2 is valid " << algorithm::isValid( *ls2 ) << "\n";
        io::vtk(*ls1, "p1.vtk");
        io::vtk(*ls2, "p2.vtk");

        std::auto_ptr<Geometry> diff = algorithm::difference( *ls1, *ls2 );
    }

    //// check difference(X, linestring) == X, with dimension(X) > 1
    //// TODO: add generators of random geometries to avoid empty geometries here ?
    //std::vector<std::string> typeNames;

    //for ( size_t i = 0; i < typeNames.size(); ++i ) {
    //    std::auto_ptr<Geometry> newGeo( tools::Registry::instance().newGeometryByTypeName( typeNames[i] ) );

    //    if ( newGeo->dimension() > 1 ) {
    //        std::auto_ptr<Geometry> diffGeo = algorithm::difference( *newGeo, Point( 0, 0 ) );
    //        BOOST_CHECK( *newGeo == *diffGeo );
    //    }
    //}
}

BOOST_AUTO_TEST_SUITE_END()

