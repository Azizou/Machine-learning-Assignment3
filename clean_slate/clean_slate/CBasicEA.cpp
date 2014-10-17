/**
    )                                                         
 ( /(                                (         )              
 )\())  (   (  (         (    )      )\  (  ( /((             
((_)\  ))\ ))\ )(   (    )\  /((  ( ((_)))\ )\())\  (   (     
 _((_)/((_)((_|()\  )\  ((_)(_))\ )\ _ /((_|_))((_) )\  )\ )  
| \| (_))(_))( ((_)((_) | __|)((_|(_) (_))(| |_ (_)((_)_(_/(  
| .` / -_) || | '_/ _ \ | _|\ V / _ \ | || |  _|| / _ \ ' \)) 
|_|\_\___|\_,_|_| \___/ |___|\_/\___/_|\_,_|\__||_\___/_||_|  

Further details can be found in:
X. Yao, Evolving artificial neural networks, Proceedings of the IEEE,
87(9):1423-1447, September 1999                                                              

This class only serves to give you some direction as where to go. You
can modify the headers, etc. if you want
*/

#include "CBasicEA.h"
#include "utils.h"
/**
This function should compute the fitness of a particular sweeper and bundle its genome and fitness together
*/
void CBasicEA::computeFitness(const std::vector<CContMinesweeper *> & parent_sweepers,
							  const std::vector<CNeuralNet *> & parent_genomes,
							  std::vector<ParentAndFitness> & fitness_out)
{
	//TODO:: roll your own

	if (parent_sweepers.size() == 0)
		return;
	CContMinesweeper *sweeper;
	CNeuralNet *net;
	double fitness;
	double death = -200;
	double mine = 1;
	for (int i = 0; i<parent_genomes.size(); i++)
	{
		
		sweeper = parent_sweepers[i];
		net = parent_genomes[i];

		fitness = (sweeper->MinesGathered())*mine;
		
		if (sweeper->isDead())
		{
			fitness += death;
		}
		ParentAndFitness pnf;
		pnf.fitness = fitness;
		pnf.parent_genome = net;
		fitness_out.push_back(pnf);


	}
}
/**
Selects the fittest of the population and use selection operators on them to spawn replacements for the rest
*/
void CBasicEA::selection_crossover_mutate(std::vector<ParentAndFitness> & parents)
{//find fitest parents and make next generation
	if (parents.size() == 0)
		return;
	int size = parents.size();
	CNeuralNet *genom1= parents[0].parent_genome;
	double fitness = parents[0].fitness;
	int index = 0;
	for (int i = 0; i < parents.size(); i++)
	{
		if (parents[i].fitness>fitness)
		{
			genom1 = parents[i].parent_genome;
			index = i;
			fitness = parents[i].fitness;
		}

	}
	CNeuralNet *genom2;
	int index1;
	if (index != 0)
	{

		genom2 = parents[0].parent_genome;
		fitness = parents[0].fitness;
		
	}
	else
	{
		genom2 = parents[1].parent_genome;
		fitness = parents[1].fitness;
		
	}
	
	for (int i = 0; i < parents.size(); i++)
	{
		if (i == index)
			continue;
		if (parents[i].fitness>fitness)
		{
			genom2 = parents[i].parent_genome;
			
			fitness = parents[i].fitness;
		}

	}
//	parents.clear();
	

	//parents.push_back(*parent1);
	//parents.push_back(*parent2);
	for (int i = 0; i < size; i++)
	{
		CNeuralNet *child = new CNeuralNet(genom1->inputLayerSize, genom1->hiddenLayerSize, genom1->outputLayerSize, genom1->lRate, genom1->mse_cutoff);
		crossover(*genom1, *genom2, *child);
		mutate(*child, *child);
		parents[i].fitness = 0;
		parents[i].parent_genome = child;
	}
	

}
/**
Crossover (exploration) of some sort
*/
void CBasicEA::crossover(const CNeuralNet & genotypeA,const CNeuralNet & genotypeB, CNeuralNet & out)
{
	//TODO:: roll your own

	int allWeights = (genotypeA.hiddenLayerSize*genotypeA.inputLayerSize + genotypeA.outputLayerSize* genotypeA.hiddenLayerSize);
	int COP =  (rand() % (int)(allWeights));//random point to start crossover

	int count = 0;
	//input layer weights
	for (int i = 0; i < genotypeA.inputLayerSize; i++)
	{

		for (int o = 0; o < genotypeA.hiddenLayerSize; o++)
		{
			if (count < COP)
			{
				out.hiddenWeights[i][o] = genotypeA.hiddenWeights[i][o];
				
			}
			else
			if (count > COP)
			{
				out.hiddenWeights[i][o] = genotypeB.hiddenWeights[i][o];
			}
			count++;
		}
	}
	//hidden layer weights
	for (int i = 0; i < genotypeA.hiddenLayerSize; i++)
	{
		
		for (int o = 0; o < genotypeA.outputLayerSize; o++)
		{
			if (count<COP)
			{
				out.outputWeights[i][o] = genotypeA.outputWeights[i][o];
				
			}
			else
			if (count > COP)
			{
				out.outputWeights[i][o] = genotypeB.outputWeights[i][o];
			}
			count++;
			
		}
	}
		
	

}
/**
Mutation (exploitation) of some sort
*/ 
void CBasicEA::mutate(const CNeuralNet & genotypeA, CNeuralNet & out)
{
	out = genotypeA;
	//adjust each weight by randomly by 1 or -1
	for (int i = 0; i < genotypeA.inputLayerSize; i++)
	{

		for (int o = 0; o < genotypeA.hiddenLayerSize; o++)
		{
			
			out.hiddenWeights[i][o] += (RandomClamped() * out.mse_cutoff);
			
		}
	}
	for (int i = 0; i < genotypeA.hiddenLayerSize; i++)
	{

		for (int o = 0; o < genotypeA.outputLayerSize; o++)
		{
			
			out.outputWeights[i][o] += (RandomClamped() * out.mse_cutoff);
			
		}
	}


	//TODO:: roll your own
}

