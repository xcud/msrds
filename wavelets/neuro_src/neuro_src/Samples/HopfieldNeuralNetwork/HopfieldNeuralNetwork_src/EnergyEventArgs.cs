using System;
using System.Collections.Generic;
using System.Text;

namespace HopfieldNeuralNetwork
{
    /// <summary>
    /// Provides data for the <typeparamref name="EnergyChanged"/> event
    /// </summary>
    public class EnergyEventArgs : EventArgs
    {
        private double energy;
        private int neuronIndex;
        /// <summary>
        /// Gets Energy of Neural network
        /// </summary>
        public double Energy
        {
            get { return energy; }        
        }
        /// <summary>
        /// Initializes a new instance of the <typeparamref name="EnergyEventArgs"/> class with the specified value of Energy
        /// </summary>
        /// <param name="Energy">The double that represents the value of neural network energy</param>
        /// <param name="NeuronIndex">The index f neuron caused energy cahnge</param>
        public EnergyEventArgs(double Energy, int NeuronIndex)
        {
            this.energy = Energy;
            this.neuronIndex = NeuronIndex;

        }
        /// <summary>
        /// Gets index of neuron, which state changing led to energy descrease
        /// </summary>
        public int NeuronIndex
        {
            get { return neuronIndex; }
        }
    }
}
