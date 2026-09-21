#include "../simrequired.h"
#include <math.h>

#define GRAV_CONST 5
#define DELTA_T 1./30.
 
set_members(Particle, double mass; double x; double y; double vx; double vy;)
#define I_DEFINED_PARTICLES

#include "../simfunctions.h"

void inertia_function (Particle *p)
{
	p->x += p->vx * DELTA_T;
	p->y += p->vy * DELTA_T;
}

void two_body_gravity (Particle **particles)
{
	Particle *p1 = particles[0],
			 *p2 = particles[1];
	
	double r = sqrt(pow(p2->x - p1->x,2) + pow(p2->y - p1->y,2));
	double rhat[2] = {(p2->x - p1->x) / r, (p2->y - p1->y) / r };

	p1->vx += (GRAV_CONST * p2->mass * DELTA_T) / pow(r,2) * rhat[0];
	p1->vy += (GRAV_CONST * p2->mass * DELTA_T) / pow(r,2) * rhat[1];

	p2->vx += -(GRAV_CONST * p1->mass * DELTA_T) / pow(r,2) * rhat[0];
	p2->vy += -(GRAV_CONST * p1->mass * DELTA_T) / pow(r,2) * rhat[1];

}

void print_x(Particle *part)
{
	printf("%f",part->x);
}

void print_y(Particle *part)
{
	printf("%f",part->y);
}


int main (void)
{

	MetricVector mv = init_MetricVector();
	ParticleVector pv = init_ParticleVector();
	PassiveVector pasv = init_PassiveVector();
	ForceVector fv = init_ForceVector();

	// Set up particles
	Particle p;

	p.mass = 5;
	p.vx = 0;
	p.vy = 0;
	p.x = 0;
	p.y = 0;
	push_Particle(&pv, &p);

	p.mass = 1;
	p.vx = -1;
	p.vy = 0;
	p.x = 0;
	p.y = 5;
	push_Particle(&pv, &p);

	p.mass = 1;
	p.vx = 1;
	p.vy = 0;
	p.x = 0;
	p.y = -5;
	push_Particle(&pv, &p);


	// Set up inertia
	Passive pas;
	pas.apply = inertia_function;
	push_Passive(&pasv, &pas);
	
	// Set up gravity
	Force f;
	f.acts_on = 2;
	f.test_function = always_true;
	f.apply_function = two_body_gravity;
	push_Force(&fv, &f);

	// Set up metrics
	Metric m;
	m.print = print_x;
	push_Metric(&mv, &m);
	m.print = print_y;
	push_Metric(&mv, &m);


	size_to_console(Particle);
	size_to_console(double);

	for (int t = 0; t < 1100; t++)
	{
		print_metrics(&mv, &pv);
		apply_forces(&fv, &pv);
		apply_passives(&pasv, &pv);

	}


}
