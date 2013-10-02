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

#ifndef _SFCGAL_ALGORITHM_STRAIGHTSKELETON_H_
#define _SFCGAL_ALGORITHM_STRAIGHTSKELETON_H_

#include <SFCGAL/config.h>

#include <memory>

namespace SFCGAL {
class Geometry ;
class Polygon ;
class MultiPolygon ;
class MultiLineString ;
}

namespace SFCGAL {
namespace algorithm {
struct NoValidityCheck;

/**
 * @brief build a 2D straight skeleton for a Polygon
 * @todo add supports for TriangulatedSurface and PolyhedralSurface
 * @todo output M as distance to border?
 * @param g input geometry
 * @param autoOrientation check and fix polygon orientation
 * @ingroup public_api
 * @pre g is a valid geometry
 */
SFCGAL_API std::auto_ptr< MultiLineString > straightSkeleton( const Geometry& g, bool autoOrientation = true ) ;

/**
 * @brief build a 2D straight skeleton for a Polygon
 * @param g input geometry
 * @param autoOrientation check and fix polygon orientation
 * @ingroup public_api
 * @pre g is a valid geometry
 * @warning No actual validity check is done
 */
SFCGAL_API std::auto_ptr< MultiLineString > straightSkeleton( const Geometry& g, bool autoOrientation, NoValidityCheck ) ;

/**
 * @brief build a 2D straight skeleton for a Polygon
 * @ingroup detail
 */
SFCGAL_API std::auto_ptr< MultiLineString > straightSkeleton( const Polygon& g, bool autoOrientation = true ) ;
/**
 * @brief build a 2D straight skeleton for a Polygon
 * @ingroup detail
 */
SFCGAL_API std::auto_ptr< MultiLineString > straightSkeleton( const MultiPolygon& g, bool autoOrientation = true ) ;

}//namespace algorithm
}//namespace SFCGAL


#endif
