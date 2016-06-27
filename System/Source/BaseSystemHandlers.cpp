#include "../Headers/BaseSystemHandlers.h"
#include "../Headers/StringToken.h"
#include "../Headers/Drawable.h"
#include "../Headers/Animator.h"

#include <fstream>

bool ConsoleKeybordEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&)
{
	switch(ea.getEventType())
	{
	case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == '\\')
			{
				_console->toggleConsole();
				return true;
			}
			else if(_console->getState() == Console::State::open)
			{
				_console->handleKeyInput(ea.getKey());

				return true;
			}
		}
	}
	return false;
}
SkyDomeHandler::~SkyDomeHandler()
{
	if (is_owner)
		delete skydome;
}
bool SkyDomeHandler::handleSystemVariable(SystemVariable *const system_variable, char *parameters)
{
	try
	{
		std::string selected = base_dir + std::string(parameters) + "\\";
		system->print("Changing SkyDome to: " + selected);

		skydome->setSkyDomeFile(selected);
		skydome->buildSkyDome();
		world.set(parameters);
	}catch (GfxException exception)
	{

		system->print("Error : " + StringToken::lexical_cast<std::string>(exception.errorCode()) );
		system->print(exception.exceptionString());
	}
	return false; //tell system not to change the systemvar
}

bool PinScreenHandler::handleSystemVariable(SystemVariable *const system_variable, char *parameters)
{
	if (system_variable == &pin_name)
	{
		std::string selected = base_dir + std::string(parameters);
		system->print("Loading Pin: " + selected);
		pin_screen.loadPin(selected);
		pin_name.set(parameters);

		return false; // dont change
	}
	else if (system_variable == &h_spacing)
	{
		float spacing = float(atof(parameters));
		pin_screen.setHorizontalSpacing(spacing);

		return true;
	}
	else if (system_variable == &v_spacing)
	{
		float spacing = float(atof(parameters));
		pin_screen.setVerticalSpacing(spacing);

		return true;
	}
	else if (system_variable == &draw_mode)
	{
		return setDrawMode(parameters);
	}
	else if (system_variable == &draw_linear_duration)
	{
		LinearMover *linear = dynamic_cast<LinearMover*>(draw);
		if (animator && linear)
		{
			float value = float(atof(parameters));
			//	draw_linear_duration.set(value);
			linear->setDuration(value);
		}
		return true;
	}
	else if (system_variable == &ps_max_movement)
	{
		float value = float(atof(parameters));

		if (value == 0)
		{
			system->print("Max movement will be automatically calculated on rebuild.");
		}
		else
		{
			system->print("Max movement will be overriden on rebuild.");
		}
		return true;
	}

	return false;
}
void PinScreenHandler::handleSystemCommand(int const id, char *parameters)
{
	switch(id)
	{
	case rebuild_pin_screen:
		rebuildPinScreen();
		break;

	case draw_pin:
		drawPin(parameters);
		break;

	case draw_circle:
		drawCircle(parameters);
		break;

	case draw_line:
		drawLine(parameters);
		break;

	case load_luminance_image:
		loadLuminanceImage(parameters);
		break;

	case play:
		playAnimation(parameters);
		break;

	case stop:
		stopAnimation();
		break;
	}
}
void PinScreenHandler::drawPin(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 3)
	{
		system->print("Invalid Arguments.");
		system->print("/drawPin <column> <line> <value>");
	}
	else
	{
		char buffer[SystemVariable::max_lenght];

		char *next = StringToken::copyToken(buffer, parameters);
		int x = atoi(next);

		next = StringToken::copyToken(buffer, next);
		int y = atoi(next);

		next = StringToken::copyToken(buffer, next);
		float value = (float) atof(next);

		std::string msg = "Move Pin (" +
			StringToken::lexical_cast<std::string>(x) + "," +
			StringToken::lexical_cast<std::string>(y) + ") " +
			StringToken::lexical_cast<std::string>(value);

		draw->startMovingPins();
		draw->movePin(x, y, value);
		draw->finishMovingPins();

		system->print(msg);
	}
}
void PinScreenHandler::drawCircle(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 4)
	{
		system->print("Invalid Arguments.");
		system->print("/drawCircle <center column> <center line> <radius> <depth>");
	}
	else
	{
		char buffer[SystemVariable::max_lenght];

		char *next = StringToken::copyToken(buffer, parameters);
		int x = atoi(next);

		next = StringToken::copyToken(buffer, next);
		int y = atoi(next);

		next = StringToken::copyToken(buffer, next);
		float radius = (float) atof(next);

		next = StringToken::copyToken(buffer, next);
		float depth = (float) atof(next);

		std::string msg = "Animate Circle (" +
			StringToken::lexical_cast<std::string>(x) + "," +
			StringToken::lexical_cast<std::string>(y) + ") " +
			StringToken::lexical_cast<std::string>(radius) + " , " +
			StringToken::lexical_cast<std::string>(depth);

		Circle circle(x, y, radius, depth);
		circle.setDrawMode(draw);

		draw->startMovingPins();
		circle.draw();
		draw->finishMovingPins();

		system->print(msg);
	}
}
void PinScreenHandler::drawLine(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 5)
	{
		system->print("Invalid Arguments.");
		system->print("/drawLine <center column> <center line> <destination column> <destination line> <depth>");
	}
	else
	{
		char buffer[SystemVariable::max_lenght];

		char *next = StringToken::copyToken(buffer, parameters);
		int x = atoi(next);

		next = StringToken::copyToken(buffer, next);
		int y = atoi(next);

		next = StringToken::copyToken(buffer, next);
		int d_x = atoi(next);

		next = StringToken::copyToken(buffer, next);
		int d_y = atoi(next);

		next = StringToken::copyToken(buffer, next);
		float depth = (float) atof(next);

		std::string msg = "Draw Line (" +
			StringToken::lexical_cast<std::string>(x) + "," +
			StringToken::lexical_cast<std::string>(y) + ") (" +
			StringToken::lexical_cast<std::string>(d_x) + "," +
			StringToken::lexical_cast<std::string>(d_y) + ") " +
			StringToken::lexical_cast<std::string>(depth);

		Line line(x, y, d_x, d_y, depth);
		line.setDrawMode(draw);

		draw->startMovingPins();
		line.draw();
		draw->finishMovingPins();

		system->print(msg);
	}
}
void PinScreenHandler::loadLuminanceImage(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 1)
	{
		system->print("Invalid Arguments.");
		system->print("/loadLuminanceImage <image.ext>");
	}
	else
	{
		try {
			char *next = StringToken::nextToken(parameters);

			std::string filename = images_dir + next;
			std::string msg = std::string("Loading image ") + filename;

			LuminanceImage i;
			i.setDrawMode(draw);
			i.loadImage(filename);

			draw->startMovingPins();
			i.draw();
			draw->finishMovingPins();

			system->print(msg);
		}
		catch (GfxException exception)
		{
			system->print("Error : " + StringToken::lexical_cast<std::string>(exception.errorCode()) );
			system->print(exception.exceptionString());
		}

	}
}
bool PinScreenHandler::setDrawMode(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 1)
	{
		system->print("Invalid Arguments.");
		system->print("/setDrawMode [absolute, relative, linear, elastic]");

		return false;
	}
	else
	{
		if (animation)
		{
			stopAnimation();
		}

		std::string msg = std::string("Setting draw mode ") + parameters;

		if (std::string(parameters) == "absolute")
		{
			if (animator) // é um animator
			{
				pin_screen.getNode()->asGroup()->removeChild(animator);
				animator = 0;
			}
			else
			{
				delete draw;
			}

			draw = new AbsoluteDrawMode();
			draw->setPinScreen(&pin_screen);

			system->print(msg);

			return true; //change
		}
		else if(std::string(parameters) == "relative")
		{
			if (animator) // é um animator
			{
				pin_screen.getNode()->asGroup()->removeChild(animator);
				animator = 0;
			}
			else
			{
				delete draw;
			}


			draw = new RelativeDrawMode();
			draw->setPinScreen(&pin_screen);

			system->print(msg);

			return true; //change
		}
		else if (std::string(parameters) == "linear")
		{
			if (!animator) // n é um animator
			{
				delete draw;
			}

			pin_screen.getNode()->asGroup()->removeChild(animator);
			animator = new osg::Group();
			pin_screen.getNode()->asGroup()->addChild(animator);

			LinearMover *l_mover = new LinearMover();
			animator->setUpdateCallback(l_mover);

			l_mover->setPinScreen( &pin_screen );
			l_mover->setDuration(draw_linear_duration.getFloat());
			l_mover->buildMover();

			draw = l_mover;

			return true;
		}
		else if (std::string(parameters) == "elastic")
		{
			if (!animator) // n é um animator
			{
				delete draw;
			}

			pin_screen.getNode()->asGroup()->removeChild(animator);
			animator = new osg::Group();
			pin_screen.getNode()->asGroup()->addChild(animator);

			ElasticMover *mover = new ElasticMover();
			animator->setUpdateCallback(mover);
			mover->setDamping(spring_damping.getFloat());
			mover->setMass(pin_mass.getFloat());
			mover->setStiffness(spring_stiffness.getFloat());

			mover->setPinScreen( &pin_screen );
			mover->buildMover();

			draw = mover;
			return true;
		}
		else 
		{
			system->print("Invalid mode.");
			system->print("/setDrawMode [absolute, relative, linear, elastic]");
			return false;
		}
	}
}
void ScriptHandler::handleSystemCommand(int const id, char *parameters)
{

	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 1)
	{
		system->print("Invalid Arguments.");
		system->print("/run <script file>");
	}
	else
	{
		char buffer[SystemVariable::max_lenght];

		char *next = StringToken::copyToken(buffer, parameters);

		std::string filename(base_dir + next);
		std::string msg = std::string("Script ") + next + " done.";

		try
		{
			Script script(filename);
			script.setSystem(system);
			script.run();
			system->print(msg);
		}
		catch (GfxException exception)
		{
			system->print("Error : " + StringToken::lexical_cast<std::string>(exception.errorCode()) );
			system->print(exception.exceptionString());
		}
	}
}
void PinScreenHandler::stopAnimation()
{
	if (animation)
	{
		std::string msg = std::string("Animation stop.");	
		system->print(msg);
		animation_group->setUpdateCallback(0);
		pin_screen.getNode()->asGroup()->removeChild(animation_group);
		animation = 0;
	}
}
void PinScreenHandler::playAnimation(char *parameters)
{
	if (StringToken::numberOfTokens(StringToken::trimLeft(parameters)) < 1)
	{
		system->print("Invalid Arguments.");
		system->print("/play <animation file>");
	}
	else
	{
		char buffer[SystemVariable::max_lenght];

		char *next = StringToken::copyToken(buffer, parameters);

		std::string filename(animation_base_dir + next);
		std::ifstream in(filename.c_str());

		if (!in)
		{
			std::string msg = std::string("Unable to load ") + filename + " ...";	
			system->print(msg);
			in.close();
		}
		else
		{
			try
			{
				if (animation)
				{
					stopAnimation();
				}

				animation_group = new osg::Group();
				pin_screen.getNode()->asGroup()->addChild(animation_group);
				animation = new LuminanceImageSequence();
				animation->setDrawMode(draw);
				animation->loadFrom(in);
				animation_group->setUpdateCallback(animation);
				animation->start();

				std::string msg = std::string("Animation ") + next + " ...";
				system->print(msg);
				in.close();

			}
			catch (GfxException exception)
			{
				in.close();
				system->print("Error : " + StringToken::lexical_cast<std::string>(exception.errorCode()) );
				system->print(exception.exceptionString());
			}
		}

	}
}