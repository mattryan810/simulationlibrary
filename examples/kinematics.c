#include "../simrequired.h"

set_members(Particle, float x; float y; float vx; float vy;)
#define I_DEFINED_PARTICLES

#include "../simfunctions.h"

#define DELTA_T 1./30.

void gravity(Particle **acts_on)
{
	Particle *p = acts_on[0];
	p->vy += -9.81 * DELTA_T;
}

void inertia_function(Particle *part)
{
	part->x += part->vx * DELTA_T;
	part->y += part->vy * DELTA_T;
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
	
	ParticleVector pv = init_ParticleVector();
	ForceVector fv = init_ForceVector();
	PassiveVector pasv = init_PassiveVector();
	MetricVector mv = init_MetricVector();

	Particle p;
	Force f_g;
	Passive inertia;
	Metric position;

	// Populate ParticleVector
	p.x = p.y = p.vx = p.vy = 0.00000;
	push_Particle(&pv, &p);
	p.vx = p.vy = 2.00000;
	push_Particle(&pv,&p);

	// Populate ForceVector
	f_g.apply_function = gravity;
	f_g.test_function = always_true;
	f_g.acts_on = 1;
	push_Force(&fv, &f_g);

	// Populate Passives
	inertia.apply = inertia_function;
	push_Passive(&pasv, &inertia);

	// Populate Metrics
	position.print = print_x;
	push_Metric(&mv, &position);
	position.print = print_y;
	push_Metric(&mv, &position);


	// for each time step
	for (int t = 0; t < 100; t++ )
	{
		

		print_metrics(&mv, &pv);
		apply_forces(&fv, &pv);
		apply_passives(&pasv, &pv);


	}

	return 0;
}

