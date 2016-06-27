#ifndef __ANIMATIOR__
#define __ANIMATIOR__

#include <osg/Group>

#include "PinScreen.h"
#include "Drawable.h"

class Animator : public osg::NodeCallback, public DrawMode
{
public:
	Animator();
	virtual ~Animator();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* node_visitor) = 0;
};

class LinearMover : public Animator
{
public:
	LinearMover();
	virtual ~LinearMover();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* node_visitor);
	virtual void movePin(int const x, int const y, float value);
	void setDuration(float const duration);
	void buildMover();
private:
	struct Pin
	{
		float h_position;
		float v_position;
		float current_position;
		float move_delta;
		float moving_position;
		bool move;
	};

	enum State
	{
		to_register,
		idle,
		to_start,
		moving
	};

	float duration;
	double start_time;
	State state;

	Pin *pins;
};

class ElasticMover : public Animator
{
public:
	ElasticMover();
	virtual ~ElasticMover();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* node_visitor);
	virtual void movePin(int const x, int const y, float value);

	void setMass(float const mass);
	void setStiffness(float const stiffness);
	void setDamping(float const damping);
	void setTimeIncrement(float const time_increment);

	float getMass() const;
	float getStiffness() const;
	float getDamping() const;
	float getTimeIncrement() const;

	void buildMover();

private:
	enum AnimationState
	{
		to_register,
		idle,
		to_start,
		moving
	};

	AnimationState animation_state;
	float animation_time;
	float animation_time_accumulator;

	/* Physics */
	float mass;
	float damping;
	float stiffness;

	/* Runge Kutta Order 4 Integrator (RK4) (check Graffer On Games)*/
	struct State
	{
		float x; //current position
		float v; //current velocity

		// destination position
		float destination;
	};

	struct Derivative
	{
		float dx;
		float dv;
	};
	float acceleration(State const &state, float t);
	Derivative evaluate(State const &initial, float t);
	Derivative evaluate(State const &initial, float t, float dt, Derivative const &d);
	void integrate(State &state, float t, float dt);

	float time; // physics time
	float time_increment; // physics time

	struct Pin
	{
		float h_position;
		float v_position;
		State current_state;
		//State last_state;
	};

	Pin *pins;
};


#include "Animator_impl.h"

#endif // __ANIMATIOR__