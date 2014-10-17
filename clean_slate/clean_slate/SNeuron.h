#ifndef SNeuron_H_
#define SNeuron_H_

#include <vector>

class SNeuron
{
private:
	int m_NumInputs;
	std::vector<double> m_vecWeight;

public:
	SNeuron(int NumInputs);


};

#endif