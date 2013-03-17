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
#include <fstream>

#include <SFCGAL/all.h>
#include <SFCGAL/io/wkt.h>

#include "../test_config.h"
#include "Bench.h"

#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>

using namespace boost::unit_test ;
using namespace SFCGAL ;

BOOST_AUTO_TEST_SUITE( SFCGAL_BenchWKT )

//
// Test limit case
BOOST_AUTO_TEST_CASE( testReadPoints )
{
	const int N = 100000 ;

	bench().start( boost::format( "READ WKT POINTS" ) );
	for ( int i = 0; i < N; i++ ){
		io::readWkt( "POINT(50000 50000)" ) ;
	}
	bench().stop() ;
}


//
// Test limit case
BOOST_AUTO_TEST_CASE( testReadLineString )
{
	const int N = 100000 ;

	bench().start( boost::format( "READ WKT LINESTRING" ) ) ;
	for ( int i = 0; i < N; i++ ){
		io::readWkt( "LINESTRING(0 0,0 1000,1000 1000)" ) ;
	}
	bench().stop() ;
}

//
// Test limit case
BOOST_AUTO_TEST_CASE( testReadPolygon )
{
	const int N = 100000 ;

	bench().start( boost::format( "READ WKT POLYGON" ) ) ;
	for ( int i = 0; i < N; i++ ){
		io::readWkt( "POLYGON((0 0,0 1000,1000 1000,1000 0,0 0))" ) ;
	}
	bench().stop();
}

//
// Test limit case
BOOST_AUTO_TEST_CASE( testReadTriangle )
{
	const int N = 100000 ;

	bench().start( boost::format( "READ WKT TRIANGLE" ) ) ;
	for ( int i = 0; i < N; i++ ){
		io::readWkt( "TRIANGLE((0 0,0 1000,1000 1000,1000 0,0 0))" ) ;
	}
	bench().stop();
}

//
// Test limit case
BOOST_AUTO_TEST_CASE( testReadSolid )
{
	const int N = 10000 ;

	bench().start( boost::format( "READ WKT SOLID" ) ) ;
	for ( int i = 0; i < N; i++ ){
		io::readWkt( "SOLID((((0.0 0.0 0.0,0.0 1.0 0.0,1.0 1.0 0.0,1.0 0.0 0.0,0.0 0.0 0.0),(0.2 0.2 0.0,0.8 0.2 0.0,0.8 0.8 0.0,0.2 0.8 0.0,0.2 0.2 0.0)),((0.0 0.0 1.0,1.0 0.0 1.0,1.0 1.0 1.0,0.0 1.0 1.0,0.0 0.0 1.0),(0.2 0.2 1.0,0.2 0.8 1.0,0.8 0.8 1.0,0.8 0.2 1.0,0.2 0.2 1.0)),((0.0 0.0 0.0,0.0 0.0 1.0,0.0 1.0 1.0,0.0 1.0 0.0,0.0 0.0 0.0)),((0.0 1.0 0.0,0.0 1.0 1.0,1.0 1.0 1.0,1.0 1.0 0.0,0.0 1.0 0.0)),((1.0 1.0 0.0,1.0 1.0 1.0,1.0 0.0 1.0,1.0 0.0 0.0,1.0 1.0 0.0)),((1.0 0.0 0.0,1.0 0.0 1.0,0.0 0.0 1.0,0.0 0.0 0.0,1.0 0.0 0.0)),((0.2 0.2 0.0,0.2 0.2 1.0,0.8 0.2 1.0,0.8 0.2 0.0,0.2 0.2 0.0)),((0.8 0.2 0.0,0.8 0.2 1.0,0.8 0.8 1.0,0.8 0.8 0.0,0.8 0.2 0.0)),((0.8 0.8 0.0,0.8 0.8 1.0,0.2 0.8 1.0,0.2 0.8 0.0,0.8 0.8 0.0)),((0.2 0.8 0.0,0.2 0.8 1.0,0.2 0.2 1.0,0.2 0.2 0.0,0.2 0.8 0.0))))" ) ;
	}
	bench().stop();
}




BOOST_AUTO_TEST_SUITE_END()




