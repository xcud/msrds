using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace HopfieldNeuralNetwork
{
    /// <summary>
    /// Represents the method that will handle an event that rise when Energy of Hopfield Neural Network changes.
    /// </summary>
    /// <param name="sender">The source of the event</param>
    /// <param name="e">An <typeparamref name="EnergyEventArgs"/> that contains value of Energy</param>
    /// <seealso cref="HopfieldNeuralNetwork.Neuron"/>
    /// <seealso cref="HopfieldNeuralNetwork.NeuralNetwork"/>
    public delegate void EnergyChangedHandler(object sender, EnergyEventArgs e);
    
    /// <summary>
    /// Defines the class for Hopfield Neural Network
    /// </summary>
    /// <seealso cref="HopfieldNeuralNetwork.Neuron"/>
    public class NeuralNetwork
    {
        private List<Neuron> neurons;

        private int n;
        private int m;
        private double e;
        private int[,] T;

        private void CalculateEnergy()
        {
            double tempE = 0;
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (i != j)
                        tempE += T[i, j] * neurons[i].State * neurons[j].State;
            e = -1 * tempE / 2;
        }
        /// <summary>
        /// Gets number of neurons in neural network
        /// </summary>
        /// <value>An <typeparamref name="Int32"/> representing a number of neurons</value>
        public int N
        {
            get { return n; }
        }
        /// <summary>
        /// Gets number of patterns currently stored in interconnection matrix
        /// </summary>
        /// <value>An <typeparamref name="Int32"/> representing a number of patterns</value>
        /// <remarks>The value of M increases every time when new pattern added via AddPattern or AddRandomPattern</remarks>
        public int M
        {
            get { return m; } 
        }        
        /// <summary>
        /// Gets the value of Neural network energy
        /// </summary>
        public double Energy
        {
            get { return e; }
        }
        /// <summary>
        /// Gets the interonnection matrix of Neural network
        /// </summary>
        public int[,] Matrix
        {
            get { return T;}
        }
        /// <summary>
        /// Gets current Neural Network state.
        /// </summary>
        public List<Neuron> Neurons
        {
            get { return neurons; }
        }
        /// <summary>
        /// Initializes a new instance of the <seealso cref="NeuralNetwork"/> class
        /// </summary>
        /// <param name="n">Number of neurons</param>
        public NeuralNetwork(int n)
        {
            this.n = n;
            neurons = new List<Neuron>(n);
            for (int i = 0; i< n; i++)
            {
                Neuron neuron = new Neuron();
                neuron.State = 0;
                neurons.Add(neuron);
            }
            
            T = new int[n, n];
            m = 0;
            
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                {
                    T[i, j] = 0;
                }
        }
        /// <summary>
        /// Adds a random pattern to interconnection matrix
        /// </summary>
        public void AddRandomPattern()
        {
            List<Neuron> randomPattern = new List<HopfieldNeuralNetwork.Neuron>(n);
            Random rnd = new Random();
            for (int i = 0; i < n; i++)
            {
                Neuron neuron = new Neuron();
                randomPattern.Add(neuron);
                
                int bit;
                bit = rnd.Next(2);
                if (bit == 0) randomPattern[i].State = NeuronStates.AlongField; 
                else
                if (bit == 1) randomPattern[i].State = NeuronStates.AgainstField;
            }
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                {
                    if (i == j) T[i, j] = 0;
                    else T[i, j] += randomPattern[i].State * randomPattern[j].State;
                }
            m++;
        }

        /// <summary>
        /// Adds specified pattern to intercconnection matrix
        /// </summary>
        /// <param name="Pattern">A list of neurons</param>
        public void AddPattern(List<Neuron> Pattern)
        {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                {
                    if (i == j) T[i, j] = 0;
                    else T[i, j] += (Pattern[i].State * Pattern[j].State);
                }
            m++;
        }
        
        /// <summary>
        /// Clears values of interconnection matrix.
        /// </summary>
        public void FreeMatrix()
        {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    T[i, j] = 0;
        }

        /// <summary>
        /// Sets specified initial state and runs networks dynamics
        /// </summary>
        /// <param name="initialState">A list of neurons which determines an initional state</param>
        public void Run(List<Neuron> initialState)
        {
            this.neurons = initialState;
            int k = 1;
            int h = 0;
                while(k != 0)
                {
                    k = 0;
                    for (int i = 0; i < n; i++)
                    {
                        h = 0;
                        for (int j = 0; j < n; j++)                        
                          h += T[i, j] * (neurons[j].State);
                        
                        if (neurons[i].ChangeState(h))
                        {
                            k++;
                            CalculateEnergy();
                            OnEnergyChanged(new EnergyEventArgs(e,i));
                        }
                    }
                }
                CalculateEnergy();             
        }

        /// <summary>
        /// Occurs when the energy of neural network changes
        /// </summary>
        public event EnergyChangedHandler EnergyChanged;
        
        /// <summary>
        /// Rises the <seealso cref="EnergyChanged"/> event
        /// </summary>
        /// <param name="e">An <typeparamref name="EnergyEventArgs"/> that contains value of Energy and index of neuron that couses energy change</param>
        protected virtual void OnEnergyChanged(EnergyEventArgs e)
        {
            if (EnergyChanged != null)
                EnergyChanged(this, e);
        }
    }
}
