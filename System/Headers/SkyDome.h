#ifndef __SKY_DOME__
#define __SKY_DOME__

#include <string>

#include <osgDB/ReadFile>
#include <osg/TextureCubeMap>
#include <osg/ClearNode>
#include <osg/TexMat>
#include <osg/Transform>
#include <osg/Transform>

class SkyDome
{
public:
	struct TexMatCallback : public osg::NodeCallback
	{
	public:
		TexMatCallback(osg::TexMat* tm);
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	private:
		osg::ref_ptr<osg::TexMat> _texMat;
	};

	class MoveEarthySkyWithEyePointTransform : public osg::Transform
	{
	public:
		virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;
		virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const;
	};


	SkyDome();
	SkyDome(std::string const& filename);
	void setSkyDomeFile(std::string const& filename);
	void buildSkyDome();
	osg::Node* getNode();
private:
	osg::TextureCubeMap* readCubeMap();
	std::string skydome;
	osg::ref_ptr<osg::ClearNode> clearNode;
	bool started;
	osg::ref_ptr<osg::Transform> transform;
};

#include "SkyDome_impl.h"

#endif // __PIN_SCREEN__