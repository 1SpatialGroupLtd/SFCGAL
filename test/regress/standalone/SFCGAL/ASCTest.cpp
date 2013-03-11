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
#include <SFCGAL/io/asc.h>

#include "../../../test_config.h"

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test ;

using namespace SFCGAL ;

BOOST_AUTO_TEST_SUITE( SFCGAL_ASCTest )

BOOST_AUTO_TEST_CASE( testReadExample )
{
	std::string filename( SFCGAL_TEST_DIRECTORY );
	filename += "/regress/data/example.asc" ;

	std::ifstream ifs( filename.c_str() );
	BOOST_REQUIRE( ifs.good() ) ;
	std::auto_ptr< Grid > grid( io::readASC( ifs ) ) ;
	BOOST_CHECK_EQUAL( grid->width(), 4U );
	BOOST_CHECK_EQUAL( grid->height(), 6U );

	BOOST_CHECK_EQUAL( grid->dx(), 50.0 );
	BOOST_CHECK_EQUAL( grid->dy(), 50.0 );
}


BOOST_AUTO_TEST_SUITE_END()




