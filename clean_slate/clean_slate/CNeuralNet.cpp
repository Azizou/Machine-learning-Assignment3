/*
                                                                           
   (               )                                        )              
 ( )\     )     ( /(       (                  (  (     ) ( /((             
 )((_) ( /(  (  )\())`  )  )(   (  `  )   (   )\))( ( /( )\())\  (   (     
((_)_  )(_)) )\((_)\ /(/( (()\  )\ /(/(   )\ ((_))\ )(_)|_))((_) )\  )\ )  
 | _ )((_)_ ((_) |(_|(_)_\ ((_)((_|(_)_\ ((_) (()(_|(_)_| |_ (_)((_)_(_/(  
 | _ \/ _` / _|| / /| '_ \) '_/ _ \ '_ \/ _` |/ _` |/ _` |  _|| / _ \ ' \)) 
 |___/\__,_\__||_\_\| .__/|_| \___/ .__/\__,_|\__, |\__,_|\__||_\___/_||_|  
                    |_|           |_|         |___/                         

 For more information on back-propagation refer to:
 Chapter 18 of Russel and Norvig (2010).
 Artificial Intelligence - A Modern Approach.
 */

#include "CNeuralNet.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include "utils.h"
#include "sigmoid.h"
/* time *
/**
 The constructor of the neural network. This constructor will allocate memory
 for the weights of both input->hidden and hidden->output layers, as well as the input, hidden
 and output layers.
*/


CNeuralNet::CNeuralNet(uint inputLayerSize, uint hiddenLayerSize, uint outputLayerSize, double lRate, double mse_cutoff) :inputLayerSize(inputLayerSize), hiddenLayerSize(hiddenLayerSize), outputLayerSize(outputLayerSize), lRate(lRate), mse_cutoff(mse_cutoff)
{
	hiddenWeights = new double*[inputLayerSize];
	for (int i = 0; i < inputLayerSize; i++)
	{
		hiddenWeights[i] = new double[hiddenLayerSize];
	}

	outputWeights = new double*[hiddenLayerSize];
	for (int i = 0; i < hiddenLayerSize; i++)
	{
		outputWeights[i] = new double[outputLayerSize];
	}

	input = new double[inputLayerSize];
	hidden = new double[hiddenLayerSize];
	output = new double[outputLayerSize];
	
	
}
/**
 The destructor of the class. All allocated memory will be released here
*/
CNeuralNet::~CNeuralNet() 
{
	for (int i = 0; i < inputLayerSize; ++i) {
		delete[] hiddenWeights[i];
	}
	delete[] hiddenWeights;
	for (int i = 0; i < hiddenLayerSize; ++i) {
		delete[] outputWeights[i];
	}
	delete[] outputWeights;
	delete[] input;
	delete[] hidden;
	delete[] output;

}
/**
 Method to initialize the both layers of weights to random numbers
*/
void CNeuralNet::initWeights()
{ 

	
	for (int i = 0; i < inputLayerSize; i++)
	{
		
		for (int o = 0; o < hiddenLayerSize; o++)
		{
			hiddenWeights[i][o] = RandomClamped();
		}
	}

	for (int i = 0; i < hiddenLayerSize; i++)
	{
		double * ary = outputWeights[i];
		for (int o = 0; o < outputLayerSize; o++)
		{
			outputWeights[i][o] = RandomClamped();
		}
	}
}
/**
 This is the forward feeding part of back propagation.
 1. This should take the input and copy the memory (use memcpy / std::copy)
 to the allocated _input array.
 2. Compute the output of at the hidden layer nodes 
 (each _hidden layer node = sigmoid (sum( _weights_h_i * _inputs)) //assume the network is completely connected
 3. Repeat step 2, but this time compute the output at the output layer
*/
void CNeuralNet::feedForward(const double * const inputs)
{
	std::copy(inputs, inputs+inputLayerSize-1, input);
	
	for (int o = 0; o < hiddenLayerSize; o++)
	{
		double sum = 0;
	for (int i = 0; i < inputLayerSize; i++)
	{
		
		sum += hiddenWeights[i][o] * inputs[i];
		
	}
	hidden[o] = sigmoid(sum);
	}

	for (int o = 0; o < outputLayerSize; o++)
	{
	double sum = 0;
	for (int i = 0; i <hiddenLayerSize; i++)
	{
	
	sum += outputWeights[i][o] * hidden[i];
	}
	output[o] = sigmoid(sum);
	}
	
}
/**
 This is the actual back propagation part of the back propagation algorithm
 It should be executed after feeding forward. Given a vector of desired outputs
 we compute the error at the hidden and output layers (allocate some memory for this) and
 assign 'blame' for any error to all the nodes that fed into the current node, based on the
 weight of the connection.
 Steps:
 1. Compute the error at the output layer: sigmoid_d(output) * (difference between expected and computed outputs)
    for each output
 2. Compute the error at the hidden layer: sigmoid_d(hidden) * 
	sum(weights_o_h * difference between expected output and computed output at output layer)
	for each hidden layer node
 3. Adjust the weights from the hidden to the output layer: learning rate * error at the output layer * error at the hidden layer
    for each connection between the hidden and output layers
 4. Adjust the weights from the input to the hidden layer: learning rate * error at the hidden layer * input layer node value
    for each connection between the input and hidden layers
 5. REMEMBER TO FREE ANY ALLOCATED MEMORY WHEN YOU'RE DONE (or use std::vector ;)
*/
void CNeuralNet::propagateErrorBackward(const double * const desiredOutput){
	double *errol=new double[outputLayerSize];
	double*errhl = new double[hiddenLayerSize];
	for (int i = 0; i < outputLayerSize; i++)
	{
		errol[i]=sigmoid_d(output[i])*(desiredOutput[i] - output[i]);

	}

	for (int i = 0; i < hiddenLayerSize; i++)
	{
		
		double sum = 0;
		for (int o = 0; o < outputLayerSize; o++)
		{
			sum +=outputWeights[i][o] * (desiredOutput[o] - output[o]);
		}
		errhl[i] = sigmoid_d(hidden[i])*sum;
	}
	
	for (int i = 0; i < hiddenLayerSize; i++)
	{
		for (int o = 0; o < outputLayerSize; o++)
		{
			outputWeights[i][o] += lRate * errol[o] * errhl[i];
		}
	}
	for (int i = 0; i < inputLayerSize; i++)
	{
		
		for (int o = 0; o < hiddenLayerSize; o++)
		{
			hiddenWeights[i][o] += lRate*errhl[o] * input[i];
		}
	}
	delete[]errol;
	delete[]errhl;
}
/**
This computes the mean squared error
A very handy formula to test numeric output with. You may want to commit this one to memory
*/
double CNeuralNet::meanSquaredError(const double * const desiredOutput){
	/*TODO:
	sum <- 0
	for i in 0...outputLayerSize -1 do
		err <- desiredoutput[i] - actualoutput[i]
		sum <- sum + err*err
	return sum / outputLayerSize
	*/
	double sum = 0;
	for (int i = 0; i < outputLayerSize; i++)
	{
		double err = desiredOutput[i] - output[i];
		sum += err*err;

	}
	return sum/outputLayerSize;
}
/**
This trains the neural network according to the back propagation algorithm.
The primary steps are:
for each training pattern:
  feed forward
  propagate backward
until the MSE becomes suitably small
*/
void CNeuralNet::train(const double** const inputs,
		const double** const outputs, uint trainingSetSize)
{
	float MSE = 1.0f;
	while (MSE > mse_cutoff)
	{

		for (int i = 0; i < trainingSetSize; i++)
		{
			
				feedForward(inputs[i]);
				propagateErrorBackward(outputs[i]);
				cout << "Learning";
		}
		MSE = meanSquaredError(output);
		
	}
	
}
/**
Once our network is trained we can simply feed it some input though the feed forward
method and take the maximum value as the classification
*/
uint CNeuralNet::classify(const double * const input){
	feedForward(input);
	double max = output[0];
	
	for (int i = 0; i <outputLayerSize; i++)
	{
		
		if (output[i]>max)
		{
			max = output[i];
			
		}
	}
	
	return max;
	
}
/**
Gets the output at the specified index
*/
double CNeuralNet::getOutput(uint index) const{
	if (index < outputLayerSize)
	{

		return output[index];
	}
	return 0;
}
