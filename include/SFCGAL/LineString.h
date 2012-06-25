#ifndef _SFCGAL_LINESTRING_H_
#define _SFCGAL_LINESTRING_H_

#include <vector>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <SFCGAL/Point.h>

#include <CGAL/Polygon_2.h>

namespace SFCGAL {

	/**
	 * A LineString in SFA
	 *
	 * @todo template < size_t N >?
	 */
	class LineString : public Geometry {
	public:
		/**
		 * Empty LineString constructor
		 */
		LineString() ;
		/**
		 * Constructor with a point vector
		 */
		LineString( const std::vector< Point > & points ) ;
		/**
		 * Empty LineString constructor
		 */
		LineString( const Point & startPoint, const Point & endPoint ) ;
		/**
		 * Copy constructor
		 */
		LineString( LineString const& other ) ;

		/**
		 * assign operator
		 */
		LineString& operator = ( const LineString & other ) ;

		/**
		 * destructor
		 */
		~LineString() ;

		//-- SFCGAL::Geometry
		virtual LineString *   clone() const ;

		//-- SFCGAL::Geometry
		virtual std::string    geometryType() const ;
		//-- SFCGAL::Geometry
		virtual GeometryType   geometryTypeId() const ;
		//-- SFCGAL::Geometry
		virtual int            dimension() const ;
		//-- SFCGAL::Geometry
		virtual int            coordinateDimension() const ;
		//-- SFCGAL::Geometry
		virtual bool           isEmpty() const ;
		//-- SFCGAL::Geometry
		virtual bool           is3D() const ;


		/**
		 * [SFA/OGC]Returns the number of points
		 */
		inline size_t          numPoints() const { return _points.size(); }
		/**
		 * [SFA/OGC]Returns the n-th point
		 */
		inline const Point  &  pointN( size_t const& n ) const { return _points[n]; }
		/**
		 * [SFA/OGC]Returns the n-th point
		 */
		inline Point &         pointN( size_t const& n ) { return _points[n]; }


		/**
		 * [SFA/OGC]Returns the first point
		 */
		inline const Point &   startPoint() const { return _points.front(); }
		/**
		 * [SFA/OGC]Returns the first point
		 */
		inline Point &         startPoint() { return _points.front(); }


		/**
		 * [SFA/OGC]Returns the first point
		 */
		inline const Point &   endPoint() const { return _points.back(); }
		/**
		 * [SFA/OGC]Returns the first point
		 */
		inline Point &         endPoint() { return _points.back(); }


		const std::vector< Point > & points() const { return _points; }
		std::vector< Point > &       points() { return _points; }


		/**
		 * Const iterator to 2D points
		 */
		template < typename K >
		class Point_2_Iterator :
			public boost::iterator_facade<
			Point_2_Iterator<K>,
			CGAL::Point_2<K> const,
			boost::forward_traversal_tag >
		{
		public:
			Point_2_Iterator() {}
			explicit Point_2_Iterator( std::vector<Point>::const_iterator it ) : it_(it) {}
			Point_2_Iterator( const Point_2_Iterator<K>& other ) : it_(other.it_) {}
		private:
			friend class boost::iterator_core_access;
			void increment() { it_++; }
			bool equal( const Point_2_Iterator<K>& other ) const { return this->it_ == other.it_; }
			const CGAL::Point_2<K>& dereference() const { p_ = it_->toPoint_2<K>(); return p_; }
			mutable CGAL::Point_2<K> p_;
			std::vector<Point>::const_iterator it_;
		};
		template < typename K >
		std::pair< Point_2_Iterator<K>, Point_2_Iterator<K> > points_2() const
		{
			return std::make_pair( Point_2_Iterator<K>(points().begin()), Point_2_Iterator<K>(points().end()) );
		}
		
		/**
		 * Const iterator to 3D points
		 */
		template < typename K >
		class Point_3_Iterator :
			public boost::iterator_facade<
			Point_3_Iterator<K>,
			CGAL::Point_3<K> const,
			boost::forward_traversal_tag >
		{
		public:
			Point_3_Iterator() {}
			explicit Point_3_Iterator( std::vector<Point>::const_iterator it ) : it_(it) {}
			Point_3_Iterator( const Point_3_Iterator<K>& other ) : it_(other.it_) {}
		private:
			friend class boost::iterator_core_access;
			void increment() { it_++; }
			bool equal( const Point_3_Iterator<K>& other ) const { return this->it_ == other.it_; }
			const CGAL::Point_3<K>& dereference() const { p_ = it_->toPoint_3<K>(); return p_; }
			mutable CGAL::Point_3<K> p_;
			std::vector<Point>::const_iterator it_;
		};
		template < typename K >
		std::pair< Point_3_Iterator<K>, Point_3_Iterator<K> > points_3() const
		{
			return std::make_pair( Point_3_Iterator<K>(points().begin()), Point_3_Iterator<K>(points().end()) );
		}

		/*
		 * Convert to CGAL::Polygon_2
		 */

		template < typename K >
		CGAL::Polygon_2<K> toPolygon_2() const
		{
			std::pair< Point_2_Iterator<K>, Point_2_Iterator<K> > p = this->points_2<K>();
			return CGAL::Polygon_2<K>( p.first, p.second );
		}
		//-- visitors

		//-- SFCGAL::Geometry
		virtual void accept( GeometryVisitor & visitor ) ;
		//-- SFCGAL::Geometry
		virtual void accept( ConstGeometryVisitor & visitor ) const ;
	private:
		std::vector< Point > _points ;
	};


}

#endif
