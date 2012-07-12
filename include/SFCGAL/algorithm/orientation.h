#ifndef _SFCGAL_ALGORITHM_ORIENTATION_H_
#define _SFCGAL_ALGORITHM_ORIENTATION_H_

#include <SFCGAL/Geometry.h>

namespace SFCGAL {
namespace algorithm {
	/**
	 * Test if a Geometry has a consistent orientation
	 */
	bool hasConsistentOrientation3D( const TriangulatedSurface & g ) ;
	/**
	 * Test if a PolyhedralSurface has a consistent orientation
	 */
	bool hasConsistentOrientation3D( const PolyhedralSurface & g ) ;


	/**
	 * Try to make consistent orientation in a TriangulatedSurface
	 */
	void makeConsistentOrientation3D( TriangulatedSurface & g ) ;

}//algorithm
}//SFCGAL


#endif
