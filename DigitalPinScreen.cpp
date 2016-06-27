#include <osg/Group>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/CullFace>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/CullFace>

#include <osg/TexGen>
#include <osg/PolygonOffset>
#include <osg/Texture2D>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/TexEnvCombine>
#include <osg/TexEnv>
#include <osg/TexGenNode>
#include <osg/CameraNode>

#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PolygonMode>

#include <osgDB/WriteFile>

#include "System/Headers/GfxException.h"
#include "System/Headers/StringToken.h"
#include "System/Headers/Console.h"
#include "System/Headers/System.h"
#include "System/Headers/PinScreen.h"
#include "System/Headers/BaseSystemHandlers.h"
#include "System/Headers/AnimationPathLookAtManipulator.h"
#include "System/Headers/Drawable.h"
#include "System/Headers/Animator.h"
#include "System/Headers/Animation.h"

#include <iostream>
#include <string>
#include <sstream>




// SHADOW
/*
osg::ref_ptr<osg::MatrixTransform> createLight()
{
	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();

	// create a spot light.
	osg::ref_ptr<osg::Light> light_1 = new osg::Light;
	light_1->setLightNum(1);
	light_1->setPosition(osg::Vec4(0, 0, 0, 1.0f));
	light_1->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	light_1->setDiffuse(osg::Vec4(0.9f, 0.8f, 0.8f, 1.0f));
	light_1->setSpotCutoff(60.0f);
	light_1->setSpotExponent(2.0f);

	osg::ref_ptr<osg::LightSource> light_source_1 = new osg::LightSource();    
	light_source_1->setLight(light_1.get());
	light_source_1->setLocalStateSetModes(osg::StateAttribute::ON); 
	transform->addChild(light_source_1.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::ShapeDrawable> shape;
	osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
	hints->setDetailRatio(0.3f);
	shape = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), 0.15f), hints.get());
	shape->setColor(osg::Vec4(1.0f, 0.5f, 0.5f, 1.0f));
	geode->addDrawable(shape.get());
	shape = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f, 0.0f, -0.4f), 0.05f, 0.8f), hints.get());
	shape->setColor(osg::Vec4(1.0f, 0.5f, 0.5f, 1.0f));
	geode->addDrawable(shape.get());
	geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	transform->addChild(geode.get());

	return transform;
}

class UpdateCameraAndTexGenCallback : public osg::NodeCallback
{
public:

	UpdateCameraAndTexGenCallback(osg::MatrixTransform* light_transform, osg::CameraNode* cameraNode, osg::TexGenNode* texgenNode):
	  _light_transform(light_transform),
		  _cameraNode(cameraNode),
		  _texgenNode(texgenNode)
	  {
	  }

	  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	  {
		  // first update subgraph to make sure objects are all moved into postion
		  traverse(node,nv);

		  // now compute the camera's view and projection matrix to point at the shadower (the camera's children)
		  osg::BoundingSphere bs;
		  for(unsigned int i=0; i<_cameraNode->getNumChildren(); ++i)
			  bs.expandBy(_cameraNode->getChild(i)->getBound());

		  osg::Vec3 position = _light_transform->getMatrix().getTrans();

		  float centerDistance = (position - bs.center()).length();

		  float znear = centerDistance - bs.radius();
		  float zfar  = centerDistance + bs.radius();
		  float zNearRatio = 0.001f; 

		  if (znear < zfar * zNearRatio) 
			  znear = zfar * zNearRatio;

		  float top   = (bs.radius() / centerDistance) * znear;
		  float right = top;

		  _cameraNode->setReferenceFrame(osg::CameraNode::ABSOLUTE_RF);
		  _cameraNode->setProjectionMatrixAsFrustum(-right, right, -top, top, znear,zfar);
		  _cameraNode->setViewMatrixAsLookAt(position, bs.center(), osg::Vec3(0.0f, 1.0f, 0.0f));

		  // compute the matrix which takes a vertex from local coords into tex coords
		  // will use this later to specify osg::TexGen..
		  osg::Matrix MVPT = (
			  _cameraNode->getViewMatrix() 
			  * _cameraNode->getProjectionMatrix()
			  * osg::Matrix::translate(1.0,1.0,1.0)
			  * osg::Matrix::scale(0.5f,0.5f,0.5f)
			  );

		  _texgenNode->getTexGen()->setMode(osg::TexGen::EYE_LINEAR);
		  _texgenNode->getTexGen()->setPlanesFromMatrix(MVPT);
	  }
protected:
	virtual ~UpdateCameraAndTexGenCallback() {}
	osg::ref_ptr<osg::MatrixTransform>  _light_transform;
	osg::ref_ptr<osg::CameraNode>       _cameraNode;
	osg::ref_ptr<osg::TexGenNode>       _texgenNode;
};

osg::Group* createShadowedScene(osg::Node* shadowed, osg::MatrixTransform* light_transform, unsigned int unit)
{
	osg::Group* group = new osg::Group;

	unsigned int tex_width = 1280;
	unsigned int tex_height = 800;

	osg::Texture2D* texture = new osg::Texture2D;
	texture->setTextureSize(tex_width, tex_height);

	texture->setInternalFormat(GL_DEPTH_COMPONENT);
	texture->setShadowComparison(true);
	texture->setShadowTextureMode(osg::Texture::LUMINANCE);
	texture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
	texture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);

	// set up the render to texture camera.
	{

		// create the camera
		osg::CameraNode* camera = new osg::CameraNode;
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setClearColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
		camera->setComputeNearFarMode(osg::CameraNode::DO_NOT_COMPUTE_NEAR_FAR);

		// set viewport
		camera->setViewport(0,0,tex_width,tex_height);

		osg::StateSet*  _local_stateset = camera->getOrCreateStateSet();
		_local_stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);


		float factor = 1.0f;
		float units = 0.0f;

		osg::ref_ptr<osg::PolygonOffset> polygon_offset = new osg::PolygonOffset();
		polygon_offset->setFactor(factor);
		polygon_offset->setUnits(units);
		_local_stateset->setAttribute(polygon_offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		_local_stateset->setMode(GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace;
		cull_face->setMode(osg::CullFace::FRONT);
		_local_stateset->setAttribute(cull_face.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		_local_stateset->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


		// set the camera to render before the main camera.
		camera->setRenderOrder(osg::CameraNode::PRE_RENDER);

		// tell the camera to use OpenGL frame buffer object where supported.
		camera->setRenderTargetImplementation(osg::CameraNode::FRAME_BUFFER_OBJECT);

		// attach the texture and use it as the color buffer.
		camera->attach(osg::CameraNode::DEPTH_BUFFER, texture);

		// add subgraph to render
		camera->addChild(shadowed);

		group->addChild(camera);

		// create the texgen node to project the tex coords onto the subgraph
		osg::TexGenNode* texgenNode = new osg::TexGenNode;
		texgenNode->setTextureUnit(unit);
		group->addChild(texgenNode);

		// set an update callback to keep moving the camera and tex gen in the right direction.
		group->setUpdateCallback(new UpdateCameraAndTexGenCallback(light_transform, camera, texgenNode));
	}


	// set the shadowed subgraph so that it uses the texture and tex gen settings.    

	osg::Group* shadowedGroup = new osg::Group;
	shadowedGroup->addChild(shadowed);
	group->addChild(shadowedGroup);

	osg::StateSet* stateset = shadowedGroup->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes(unit,texture,osg::StateAttribute::ON);
	stateset->setTextureMode(unit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON);
	stateset->setTextureMode(unit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON);
	stateset->setTextureMode(unit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON);
	stateset->setTextureMode(unit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON);


	//////////////////////////////////////////////////////////////////
	// fragment shader
	//
	char fragmentShaderSource_noBaseTexture[] = 
		"uniform sampler2DShadow shadowTexture; \n"
		"uniform vec2 ambientBias; \n"
		"\n"
		"void main(void) \n"
		"{ \n"
		"    gl_FragColor = gl_Color * (ambientBias.x + shadow2DProj(shadowTexture, gl_TexCoord[0]) * ambientBias.y); \n"
		"}\n";

	osg::Program* program = new osg::Program;
	stateset->setAttribute(program);

	osg::Shader* fragment_shader = new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource_noBaseTexture);
	program->addShader(fragment_shader);

	osg::Uniform* shadowTextureSampler = new osg::Uniform("shadowTexture",(int)unit);
	stateset->addUniform(shadowTextureSampler);

	osg::Uniform* ambientBias = new osg::Uniform("ambientBias", osg::Vec2(0.4f,1.1f));
	stateset->addUniform(ambientBias);

	// add the shadower and shadowed.
	group->addChild(light_transform);

	return group;
}
*/

// - main ----------------------------------------------------------------------
int main (int argc, char* argv[])
{

	/* Set Up Window */
	
	bool fullscreen = true;
	int screen_height = 768;
	int screen_width = 1024;

	bool record_avi = true;

	// Create a Producer-based viewer
	osgProducer::Viewer viewer;
	viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
	//viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE | osgProducer::Viewer::HEAD_LIGHT_SOURCE);

	// Create a group and add the geometry to it
	osg::Group* group = new osg::Group();

	// Start Console and main system 
	Console console(group, screen_height, screen_width, "Data/Images/console_back.png", "Data/Images/console.png");
	console.addMessage("Registering console keyboard handler..");
	viewer.getEventHandlerList().push_front(new ConsoleKeybordEventHandler(&console));
	console.addMessage("Console Ok.");

	console.addMessage("Starting system...");
	System system;
	system.setConsole(&console);

	//	console.addMessage("Starting SkyDome...");
	//	SkyDomeHandler sky_dome_handler(&system, "Data\\Images\\", "MartianPlanet" );
	//	group->addChild(sky_dome_handler.getSkyDome()->getNode());

	console.addMessage("Starting PinScreen...");
	PinScreenHandler pin_screen_handler(&system, "Data\\Models\\", "cube.osg", 100, 100, 0.01f, 0.01f);
	pin_screen_handler.setImagesDir("Data\\Images\\"); //for luminance images
	pin_screen_handler.setAnimationBaseDir("Data\\Animation\\"); // for animations

	console.addMessage("Starting Script system...");
	ScriptHandler script_handler(&system, "Data\\Scripts\\");

	/* enable back face culling */
	osg::StateSet* state_set = pin_screen_handler.getPinScreen().getNode()->getOrCreateStateSet();
	state_set->setAttribute(new osg::CullFace(osg::CullFace::BACK), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	state_set->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	console.addMessage("Back face culling on.");

	/* Anti aliasing */
	int multi_samples = 0;

	osg::DisplaySettings *display_settings = osg::DisplaySettings::instance();
	display_settings->setNumMultiSamples(multi_samples); // numsamples 2, 4 or 8
	console.addMessage("Anti-aliasing: multisamples: " + StringToken::lexical_cast<std::string>(multi_samples));

	/*	tmp transform here */
	osg::PositionAttitudeTransform* transform = new osg::PositionAttitudeTransform();
	transform->addChild( pin_screen_handler.getPinScreen().getNode() );
	

	bool shadows_on = false;
	if (! shadows_on)
	{
		group->addChild(transform);
	}
	else
	{
		// Shadows 
		/*
		osg::ref_ptr<osg::MatrixTransform> light_transform = createLight();

		light_transform->setMatrix(
			osg::Matrix::rotate(osg::inDegrees(90.0f), 1.0f, 0.0f, 0.0f) * 
			//	osg::Matrix::rotate(osg::inDegrees(45.0f), 0.0f, 0.0f, 1.0f) * 
			osg::Matrix::translate(0.0, -100.0, 0.0f)
			);

		osg::ref_ptr<osg::Group> shadowedScene = createShadowedScene(transform, light_transform.get(), 0);
		group->addChild(shadowedScene.get());
		*/
	}

	system.print("System Ok.");


	console.addMessage("Welcome to digital pin screen.");
	system.handleCommand("/run start_up.dps");
	

	viewer.setSceneData(group);

	viewer.realize();

	/* para obter o scene_view!! , n alterar as CullSettings no scene_view pk o osgcameragroup.frame() altera-as a cada frame */
	osgProducer::OsgSceneHandler  *scene_handler = viewer.getSceneHandlerList()[0].get();
	osgUtil::SceneView *scene_view = scene_handler->getSceneView();

	/* Disable ALL OsgCulling */
	osg::CullStack::CullingMode culling_mode = viewer.getCullSettings().getCullingMode();
	//culling_mode = osg::CullStack::VIEW_FRUSTUM_CULLING;
	viewer.getCullSettings().setCullingMode(culling_mode);

	osg::Timer_t frame_start_tick = osg::Timer::instance()->tick();
	osg::Timer_t frame_end_tick = osg::Timer::instance()->tick();

	double total_time = 0;
	double frame_time = 0;

	unsigned int i = 0;

	while (!viewer.done())	
	{
		frame_time = osg::Timer::instance()->delta_s(frame_start_tick, frame_end_tick);
		total_time += frame_time;
		frame_start_tick = osg::Timer::instance()->tick();

		viewer.sync();

		if (record_avi)
			viewer.getFrameStamp()->setReferenceTime(total_time);

		viewer.update();
		viewer.frame();

		frame_end_tick = osg::Timer::instance()->tick();

		if (record_avi)
		{
			std::stringstream string_stream;
			string_stream << "dump\\dump_" << i << ".bmp";

			osg::ref_ptr<osg::Image> image = new osg::Image;
			image->readPixels(0, 0, screen_width, screen_height, GL_RGB,GL_UNSIGNED_BYTE);
			osgDB::writeImageFile(*image, string_stream.str());
		}

		++i;
	}

	viewer.sync();
}
