#pragma once
#include "ccontcontroller.h"
#include <vector>
#include "CNeuralNet.h"
#include "CParams.h"
#include "CBasicEA.h"

class CEAController :
	public CContController
{
protected:

public:
	CEAController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	virtual bool Update(void);
	virtual ~CEAController(void);
	uint inputs = 2;
	uint hidden = 20;
	uint out = 2;
	double learning_rate = 0.075;
	double mse_cutoff = 0.1;
	std::vector<CNeuralNet*> nets;

};

