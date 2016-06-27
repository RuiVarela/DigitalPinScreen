#ifndef __ANIMATION__
#define __ANIMATION__

#include <string>
#include <vector>
#include <iosfwd>

#include <osg/Group>

#include "Drawable.h"

class Animation : public osg::NodeCallback
{
public:
	enum AnimationState
	{
		to_register,
		idle,
		to_start,
		play_to_right,
		play_to_left
	};

	enum PlayMode
	{
		no_loop,
		loop,
		swing,
	};

	Animation();
	virtual ~Animation();

	void setMode(PlayMode const play_mode);
	PlayMode getMode() const;

	void setDrawMode(DrawMode *draw_mode);
	DrawMode *getDrawMode();

	virtual void saveTo(std::ostream& out) const;
	virtual void loadFrom(std::istream& in);

	virtual void start() = 0;
	virtual void pause() = 0;
protected:
	PlayMode mode;
	AnimationState state;
	DrawMode *draw;
};

class LuminanceImageSequence : public Animation
{
public:
	typedef std::pair<float, int> LuminanceImageFrame; 
	LuminanceImageSequence();
	virtual ~LuminanceImageSequence();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* node_visitor);

	virtual void start();
	virtual void pause();

	//return image index
	int loadImage(std::string const& filename);
	void addKeyFrame(float const time, int const image);
	//build the reversed keyframe list for swing mode
	void finishKeyFrameLoading();
	void setImagesBasePath(std::string const& base_path);

	void saveTo(std::ostream& out) const;
	void loadFrom(std::istream& in);

private:
	void free();

	int current_frame;
	double start_time;
	std::vector<LuminanceImage*> images;
	std::vector<LuminanceImageFrame> frames;
	std::vector<LuminanceImageFrame> frames_reversed;
	std::string base_path;
};

std::istream& operator>>(std::istream& in, Animation::PlayMode &play_mode);
std::ostream& operator<<(std::ostream& out, Animation::PlayMode const &play_mode);

#include "Animation_impl.h"

#endif // __ANIMATION__

