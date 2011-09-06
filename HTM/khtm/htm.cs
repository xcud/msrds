using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    class htm
    {
        //layer _RootLayer = null;
        layer[] _Layer;
        public void BuildNetwork(int [] layerSizeSet)
        {
            layer frontLayer = null;
            _Layer = new layer[layerSizeSet.Length];
            for(int i =0 ;i < layerSizeSet.Length;i++)
            {
                _Layer[i] = new layer(layerSizeSet[i]);
                if (i != 0)
                    _Layer[i - 1].SetUplayer(_Layer[i]);
            }
        }

        public void AddInput(Sequence input)
        {
            _Layer[0].AddInput(input);
        }
    }
}
