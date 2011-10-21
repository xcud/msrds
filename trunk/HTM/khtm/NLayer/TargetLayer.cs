using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm.NLayer
{
    public class TargetLayer : NLayer.iLayer
    {

        int[] _Data;
        Dictionary<string, int> _SymbolList = new Dictionary<string, int>();
        int _TargetIndex;

        int[,] _DownWeights;
        NLayer.iLayer _DownLayer;


        public TargetLayer(int dataSize)
        {
            _Data = new int[dataSize];
        }

        public void SetTarget(string symbol)
        {
            if (_SymbolList.Keys.Contains(symbol) == false)
            {
                _SymbolList.Add(symbol, _SymbolList.Count);
            }

            _TargetIndex = _SymbolList[symbol];
        }
        
        public void SetUplayer(NLayer.iLayer upLayer, int[,] upWeight)
        {

        }

        public void SetDownlayer(NLayer.iLayer downLayer, int[,] downWeight)
        {
            _DownLayer = downLayer;
            _DownWeights = downWeight;

        }

        public int AddInputData(int dataIndex)
        {


            return _TargetIndex;
        }
    }
}
