#include "GfxException.h"
/*
* Animation
*/
inline Animation::Animation()
:mode(no_loop), state(to_register), draw(0)
{
}
inline Animation::~Animation()
{
}
inline void Animation::setMode(PlayMode const play_mode)
{
	this->mode =  play_mode;
}
inline Animation::PlayMode Animation::getMode() const
{
	return mode;
}
inline void Animation::setDrawMode(DrawMode *draw_mode)
{
	draw = draw_mode;
}
inline DrawMode *Animation::getDrawMode()
{
	return draw;
}
/*
* LuminanceImageAnimation
*/
inline LuminanceImageSequence::LuminanceImageSequence()
:Animation(), current_frame(0), start_time(0)
{
}
inline LuminanceImageSequence::~LuminanceImageSequence()
{
	free();
}
inline void LuminanceImageSequence::free()
{
	for (int i = 0; i != images.size(); ++i)
		delete images[i];
}
inline int LuminanceImageSequence::loadImage(std::string const& filename)
{
	std::string full_filename = base_path + filename;

	LuminanceImage *image = new LuminanceImage();
	image->setDrawMode(draw);
	image->loadImage(full_filename);

	images.push_back(image);

	return images.size();
}
inline void LuminanceImageSequence::addKeyFrame(float const time, int const image)
{
	if (image >= int(images.size()))
		throw GfxException(8001, "Animation KeyFrame out of bounds");

	frames.push_back( LuminanceImageFrame(time, image) );
}
inline void LuminanceImageSequence::setImagesBasePath(std::string const& base_path)
{
	this->base_path = base_path;
}
inline void LuminanceImageSequence::start()
{
	if (frames.size() > 0)
	{
		current_frame = 0;
		state = to_start;
	}
}
inline void LuminanceImageSequence::pause()
{
	
}
