#ifdef I_DEFINED_PARTICLES

#define size_to_console(TYPE) fprintf(stderr,"size of %s = %lu\n", #TYPE, sizeof(TYPE))

int always_true(Particle **acts_on)
{
	return 1;
}

// Should add something to check the print function for each metric actually does something. with a non-specified function i get a bus error
// Perhaps also output formatting?
void print_metrics  (MetricVector *mv, ParticleVector *pv)
{
	Metric* current_metric;
	Particle* target_particle;

	// for each particle
	for (int p = 0; p < pv->elements; p++)
	{
		target_particle = &(pv->arr[p]);

		if (p != 0)
			printf(", ");

		for (int m = 0; m < mv->elements; m++)
		{
			if (m != 0)
				printf(", ");

			current_metric = &mv->arr[m];
			current_metric->print(target_particle);
		}
	}
	printf("\n");
}

void apply_forces   (ForceVector *fv, ParticleVector *pv)
{
	Force *current_force;
	Particle *target_particles[2];
	for (int f = 0; f < fv->elements; f++)
	{
		int p1,p2;
		current_force = &fv->arr[f];
		switch (current_force->acts_on)
		{
			case 1:
				for (p1 = 0; p1 < pv->elements; p1++)
				{
					target_particles[0] = &pv->arr[p1];
					if (current_force->test_function(target_particles))
						current_force->apply_function(target_particles);
				}
				break;
			case 2:
				for (p1 = 0; p1 < pv->elements - 1; p1++)
				{
					target_particles[0] = &pv->arr[p1];
					for (p2 = p1 + 1; p2 < pv->elements; p2++)
					{
						target_particles[1] = &pv->arr[p2];
						if (current_force->test_function(target_particles))
							current_force->apply_function(target_particles);
					}
				}
				break;
			default:
				printf("error: attempted to use unsupported force interaction amount\n");
				exit(-1);
				break;

		}
	}
}

void apply_passives (PassiveVector *pasv, ParticleVector *pv)
{
	for (int p = 0; p < pv->elements; p++)
		for (int pas = 0; pas < pasv->elements; pas++)
			pasv->arr[pas].apply(&pv->arr[p]);
}

#else
#error Before including simfunctions.h, make sure to call set_members(Particle, MEMBER_LIST) and confirm with "#define I_DEFINED_PARTICLES"
#endif
