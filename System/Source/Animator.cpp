#include "../Headers/Animator.h"

void LinearMover::operator()(osg::Node* node, osg::NodeVisitor* node_visitor)
{
	if (state > idle)
	{
		if (state == to_start)
		{
			start_time = node_visitor->getFrameStamp()->getReferenceTime();
			state = moving;
		}

		double time = node_visitor->getFrameStamp()->getReferenceTime() - start_time;
		double move_factor = time / duration;

		Pin *pin = &pins[0];

		int const width = screen->getWidth();
		int const height = screen->getHeight();

		for (int current_line = 0; current_line != height; ++current_line)
			for (int current_col = 0; current_col != width; ++current_col)
			{
				if( (*pin).move )
				{
					(*pin).moving_position = move_factor * (*pin).move_delta;

					if(move_factor >= 1.0)
					{
						(*pin).current_position += (*pin).move_delta;		
						(*pin).moving_position = 0.0;
						(*pin).move = false;
						state = idle;
					}

					screen->getPin(current_col, current_line)->asTransform()->asMatrixTransform()->setMatrix( 
						osg::Matrix::translate( (*pin).h_position , (*pin).current_position + (*pin).moving_position , (*pin).v_position) );
				}
				++pin;
			}

			screen->update();
	}
}
void LinearMover::buildMover()
{
	if (pins)
		delete[] pins;

	int const width = screen->getWidth();
	int const height = screen->getHeight();
	
	float const v_spacing = screen->getVerticalSpacing();
	float const h_spacing = screen->getHorizontalSpacing();

	float const v_size = screen->verticalPinSize();
	float const h_size= screen->horizontalPinSize();

	pins = new Pin[width * height];
	Pin *pin = &pins[0];

	for (int current_line = 0; current_line != height; ++current_line)
		for (int current_col = 0; current_col != width; ++current_col)
		{
			float pos_x = (h_spacing + h_size) * current_col;
			float pos_y = (v_spacing + v_size) * current_line;

			(*pin).h_position = pos_x;
			(*pin).v_position = pos_y;
			(*pin).current_position = 0.0f;
			(*pin).moving_position = 0.0f;
			(*pin).move_delta = 0.0f;
			(*pin).move = false;

			++pin;
		}

		state = idle;
}
void ElasticMover::operator()(osg::Node* node, osg::NodeVisitor* node_visitor)
{
	if (animation_state > idle)
	{
		if (animation_state == to_start)
		{
			time = 0;
			animation_time = node_visitor->getFrameStamp()->getReferenceTime();
			animation_time_accumulator = 0.0f;
			animation_state = moving;
		}

		float delta_time = node_visitor->getFrameStamp()->getReferenceTime() - animation_time;
		animation_time = node_visitor->getFrameStamp()->getReferenceTime();

		animation_time_accumulator += delta_time;

		int physics_cycles = animation_time_accumulator / time_increment;

		Pin *pin = &pins[0];

		int const width = screen->getWidth();
		int const height = screen->getHeight();

		for (int current_line = 0; current_line != height; ++current_line)
			for (int current_col = 0; current_col != width ; ++current_col)
			{
				float time = this->time;

				//sync physics fps with rendering fps
				for (int i = 0; i != physics_cycles; ++i)
				{
					integrate( (*pin).current_state, time, time_increment );
					time += time_increment;
				}

				screen->getPin(current_col, current_line)->asTransform()->asMatrixTransform()->setMatrix( 
					osg::Matrix::translate( (*pin).h_position , (*pin).current_state.x , (*pin).v_position) );

				++pin;
			}

			animation_time_accumulator -= physics_cycles * time_increment;
			time += physics_cycles * time_increment;

			screen->update();
	}
}
void ElasticMover::buildMover()
{
	if (pins)
		delete[] pins;

	int const width = screen->getWidth();
	int const height = screen->getHeight();
	
	float const v_spacing = screen->getVerticalSpacing();
	float const h_spacing = screen->getHorizontalSpacing();

	float const v_size = screen->verticalPinSize();
	float const h_size= screen->horizontalPinSize();

	pins = new Pin[width * height];
	Pin *pin = &pins[0];

	for (int current_line = 0; current_line != height; ++current_line)
		for (int current_col = 0; current_col != width; ++current_col)
		{
			float pos_x = (h_spacing + h_size) * current_col;
			float pos_y = (v_spacing + v_size) * current_line;

			(*pin).h_position = pos_x;
			(*pin).v_position = pos_y;

			(*pin).current_state.destination = 0.0f;
			(*pin).current_state.x = 0.0f;
			(*pin).current_state.v = 0.0f;

		//	(*pin).last_state.destination = 0.0f;
		//	(*pin).last_state.x = 0.0f;
		//	(*pin).last_state.v = 0.0f;

			++pin;
		}

		animation_state = idle;
}
