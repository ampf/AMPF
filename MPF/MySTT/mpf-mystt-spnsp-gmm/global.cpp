#include "global.h"

void Initialize1DFloat(float *array, int dim)
{
  for(int i=0; i<dim; i++)
  {
	array[i] = 0.0;
  }
}

void Initialize1DInt(int *array, int dim)
{
  for(int i=0; i<dim; i++)
  {
	array[i] = 0;
  }
}

void Initialize2DFloat(float **array, int dim1, int dim2)
{
  for(int i=0; i<dim1; i++)
  {
	for(int j=0; j<dim2; j++)
	{
	  array[i][j] = 0.0;
	}
  }
}
void Initialize1DDouble(double *array, int dim)
{
  for(int i=0; i<dim; i++)
  {
	array[i] = 0.0;
  }
}

void Initialize2DDouble(double **array, int dim1, int dim2)
{
  for(int i=0; i<dim1; i++)
  {
	for(int j=0; j<dim2; j++)
	{
	  array[i][j] = 0.0;
	}
  }
}
