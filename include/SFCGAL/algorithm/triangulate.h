#ifndef _SFCGAL_ALGORITHM_TRIANGULATE_H_
#define _SFCGAL_ALGORITHM_TRIANGULATE_H_

#include <SFCGAL/Geometry.h>

namespace SFCGAL {
namespace algorithm {
	/**
	 * Convert a 2D/3D [Multi]Polygon to a TriangulatedSurface.
	 */
	void triangulate( const Geometry & geometry, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D Geometry to a TriangulatedSurface.
	 */
	void triangulate2D( const Geometry & geometry, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D/3D point set to a TriangulatedSurface.
	 * @warning projects points in the OXY plane
	 */
	void triangulate( const MultiPoint & geometry, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D/3D polygon to a TriangulatedSurface.
	 * @warning projects points in the polygon plane
	 */
	void triangulate( const Polygon & polygon, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D polygon to a TriangulatedSurface.
	 */
	void triangulate2D( const Polygon & polygon, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D/3D MultiPolygon to a TriangulatedSurface. Each polygon is triangulated invidualy
	 * @warning projects points in the polygon plane
	 */
	void triangulate( const MultiPolygon & polygon, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D MultiPolygon to a TriangulatedSurface. Each polygon is triangulated invidualy
	 */
	void triangulate2D( const MultiPolygon & polygon, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D/3D PolyhedralSurface to a TriangulatedSurface. Each polygon is triangulated invidualy
	 * @warning projects points in the polygon plane
	 */
	void triangulate( const PolyhedralSurface & poly, TriangulatedSurface & triangulatedSurface ) ;
	/**
	 * Convert a 2D PolyhedralSurface to a TriangulatedSurface. Each polygon is triangulated invidualy
	 */
	void triangulate2D( const PolyhedralSurface & poly, TriangulatedSurface & triangulatedSurface ) ;
}//algorithm
}//SFCGAL

#endif

