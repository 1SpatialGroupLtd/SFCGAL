#include <iostream>

#include <QtGui/QApplication>

#include <osgDB/ReadFile>
#include <osgUtil/SmoothingVisitor>

#include <osg/Material>

#include <SFCGAL/all.h>

#include <SFCGAL/viewer/ViewerWidget.h>
#include <SFCGAL/viewer/ViewerWindow.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/io/OsgFactory.h>


#include <SFCGAL/algorithm/extrude.h>

using namespace SFCGAL ;
using namespace SFCGAL::viewer ;


int main(int argc, char** argv)
{
    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    QApplication app(argc, argv);
    ViewerWidget * viewer = ViewerWidget::createFromArguments( arguments );

    //osg::Node* cow = osgDB::readNodeFile("/home/eplu/3RDPARTY/OpenSceneGraph-Data-3.0.0/cow.osgt") ;
    //cow->setName("cow node");
    //viewer->scene()->addChild( cow  );

    osg::Geode* geode = new osg::Geode;
    //osg::StateSet* status = geode->getOrCreateStateSet();
    //status->setMode(GL_LIGHTING, osg::StateAttribute::ON);


	std::auto_ptr< Geometry > g( io::readWkt("POLYGON((0.0 0.0,1.0 0.0,1.0 1.0,0.0 1.0,0.0 0.0),(0.2 0.2,0.8 0.2,0.8 0.8,0.2 0.8,0.2 0.2))")  );
	g->as< Polygon >().ringN(1).reverse();
	std::auto_ptr< Geometry > ext( algorithm::extrude(*g,0.0,0.0,1.0) );

	io::OsgFactory factory ;

	osg::Geometry * osgGeometry = factory.createGeometry( *ext ) ;

	/*
	 * provide a color
	osg::Vec3Array * colors = new osg::Vec3Array();
	colors->push_back( osg::Vec3(1.0f,0.0f,0.0f) );
	osgGeometry->setColorArray( colors );
	osgGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
 */

	/*
	 * provide a material
	 */
	osg::ref_ptr<osg::StateSet> stateSet( geode->getOrCreateStateSet() );
	osg::ref_ptr<osg::Material> material( new osg::Material );
	material->setDiffuse(osg::Material::FRONT,osg::Vec4(1.0f,0.0f,0.0f,1.0f));
	stateSet->setAttribute( material.release() );



	geode->addDrawable( osgGeometry );


	viewer->getScene()->addChild( geode ) ;


	ViewerWindow window( viewer );
	window.show();

	return app.exec();
}
