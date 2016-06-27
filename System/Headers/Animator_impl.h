inline Animator::Animator()
{
}
inline Animator::~Animator()
{
}
/*
* LinearMover
*/
inline LinearMover::LinearMover()
:duration(0.0f), state(to_register), pins(0)
{
}
inline LinearMover::~LinearMover()
{
	if (pins)
		delete[] pins;
}
inline void LinearMover::setDuration(float const duration)
{
	this->duration = duration;
}
inline void LinearMover::movePin(int const x, int const y, float value)
{
	//value = destination_position

	pins[y * screen->getWidth() + x].current_position += pins[y * screen->getWidth() + x].moving_position;	
	pins[y * screen->getWidth() + x].move_delta = value - pins[y * screen->getWidth() + x].current_position;
	pins[y * screen->getWidth() + x].move = true;

	state = (state > to_register) ? to_start : to_register;
}
/*
* ElasticMover
*/
inline ElasticMover::ElasticMover()
:animation_state(to_register), mass(1.0f), damping(1.0f), stiffness(10.0f), time(0.0f), time_increment(0.01f), pins(0)
{
//	animation_state = to_start;
}
inline ElasticMover::~ElasticMover()
{
	if (pins)
		delete[] pins;
}
inline float ElasticMover::acceleration(State const &state, float t)
{
	return -( stiffness / mass ) * ( - state.destination + state.x ) - ( damping / mass ) * state.v;
}
inline void ElasticMover::setMass(float const mass)
{
	this->mass = mass;
}
inline void ElasticMover::setStiffness(float const stiffness)
{
	this->stiffness = stiffness;
}
inline void ElasticMover::setDamping(float const damping)
{
	this->damping = damping;
}
inline void ElasticMover::setTimeIncrement(float const time_increment)
{
	this->time_increment = time_increment;
}
inline float ElasticMover::getMass() const
{
	return mass;
}
inline float ElasticMover::getStiffness() const
{
	return stiffness;
}
inline float ElasticMover::getDamping() const
{
	return damping;
}
inline float ElasticMover::getTimeIncrement() const
{
	return time_increment;
}
inline void ElasticMover::movePin(int const x, int const y, float value)
{
	pins[y * screen->getWidth() + x].current_state.destination = value;
	animation_state = (animation_state > to_register) ? to_start : to_register;
}
inline ElasticMover::Derivative ElasticMover::evaluate(State const &initial, float t)
{
	Derivative output;
	output.dx = initial.v;
	output.dv = acceleration(initial, t);
	return output;
}
inline ElasticMover::Derivative ElasticMover::evaluate(State const &initial, float t, float dt, Derivative  const &d)
{
	State state;
	state.x = initial.x + d.dx * dt;
	state.v = initial.v + d.dv * dt;

	state.destination = initial.destination;

	Derivative output;
	output.dx = state.v;
	output.dv = acceleration(state, t+dt);
	return output;
}
inline void ElasticMover::integrate(State &state, float t, float dt)
{
	Derivative a = evaluate(state, t);
	Derivative b = evaluate(state, t, dt * 0.5f, a);
	Derivative c = evaluate(state, t, dt * 0.5f, b);
	Derivative d = evaluate(state, t, dt, c);

	state.x = state.x + ( 1.0f / 6.0f * (a.dx + 2.0f * (b.dx + c.dx) + d.dx) ) * dt;
	state.v = state.v + ( 1.0f / 6.0f * (a.dv + 2.0f * (b.dv + c.dv) + d.dv) ) * dt;
}