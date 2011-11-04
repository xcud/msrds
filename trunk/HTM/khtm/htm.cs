using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    public class htm
    {
        //layer _RootLayer = null;
        NLayer.iLayer[] _Layer;
        int[][,] _Weights;
        NLayer.TargetLayer _TargetLayer;
        
        public void BuildNetwork(int [] layerSizeSet)
        {

            _Weights = new int [layerSizeSet.Length-1][,];

            for (int i = 0; i < _Weights.Length; i++)
            {
                _Weights[i] = new int[layerSizeSet[i],layerSizeSet[i+1]]; 
            }

            _Layer = new NLayer.iLayer[layerSizeSet.Length];

            for(int i =0 ;i < layerSizeSet.Length;i++)
            {
                if (layerSizeSet.Length - 1 == i)
                {
                    _TargetLayer = new NLayer.TargetLayer(layerSizeSet[i]);
                    _Layer[i] = _TargetLayer;
                }
                else
                {
                    _Layer[i] = new NLayer.MiddleKLayer(i,layerSizeSet[i]);
                }



                if (i != 0)
                {
                    _Layer[i - 1].SetUplayer(_Layer[i], _Weights[i-1]);
                    _Layer[i].SetDownlayer(_Layer[i - 1], _Weights[i-1]);
                }
                else
                {

                }


            }
        }



        public void AddInputData(int dataIndex)
        {
            _Layer[0].AddInputData(dataIndex);
        }



        public void SetTarget(string symbol)
        {
            _TargetLayer.SetTarget(symbol);
        }
    }
}
