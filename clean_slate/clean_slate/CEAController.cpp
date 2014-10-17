/**
    )                                                         
 ( /(                                (         )              
 )\())  (   (  (         (    )      )\  (  ( /((             
((_)\  ))\ ))\ )(   (    )\  /((  ( ((_)))\ )\())\  (   (     
 _((_)/((_)((_|()\  )\  ((_)(_))\ )\ _ /((_|_))((_) )\  )\ )  
| \| (_))(_))( ((_)((_) | __|)((_|(_) (_))(| |_ (_)((_)_(_/(  
| .` / -_) || | '_/ _ \ | _|\ V / _ \ | || |  _|| / _ \ ' \)) 
|_|\_\___|\_,_|_| \___/ |___|\_/\___/_|\_,_|\__||_\___/_||_|  
                                              
	   (                )        (  (           
	   )\            ( /((       )\ )\  (  (    
	 (((_)  (   (    )\())(   ( ((_|(_)))\ )(   
	 )\___  )\  )\ )(_))(()\  )\ _  _ /((_|()\  
	((/ __|((_)_(_/(| |_ ((_)((_) || (_))  ((_) 
	 | (__/ _ \ ' \))  _| '_/ _ \ || / -_)| '_| 
	  \___\___/_||_| \__|_| \___/_||_\___||_|   
                                            
*/
#include "CEAController.h"


CEAController::CEAController(HWND hwndMain):
	CContController(hwndMain)
{
	
}

void CEAController::InitializeLearningAlgorithm(void)
{
	//TODO: set up any data structures you need to store the genomes here
	CContController::InitializeLearningAlgorithm(); 

	CNeuralNet *net;
	for (int i = 0; i < m_vecSweepers.size(); ++i)
	{
		net = new CNeuralNet(inputs, hidden, out, learning_rate, mse_cutoff);
		nets.push_back(net);
	}

}

/**
Returns the dot product between the sweeper's look vector and the vector from the sweeper to the object
*/
inline double dot_between_vlook_and_vObject(const CContMinesweeper &s,const CContCollisionObject &o){
	SVector2D<double> vLook = s.getLookAt();
	SVector2D<double> pt = o.getPosition();
		//get the vector to the point from the sweepers current position:
		SVector2D<double> vObj(SVector2D<double>(pt.x,pt.y) - s.Position());
		Vec2DNormalize<double>(vObj);
		//remember (MAM1000 / CSC3020) the dot product between two normalized vectors returns
		//1 if the two vectors point in the same direction
		//0 if the two vectors are perpendicular
		//-1 if the two vectors are pointing in opposite directions
		return Vec2DDot<double>(vLook,vObj);
}

bool CEAController::Update(void)
{
	uint cDead = std::count_if(m_vecSweepers.begin(),
							   m_vecSweepers.end(),
						       [](CContMinesweeper * s)->bool{
								return s->isDead();
							   });
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	if (m_iTicks+1 >= CParams::iNumTicks){
		//TODO: when the round is over select, cross over and mutate here
		
		
			std::vector<ParentAndFitness>  fitness_out;
			CBasicEA::computeFitness(m_vecSweepers, nets, fitness_out);//fitness
			CBasicEA::selection_crossover_mutate(fitness_out);
			nets.clear();
			for (int i = 0; i < fitness_out.size(); i++)
			{
				nets.push_back(fitness_out[i].parent_genome);//new generation
			}

	}
	CContController::Update(); 
	for (uint i = 0; i < CParams::iNumSweepers; ++i){
		CContMinesweeper * s = m_vecSweepers[i]; 

		double rock = (s)->getClosestRock() < 0 ? -1 : dot_between_vlook_and_vObject(*s, *m_vecObjects[(s)->getClosestRock()]);
		double mine = (s)->getClosestMine() < 0 ? -1 : dot_between_vlook_and_vObject(*s, *m_vecObjects[(s)->getClosestMine()]);
		double supermine = (s)->getClosestSupermine() < 0 ? -1 : dot_between_vlook_and_vObject(*s, *m_vecObjects[(s)->getClosestSupermine()]);
		double rockDist = (s)->getClosestRock() < 0 ? 1000000000 : Vec2DLength(m_vecObjects[(s)->getClosestRock()]->getPosition() - (s)->Position());
		double supermineDist = (s)->getClosestSupermine() < 0 ? 1000000000 : Vec2DLength(m_vecObjects[(s)->getClosestSupermine()]->getPosition() - (s)->Position());
		int dist = 60;
	
		double input[2] = 
		{
			mine, 
			(rockDist < dist || supermineDist < dist) ? ((rockDist < supermineDist) ? rock : supermine) : -1
		};
		
		if (nets[i]->classify(input) == 0)
		{ // towards the mine
			SPoint pt(m_vecObjects[(s)->getClosestMine()]->getPosition().x,m_vecObjects[(s)->getClosestMine()]->getPosition().y);
			(s)->turn(pt, 1);
		}
		else 
		{//turn away
			if (rockDist < supermineDist)
			{
				SPoint pt(m_vecObjects[(s)->getClosestRock()]->getPosition().x,m_vecObjects[(s)->getClosestRock()]->getPosition().y);
				(s)->turn(pt, 1, false);
			}
			else 
			{
				SPoint pt(m_vecObjects[(s)->getClosestSupermine()]->getPosition().x,m_vecObjects[(s)->getClosestSupermine()]->getPosition().y);
				(s)->turn(pt, 1, false);
			}
		}

	}

	return true; 
}

CEAController::~CEAController(void)
{
	//TODO:: release any allocations here
}
