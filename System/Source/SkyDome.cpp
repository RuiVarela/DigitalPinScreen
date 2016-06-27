#include "../Headers/GfxException.h"
#include "../Headers/SkyDome.h"

#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Depth>
#include <osg/NodeCallback>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/CullFace>
#include <osg/ClearNode>

#include <osgUtil/CullVisitor>

void SkyDome::TexMatCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
	if (cv)
	{
		const osg::Matrix& MV = cv->getModelViewMatrix();
		const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f,0.0f,1.0f) * osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f,0.0f,0.0f);
		//osg::Quat q = MV.getRotate();
		osg::Quat q;
		MV.get(q);

		const osg::Matrix C = osg::Matrix::rotate( q.inverse() );

		_texMat->setMatrix( C * R );
	}
	traverse(node,nv);
}
/* Contra a esfera no eyepoint*/
bool SkyDome::MoveEarthySkyWithEyePointTransform::computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
{
	/** Get the transformation matrix which moves from local coords to world coords.*/
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
	if (cv)
	{
		osg::Vec3 eyePointLocal = cv->getEyeLocal();
		matrix.preMult(osg::Matrix::translate(eyePointLocal));
	}
	return true;
}
bool SkyDome::MoveEarthySkyWithEyePointTransform::computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
{
	/** Get the transformation matrix which moves from world coords to local coords.*/
	osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
	if (cv)
	{
		osg::Vec3 eyePointLocal = cv->getEyeLocal();
		matrix.postMult(osg::Matrix::translate(-eyePointLocal));
	}
	return true;
}

osg::TextureCubeMap* SkyDome::readCubeMap()
{
	osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;

	osg::Image* imagePosX = osgDB::readImageFile(skydome + "posx.jpg");
	osg::Image* imageNegX = osgDB::readImageFile(skydome + "negx.jpg");
	osg::Image* imagePosY = osgDB::readImageFile(skydome + "posy.jpg");
	osg::Image* imageNegY = osgDB::readImageFile(skydome + "negy.jpg");
	osg::Image* imagePosZ = osgDB::readImageFile(skydome + "posz.jpg");
	osg::Image* imageNegZ = osgDB::readImageFile(skydome + "negz.jpg");

	if (!(imagePosX && imageNegX && imagePosY && imageNegY && imagePosZ && imageNegZ))
		throw GfxException(5001,"SkyDome file not found");

	cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
	cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
	cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

	cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

	//cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);


	return cubemap;
}
void SkyDome::buildSkyDome()
{
	osg::StateSet* stateset = new osg::StateSet();

	osg::ref_ptr<osg::TexEnv> te = new osg::TexEnv();
	te->setMode(osg::TexEnv::REPLACE);
	stateset->setTextureAttributeAndModes(0, te.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::TexGen> tg = new osg::TexGen;
	tg->setMode(osg::TexGen::NORMAL_MAP);
	stateset->setTextureAttributeAndModes(0, tg.get(), osg::StateAttribute::ON);

	osg::ref_ptr<osg::TexMat> tm = new osg::TexMat;
	stateset->setTextureAttribute(0, tm.get());
	
	osg::ref_ptr<osg::TextureCubeMap> skymap = readCubeMap();
	stateset->setTextureAttributeAndModes(0, skymap.get(), osg::StateAttribute::ON);

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1.0,1.0);   
	stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON );

	stateset->setRenderBinDetails(-1,"RenderBin");

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f),1));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable.get());


	if (started)
	{
		transform->removeChild(0,1);
		clearNode->removeChild(0,1);
	}
		

	transform = new MoveEarthySkyWithEyePointTransform();
	transform->setCullingActive(false);
	transform->addChild(geode.get());

	if (!started)
	{
		clearNode = new osg::ClearNode();
		
	}
	clearNode->setCullCallback(new TexMatCallback(tm.get()));
	clearNode->addChild(transform.get());

	started = true;
}
