#include "../Headers/Animation.h"
#include "../Headers/StringToken.h"

#include <fstream>
#include <iostream>

std::istream& operator>>(std::istream& in, Animation::PlayMode &play_mode)
{
	std::string play_mode_as_string;

	in >> play_mode_as_string;

	if( in.good() )
	{
		if (play_mode_as_string == "no_loop")
			play_mode = Animation::no_loop;
		else if(play_mode_as_string == "loop")
			play_mode = Animation::loop;
		else if(play_mode_as_string == "swing")
			play_mode = Animation::swing;    
		else
			in.setstate(std::ios::failbit);
	}
	return in;
}
std::ostream& operator<<(std::ostream& out, Animation::PlayMode const &play_mode)
{
	std::string play_mode_as_string;
	switch (play_mode)
	{
	case Animation::no_loop :
		play_mode_as_string = "no_loop";
		break;
	case Animation::loop :
		play_mode_as_string = "loop";
		break;     
	case Animation::swing :
		play_mode_as_string = "swing";
		break;      
	}

	return out << play_mode_as_string;
}
void Animation::saveTo(std::ostream& out) const
{
	out << mode << std::endl;

	if (!out)
		throw GfxException(7003,"File error, animation cannot be saved.");
}
void Animation::loadFrom(std::istream& in)
{
	in >> mode >> StringToken::cleanLine;

	if (!in)
		throw GfxException(7004,"File error, animation cannot be loaded.");
}
void LuminanceImageSequence::saveTo(std::ostream& out) const
{
	Animation::saveTo(out);

	out << base_path << std::endl;
	
	out << int(images.size()) << std::endl;

	for (int i = 0; i != images.size(); ++i)
	{
		int size = images[i]->getFileName().size() - base_path.size();
		int begin = base_path.size();

		if (begin < 0)
			begin = 0;

		std::string file_name = images[i]->getFileName().substr(begin, size);
		
		out << file_name << std::endl;
	}

	out << int(frames.size()) << std::endl;

	for (int i = 0; i != frames.size(); ++i)
	{
		out << frames[i].first << " " << frames[i].second << std::endl;
	}

	if (!out)
		throw GfxException(7005,"File error, LuminanceImageSequence cannot be saved.");
}
void LuminanceImageSequence::loadFrom(std::istream& in)
{
	Animation::loadFrom(in);

	free();

	getline(in, base_path);

	int number_of_images;
	in >> number_of_images >> StringToken::cleanLine;

	for (int i = 0; i != number_of_images; ++i)
	{
		std::string filename;
		getline(in, filename);

		loadImage(filename);
	}

	int number_of_frames;
	in >> number_of_frames >> StringToken::cleanLine;

	if (!in)
		throw GfxException(7005,"File error, LuminanceImageSequence cannot be loaded.");

	for (int i = 0; i != number_of_frames; ++i)
	{
		float time;
		int image;

		in >> time >> image >> StringToken::cleanLine;

		if (!in)
			throw GfxException(7005,"File error, LuminanceImageSequence cannot be loaded.");
		

		frames.push_back( LuminanceImageFrame(time, image) );
	}

	if (!in)
		throw GfxException(7005,"File error, LuminanceImageSequence cannot be loaded.");

	finishKeyFrameLoading();
}

void LuminanceImageSequence::finishKeyFrameLoading()
{
	int total_frames = frames.size();

	if ( total_frames > 1 )
	{
		frames_reversed.clear();

		float total_time = 0;

		for (int i = (total_frames - 1); i != 0; --i )
		{
			frames_reversed.push_back( LuminanceImageFrame(total_time, frames[i].second ) );
			total_time += frames[i].first - frames[i - 1].first;
		}
	}
}

void LuminanceImageSequence::operator()(osg::Node* node, osg::NodeVisitor* node_visitor)
{
	if (state > idle)
	{
		if (state == to_start)
		{
			start_time = node_visitor->getFrameStamp()->getReferenceTime();
			state = play_to_right;
		}

		float delta_time = node_visitor->getFrameStamp()->getReferenceTime() - start_time;

		//std::cout << "delta " << delta_time << std::endl;

		switch (state)
		{
		case play_to_right:
			{
				if (delta_time > frames[current_frame].first)
				{
					images[ frames[current_frame].second ]->draw();
					draw->finishMovingPins();

					++current_frame;

					if ( current_frame == int(frames.size()) )
					{
						if (mode == no_loop)
						{
							state = idle;
						}
						else if (mode == loop)
						{
							start_time = node_visitor->getFrameStamp()->getReferenceTime();
							current_frame = 0;
						}
						else // swing
						{
							start_time = node_visitor->getFrameStamp()->getReferenceTime();
							current_frame = 0;
							state = play_to_left;
						}
					}
				}
			}
			break;
		case play_to_left:
			{
				if (delta_time > frames_reversed[current_frame].first)
				{
					images[ frames_reversed[current_frame].second ]->draw();
					draw->finishMovingPins();

					++current_frame;

					if ( current_frame == int(frames_reversed.size()) )
					{
						if (mode == no_loop)
						{
							state = idle;
						}
						else if (mode == loop)
						{
							start_time = node_visitor->getFrameStamp()->getReferenceTime();
							current_frame = 0;
						}
						else // swing
						{
							start_time = node_visitor->getFrameStamp()->getReferenceTime();
							current_frame = 0;
							state = play_to_right;
						}
					}
				}

			}
			break;
		}
	}
}

