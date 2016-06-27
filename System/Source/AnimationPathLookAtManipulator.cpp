#include "../Headers/AnimationPathLookAtManipulator.h"

void AnimationPathLookAtManipulator::init(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	home(ea, us);
}

void AnimationPathLookAtManipulator::home(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	home(ea.time());
}

void AnimationPathLookAtManipulator::home(double current_time)
{
	if (valid())
		time_offset = eye_animation_path->getFirstTime() - current_time; 
}
bool AnimationPathLookAtManipulator::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
{
	if( !valid() ) 
		return false;

	if( ea.getEventType() == osgGA::GUIEventAdapter::FRAME )
		handleFrame( ea.time() );

	return false;
}
void AnimationPathLookAtManipulator::handleFrame(double time)
{
	osg::AnimationPath::ControlPoint eye_control_point, center_control_point, up_control_point;

	double anim_time = (time + time_offset);

	eye_animation_path->getInterpolatedControlPoint(anim_time, eye_control_point);
	center_animation_path->getInterpolatedControlPoint(anim_time, center_control_point);
	up_animation_path->getInterpolatedControlPoint(anim_time, up_control_point);

	matrix = osg::Matrix::lookAt(eye_control_point.getPosition(), center_control_point.getPosition(), up_control_point.getPosition());
}
void AnimationPathLookAtManipulator::insert(double const time, osg::Vec3d const &eye, osg::Vec3d const &center, osg::Vec3d const &up)
{
	eye_animation_path->insert(time, osg::AnimationPath::ControlPoint(eye));
	center_animation_path->insert(time, osg::AnimationPath::ControlPoint(center));
	up_animation_path->insert(time, osg::AnimationPath::ControlPoint(up));
}