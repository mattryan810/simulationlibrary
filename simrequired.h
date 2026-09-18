#include <stdio.h>
#include <stdlib.h>



#define set_members(TYPENAME, MEMBERLIST)                          \
                                                                   \
typedef struct TYPENAME {                                          \
	MEMBERLIST                                                     \
} TYPENAME;                                                        \
                                                                   \
typedef struct TYPENAME ## Vector {                                \
	TYPENAME *arr;                                                 \
	size_t element_size;                                           \
	unsigned elements;                                             \
} TYPENAME ## Vector;                                              \
                                                                   \
TYPENAME ## Vector init_ ## TYPENAME ## Vector (void)              \
{                                                                  \
	TYPENAME ## Vector temp;                                       \
	                                                               \
	temp.element_size = sizeof(TYPENAME);                          \
	temp.elements = 0;                                             \
	temp.arr = malloc(temp.elements * temp.element_size);          \
                                                                   \
	return temp;                                                   \
}                                                                  \
                                                                   \
int push_ ## TYPENAME (TYPENAME ## Vector *v, TYPENAME *T)         \
{                                                                  \
	v->arr = realloc(v->arr, ++v->elements * v->element_size);     \
	(v->arr)[v->elements - 1] = *T;                                \
                                                                   \
	return v->elements - 1;                                        \
}                                                                  \

typedef struct Particle Particle;
typedef struct ParticleVector ParticleVector;

// acts_on will always be a force pair, where the second can be a NULL ptr
set_members(Force, 
		int (*test_function) (Particle **acts_on);
		void (*apply_function) (Particle **acts_on);
		int acts_on;
		)


set_members(Passive,
		void (*apply)(Particle *part);
		)

set_members(Metric, 
		void (*print)(Particle *part);
		)
