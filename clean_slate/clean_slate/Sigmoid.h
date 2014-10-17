#ifndef SIGMOID_H
#define SIGMOID_H
#include <math.h> 

double sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}
double sigmoid_d(double x)
{
	double val = sigmoid(x);

	return val *(1 - val);
}
#endif