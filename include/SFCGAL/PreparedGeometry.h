#ifndef _SFCGAL_PREPARED_GEOMETRY_H_
#define _SFCGAL_PREPARED_GEOMETRY_H_

#include <SFCGAL/Envelope.h>

#include <boost/serialization/split_member.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

#include <stdint.h> // uint32_t

namespace SFCGAL {

class Geometry;

typedef uint32_t srid_t;

/**
 * A PreparedGeometry is a shell around a SFCGAL::Geometry.
 * It is used to store annex data, like SRID or cached computations
 *
 * It is noncopyable since it stores a std::auto_ptr<SFCGAL::Geometry>
 */
class PreparedGeometry : public boost::noncopyable
{
 public:
	/**
	 * Default constructor
	 */
	PreparedGeometry();

	/**
	 * Constructor
	 * @geometry pointer to the underlying SFCGAL::Geometry. Takes ownership
	 */
	PreparedGeometry( std::auto_ptr<Geometry> geometry, srid_t srid = 0 );

	/**
	 * Constructor
	 * @geometry pointer to the underlying SFCGAL::Geometry. Takes ownership
	 */
	PreparedGeometry( Geometry* geometry, srid_t srid = 0 );

	virtual ~PreparedGeometry();

	/**
	 * Geometry accessors
	 */
	const Geometry& geometry() const;
	Geometry& geometry();

	/**
	 * Geometry setter
	 */
	void resetGeometry( Geometry* geom );

	/**
	 * SRID read only accessor
	 */
	const srid_t& SRID() const { return _srid; }

	/**
	 * SRID accessor
	 */
	srid_t& SRID() { return _srid; }

	/**
	 * Envelope accessor (using cache)
	 */
	const Envelope& envelope() const;

	/**
	 * Resets the cache
	 */
	void invalidateCache();

	/**
	 * Convert to an extended WKT (with SRID)
	 * @numDecimals: number of decimals, -1 for keeping the exact rational representation, if possible
	 */
	std::string asEWKT( const int& numDecimals = - 1) const;

	/**
	 * Serializer
	 */
	template <class Archive>
	void save( Archive& ar, const unsigned int version ) const
	{
		ar & _srid;
		const Geometry* pgeom = _geometry.get();
		ar & pgeom;
	}

	template <class Archive>
	void load( Archive& ar, const unsigned int version )
	{
		ar & _srid;
		Geometry* pgeom;
		ar & pgeom;
		_geometry.reset( pgeom );
	}

	template <class Archive>
	void serialize( Archive& ar, const unsigned int version )
	{
		boost::serialization::split_member(ar, *this, version);
	}

protected:
	// Pointer to underlying Geometry
	std::auto_ptr<Geometry> _geometry;

	// SRID of the geometry
	srid_t _srid;

	// bbox of the geometry
	mutable boost::optional<Envelope> _envelope;
};

}

#endif
