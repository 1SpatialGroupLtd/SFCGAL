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

#include <SFCGAL/Coordinate.h>

#include <SFCGAL/Kernel.h>
#include <SFCGAL/Exception.h>
#include <SFCGAL/numeric.h>

namespace SFCGAL {

///
///
///
Coordinate::Coordinate():
    _storage( Empty() )
{
}

///
///
///
Coordinate::Coordinate( const Kernel::FT& x, const Kernel::FT& y ):
    _storage( Kernel::Point_2( x, y ) )
{

}

///
///
///
Coordinate::Coordinate( const Kernel::FT& x, const Kernel::FT& y, const Kernel::FT& z ):
    _storage( Kernel::Point_3( x, y, z ) )
{

}

///
///
///
Coordinate::Coordinate( const Kernel::Point_2& other ):
    _storage( other )
{

}

///
///
///
Coordinate::Coordinate( const Kernel::Point_3& other ):
    _storage( other )
{

}


///
///
///
Coordinate::Coordinate( const Coordinate& other ):
    _storage( other._storage )
{

}

///
///
///
Coordinate& Coordinate::operator = ( const Coordinate& other )
{
    _storage = other._storage;
    return *this ;
}

///
///
///
Coordinate::~Coordinate()
{

}


class CoordinateDimensionVisitor : public boost::static_visitor<int> {
public:
    int operator()( const Empty& ) const {
        return 0;
    }
    int operator()( const Epeck::Point_2& ) const {
        return 2;
    }
    int operator()( const Epeck::Point_3& ) const {
        return 3;
    }
    int operator()( const Epick::Point_2& ) const {
        return 2;
    }
    int operator()( const Epick::Point_3& ) const {
        return 3;
    }
};


///
///
///
int Coordinate::coordinateDimension() const
{
    CoordinateDimensionVisitor visitor;
    return boost::apply_visitor( visitor, _storage );
}


///
///
///
bool Coordinate::isEmpty() const
{
    return _storage.which() == 0;
}


class CoordinateIs3dVisitor : public boost::static_visitor<bool> {
public:
    bool operator()( const Empty& /*storage */) const {
        return false ;
    }
    bool operator()( const Epick::Point_2& /*storage */) const {
        return false ;
    }
    bool operator()( const Epick::Point_3& /*storage */) const {
        return true ;
    }
    bool operator()( const Epeck::Point_2& /*storage */) const {
        return false ;
    }
    bool operator()( const Epeck::Point_3& /*storage */) const {
        return true ;
    }
};

///
///
///
bool Coordinate::is3D() const
{
    return boost::apply_visitor( CoordinateIs3dVisitor(), _storage) ;
}

// TODO remove (deprecated)
class GetXVisitor : public boost::static_visitor<Epeck::FT> {
public:
    Epeck::FT operator()( const Empty& ) const {
        BOOST_THROW_EXCEPTION( Exception( "trying to get an empty coordinate z value" ) );
        return 0;
    }
    Epeck::FT operator()( const Epick::Point_2& storage ) const {
        CGAL::Cartesian_converter<Epick,Epeck> converter ;
        return converter(storage).x();
    }
    Epeck::FT operator()( const Epick::Point_3& storage ) const {
        CGAL::Cartesian_converter<Epick,Epeck> converter ;
        return converter(storage).x();
    }
    Epeck::FT operator()( const Epeck::Point_2& storage ) const {
        return storage.x();
    }
    Epeck::FT operator()( const Epeck::Point_3& storage ) const {
        return storage.x();
    }
};

///
///
///
Epeck::FT Coordinate::x() const
{
    GetXVisitor visitor;
    return boost::apply_visitor( visitor, _storage );
}

// TODO remove (deprecated)
class GetYVisitor : public boost::static_visitor<Epeck::FT> {
public:
    Epeck::FT operator()( const Empty& ) const {
        BOOST_THROW_EXCEPTION( Exception( "trying to get an empty coordinate y value" ) );
        return 0;
    }

    Epeck::FT operator()( const Epick::Point_2& storage ) const {
        CGAL::Cartesian_converter<Epick,Epeck> converter ;
        return converter(storage).y();
    }
    Epeck::FT operator()( const Epick::Point_3& storage ) const {
        CGAL::Cartesian_converter<Epick,Epeck> converter ;
        return converter(storage).y();
    }

    Epeck::FT operator()( const Epeck::Point_2& storage ) const {
        return storage.y();
    }
    Epeck::FT operator()( const Epeck::Point_3& storage ) const {
        return storage.y();
    }
};

///
///
///
Epeck::FT Coordinate::y() const
{
    GetYVisitor visitor;
    return boost::apply_visitor( visitor, _storage );
}

// TODO remove (deprecated)
class GetZVisitor : public boost::static_visitor<Epeck::FT> {
public:
    Epeck::FT operator()( const Empty& ) const {
        BOOST_THROW_EXCEPTION( Exception( "trying to get an empty coordinate z value" ) );
        return 0;
    }

    Epeck::FT operator()( const Epick::Point_2& ) const {
        return 0;
    }
    Epeck::FT operator()( const Epick::Point_3& storage ) const {
        CGAL::Cartesian_converter<Epick,Epeck> converter ;
        return converter(storage).z();
    }

    Epeck::FT operator()( const Epeck::Point_2& ) const {
        return 0;
    }
    Epeck::FT operator()( const Epeck::Point_3& storage ) const {
        return storage.z();
    }
};

///
///
///
Epeck::FT Coordinate::z() const
{
    GetZVisitor visitor;
    return boost::apply_visitor( visitor, _storage );
}

//----------------------

/*
 * TODO remove and replace by a rounding conversion to Epick
 *  so that multi-kernel makes sens
 */
class RoundVisitor : public boost::static_visitor<> {
public:
    RoundVisitor( const long& scaleFactor ):
        _scaleFactor( scaleFactor ) {

    }

    void operator()( Empty& ) const {

    }

    void operator()( Epeck::Point_2& storage ) const {
        storage = Kernel::Point_2(
            _roundFT( storage.x() ),
            _roundFT( storage.y() )
        );
    }
    void operator()( Epeck::Point_3& storage ) const {
        storage = Kernel::Point_3(
            _roundFT( storage.x() ),
            _roundFT( storage.y() ),
            _roundFT( storage.z() )
        );
    }


    void operator()( Epick::Point_2& storage ) const {
        storage = Epick::Point_2(
            _roundFT( storage.x() ),
            _roundFT( storage.y() )
        );
    }
    void operator()( Epick::Point_3& storage ) const {
        storage = Epick::Point_3(
            _roundFT( storage.x() ),
            _roundFT( storage.y() ),
            _roundFT( storage.z() )
        );
    }

private:
    long _scaleFactor ;

    Epick::FT _roundFT( const Epick::FT& v ) const {
        return SFCGAL::round( v * _scaleFactor ) / _scaleFactor ;
    }

    // TODO replace return by Epick::FT
    Epeck::FT _roundFT( const Epeck::FT& v ) const {
        return Kernel::FT( CGAL::Gmpq(
            SFCGAL::round( v.exact() * _scaleFactor ),
            _scaleFactor
        ) ) ;
    }

};


Coordinate& Coordinate::round( const long& scaleFactor )
{
    RoundVisitor roundVisitor( scaleFactor ) ;
    boost::apply_visitor( roundVisitor, _storage ) ;
    return *this ;
}



//----------------------

///
///
///
bool Coordinate::operator < ( const Coordinate& other ) const
{
    // no empty comparison
    if ( isEmpty() || other.isEmpty() ) {
        BOOST_THROW_EXCEPTION( Exception( "try to compare empty points using a < b " ) );
    }

    // no mixed dimension comparison
    if ( ( is3D() && ! other.is3D() ) || ( ! is3D() && other.is3D() ) ) {
        BOOST_THROW_EXCEPTION( Exception( "try to compare empty points with different coordinate dimension using a < b" ) );
    }

    // comparison along x
    if ( x() < other.x() ) {
        return true ;
    }
    else if ( other.x() < x() ) {
        return false;
    }

    // comparison along y
    if ( y() < other.y() ) {
        return true ;
    }
    else if ( other.y() < y() ) {
        return false;
    }

    // comparison along z if possible
    if ( is3D() ) {
        if ( z() < other.z() ) {
            return true ;
        }
        else if ( other.z() < z() ) {
            return false;
        }
    }

    // points are equals
    return false;
}

///
///
///
bool Coordinate::operator == ( const Coordinate& other ) const
{
    if ( isEmpty() ) {
        return other.isEmpty() ;
    }

    if ( is3D() || other.is3D() ) {
        return x() == other.x() && y() == other.y()  && z() == other.z() ;
    }
    else {
        return x() == other.x() && y() == other.y() ;
    }
}

///
///
///
bool Coordinate::operator != ( const Coordinate& other ) const
{
    return ! ( *this == other );
}


}//SFCGAL
