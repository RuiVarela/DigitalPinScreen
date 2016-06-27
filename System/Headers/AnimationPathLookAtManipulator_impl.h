inline AnimationPathLookAtManipulator::AnimationPathLookAtManipulator()
:eye_animation_path(new osg::AnimationPath()), center_animation_path(new osg::AnimationPath()), up_animation_path(new osg::AnimationPath())
{
	eye_animation_path->setLoopMode(osg::AnimationPath::LOOP);
	center_animation_path->setLoopMode(osg::AnimationPath::LOOP);
	up_animation_path->setLoopMode(osg::AnimationPath::LOOP);

	time_offset = 0.0;
}
inline void AnimationPathLookAtManipulator::setByMatrix(const osg::Matrixd& matrix)
{ 
	this->matrix = matrix; 
}
inline void AnimationPathLookAtManipulator::setByInverseMatrix(const osg::Matrixd& matrix) 
{
	this->matrix.invert(matrix); 
} 
inline osg::Matrixd AnimationPathLookAtManipulator::getMatrix() const
{
	return matrix; 
}
inline osg::Matrixd AnimationPathLookAtManipulator::getInverseMatrix() const 
{
	return matrix; //osg::Matrixd::inverse(_matrix);
}
inline const char* AnimationPathLookAtManipulator::className() const
{
	return "AnimationPathLookAt";
}
inline bool AnimationPathLookAtManipulator::valid() const
{ 
	return (eye_animation_path.valid() && center_animation_path.valid() && up_animation_path.valid());
}
