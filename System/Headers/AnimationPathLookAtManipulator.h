#ifndef __ANIMATION_PATH_LOOKAT_MANIPULATOR__
#define __ANIMATION_PATH_LOOKAT_MANIPULATOR__

#include <osg/AnimationPath>
#include <osg/Notify>
#include <osgGA/MatrixManipulator>
#include <osgGA/GUIEventAdapter>

class AnimationPathLookAtManipulator : public osgGA::MatrixManipulator
{
public:
	AnimationPathLookAtManipulator();

	virtual const char* className() const;

	/** set the position of the matrix manipulator using a 4x4 Matrix.*/
	virtual void setByMatrix(const osg::Matrixd& matrix);

	/** set the position of the matrix manipulator using a 4x4 Matrix.*/
	virtual void setByInverseMatrix(const osg::Matrixd& matrix);

	/** get the position of the manipulator as 4x4 Matrix.*/
	virtual osg::Matrixd getMatrix() const;

	/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
	virtual osg::Matrixd getInverseMatrix() const;

	bool valid() const;
	void init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	void home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);
	void home(double current_time);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

	void insert(double const time, osg::Vec3d const &eye, osg::Vec3d const &center, osg::Vec3d const& up);
private:
	void handleFrame(double const time);
	osg::ref_ptr<osg::AnimationPath> eye_animation_path;
	osg::ref_ptr<osg::AnimationPath> center_animation_path;
	osg::ref_ptr<osg::AnimationPath> up_animation_path;

	double time_offset;
	osg::Matrixd matrix;
};

#include "AnimationPathLookAtManipulator_impl.h"

#endif // __ANIMATION_PATH_LOOKAT_MANIPULATOR__