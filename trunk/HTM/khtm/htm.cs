using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    public class htm
    {
        //layer _RootLayer = null;
        layer[] _Layer;
        int[][,] _Weights;

        public void BuildNetwork(int [] layerSizeSet)
        {
            for (int i = 0; i < layerSizeSet.Length-1; i++)
            {
                _Weights[i] = new int[layerSizeSet[i],layerSizeSet[i+1]]; 
            }
            
            _Layer = new layer[layerSizeSet.Length];
            for(int i =0 ;i < layerSizeSet.Length;i++)
            {
                _Layer[i] = new layer(layerSizeSet[i]);
                if (i != 0)
                {
                    _Layer[i - 1].SetUplayer(_Layer[i], _Weights[i]);
                    _Layer[i].SetDownlayer(_Layer[i - 1], _Weights[i]);
                }
            }
        }



        public void AddInputData(int dataIndex)
        {
            _Layer[0].AddInputData(dataIndex);
        }
    }
}
