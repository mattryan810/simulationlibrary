#include "../simrequired.h"

set_members(Particle,
		float x; 
		float y; 
		float vx; 
		float vy; 
		float charge;
		float mass;
		Particle** connected_to; 
		unsigned int elements;
		)

#define I_DEFINED_PARTICLES
#define K_CONST 1.
#define SPRING_CONST 1.
#define DELTA_T (1./30.)


#include "../simfunctions.h"
#include <math.h>
#include <time.h>


void connect(Particle* p1, Particle* p2) {

	size_t ptr_size = sizeof(Particle*);


	p1->connected_to = realloc(p1->connected_to, ptr_size * ++p1->elements);
	p2->connected_to = realloc(p2->connected_to, ptr_size * ++p2->elements);

	p1->connected_to[p1->elements - 1] = p2;
	p2->connected_to[p2->elements - 1] = p1;
	// fprintf(stderr,"connected %p to %p\n", (void*)p1, (void*)p2);

}

int are_connected(Particle** particles)
{
	Particle *p1, *p2;

	// p1 has fewer elements, so check if p2 is in those
	if (particles[0]->elements <= particles[1]->elements)
	{
		p1 = particles[0];
		p2 = particles[1];
	} 
	else {

		p1 = particles[1];
		p2 = particles[0];
	}

	for (int i = 0; i < p1->elements; i++)
		if (p1->connected_to[i] == p2)
			return 1;

	return 0;
}

void clear_particle(Particle* p)
{
	p->elements = p->x = p->y = p->vx = p->vy = p->mass = p->charge = 0;
	p->connected_to = malloc(p->elements * sizeof(Particle));
}

void coulombs_law (Particle** particles)
{
	Particle *p1 = particles[0],
			 *p2 = particles[1];
	
	if (p1->charge == 0 || p2->charge == 0 || p1->mass == 0 || p2->mass == 0)
	{
		fprintf(stderr,"error: required value omitted\n");
		exit(-1);
	}
	
	double r = sqrt(pow(p2->x - p1->x,2) + pow(p2->y - p1->y,2));
	double rhat[2] = {(p2->x - p1->x) / r, (p2->y - p1->y) / r };


	// F = k |q1 q2| / r^2 rhat= m a => a = k |q1 q2| rhat / mr^2
	double F = -K_CONST * p1->charge * p2->charge / pow(r,2);
	

	p1->vx += F * rhat[0] / p1->mass;
	p1->vy += F * rhat[1] / p1->mass;

	p2->vx += F * -rhat[0] / p2->mass;
	p2->vy += F * -rhat[1]  / p2->mass;

}

void spring_function (Particle** particles)
{


	Particle *p1 = particles[0],
			 *p2 = particles[1];

	double r = sqrt(pow(p2->x - p1->x,2) + pow(p2->y - p1->y,2));
	double rhat[2] = {(p2->x - p1->x) / r, (p2->y - p1->y) / r };

	// F = -kx = ma => a = -kx/m rhat
	double F = SPRING_CONST * r;

	p1->vx += F * rhat[0] / p1->mass;
	p1->vy += F * rhat[1] / p1->mass;

	p2->vx += F * -rhat[0] / p2->mass;
	p2->vy += F * -rhat[1] / p2->mass;

}

void inertia_function (Particle *p)
{
	p->x += p->vx * DELTA_T;
	p->y += p->vy * DELTA_T;
}

void damping_function (Particle *p)
{
	const double damping_const = 0.7;
	p->vx *= damping_const;
	p->vy *= damping_const;
}

double calc_max_v (ParticleVector *pv)
{
	double maxv = 0, currentv;
	for (int i = 0; i < pv->elements; i++)
	{
		currentv = sqrt(pow(pv->arr[i].vx,2) + pow(pv->arr[i].vy,2));
		if (currentv > maxv)
			maxv = currentv;
	}
	return maxv;
}

void print_x(Particle *part)
{
	printf("%f",part->x);
}

void print_y(Particle *part)
{
	printf("%f",part->y);
}

int random_int(int max){
	return (int)((double)rand() * max / RAND_MAX);
}

Particle random_particle(double min_max_pos)
{
	Particle p;
	clear_particle(&p);

	p.x = (double)rand() * min_max_pos / RAND_MAX;
	p.y = (double)rand() * min_max_pos / RAND_MAX;

	p.charge = p.mass = 1;

	return p;
}

void print_connection_matrix (ParticleVector *pv)
{
	int matrix[pv->elements][pv->elements];

	for (int i = 0; i < pv->elements; i++)
		for (int j = i; j < pv->elements; j++)
		{
			matrix[i][j] = matrix[j][i] = 0;
			for (int k = 0; k < pv->arr[i].elements; k++)
				if (pv->arr[i].connected_to[k] == &pv->arr[j])
				{
					matrix[i][j] = matrix[j][i] = 1;
					break;
				}
		}



	fprintf(stderr,"{\n");
	for (int i = 0; i < pv->elements; i++)
	{
		if (i != 0)
			fprintf(stderr,",\n");
		fprintf(stderr,"{");
		for (int j = 0; j < pv->elements; j++)
		{
			if (j != 0)
				fprintf(stderr,", ");
			fprintf(stderr,"%d", matrix[i][j]);
		}
		fprintf(stderr,"}");
	}
	fprintf(stderr,"}\n");


}

int main (void)
{
	srand(time(NULL));

	ParticleVector pv = init_ParticleVector();
	ForceVector fv = init_ForceVector();
	PassiveVector pasv = init_PassiveVector();
	MetricVector mv = init_MetricVector();

	Passive inertia;
	inertia.apply = inertia_function;
	push_Passive(&pasv, &inertia);

	Passive damping;
	damping.apply = damping_function;
	push_Passive(&pasv, &damping);

	Force charge;
	charge.acts_on = 2;
	charge.test_function = always_true;
	charge.apply_function = coulombs_law;
	push_Force(&fv, &charge);


	Force spring;
	spring.acts_on = 2;
	spring.test_function = are_connected;
	spring.apply_function = spring_function;
	push_Force(&fv, &spring);


	// Just populates random particles and connections to demonstrate
	int num_of_particles = 100;
	Particle p;
	for (int i = 0; i < num_of_particles; i++)
	{
		p = random_particle(10);
		push_Particle(&pv,&p);
	}

	double magic;
	for (int i = 0; i < num_of_particles; i++)
	{
		magic = random_int(num_of_particles);
		for (int k = 0; k < num_of_particles; k++)
		{
			if ((k == i-1 || k == magic) && magic != i)
				connect(&pv.arr[i],&pv.arr[k]);
		}

	}


	Metric m;
	m.print = print_x;
	push_Metric(&mv, &m);
	m.print = print_y;
	push_Metric(&mv, &m);

	fprintf(stderr, "there were %d particles generated.\n", num_of_particles);
	
	print_connection_matrix(&pv);

	double maxv = 0;
	for (int t = 0; (maxv = calc_max_v(&pv)) > 0.015 || t < 19; t++)
	{
		maxv = calc_max_v(&pv);

		print_metrics(&mv, &pv);
		apply_forces(&fv, &pv);
		apply_passives(&pasv, &pv);

	}

	return 0;
}
