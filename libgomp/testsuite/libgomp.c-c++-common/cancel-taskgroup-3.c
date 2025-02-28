/* { dg-do run } */
/* { dg-set-target-env-var OMP_CANCELLATION "true" } */

#include <stdlib.h>
#include <omp.h>

int
main ()
{
  int a = 0, i;
  #pragma omp parallel
  #pragma omp taskgroup
  {
    #pragma omp task
    {
      #pragma omp cancel taskgroup
      if (omp_get_cancellation ())
	abort ();
    }
    #pragma omp taskwait
    #pragma omp for reduction (task, +: a)
    for (i = 0; i < 64; ++i)
      {
	a++;
	#pragma omp task in_reduction (+: a)
	{
	  volatile int zero = 0;
	  a += zero;
	  if (omp_get_cancellation ())
	    abort ();
	}
      }
    if (a != 64)
      abort ();
    #pragma omp task
    {
      if (omp_get_cancellation ())
	abort ();
    }
  }
  a = 0;
  #pragma omp parallel
  #pragma omp taskgroup
  {
    #pragma omp taskwait
    #pragma omp for reduction (task, +: a)
    for (i = 0; i < 64; ++i)
      {
	a++;
	#pragma omp task in_reduction (+: a)
	{
	  volatile int zero = 0;
	  a += zero;
	  #pragma omp cancel taskgroup
	  if (omp_get_cancellation ())
	    abort ();
	}
      }
    if (a != 64)
      abort ();
    #pragma omp task
    {
      if (omp_get_cancellation ())
	abort ();
    }
  }
  return 0;
}
