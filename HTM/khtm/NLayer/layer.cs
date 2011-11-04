using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm.NLayer
{
    public class MiddleKLayer : NLayer.iLayer
    {
        NLayer.iLayer _UpLayer;//= new layer();
        NLayer.iLayer _DownLayer;
        List<Sequence> _list = new List<Sequence>();

        Sequence _UpInput ;//= new Sequence();
        int[] _Data;
        int _CurCount  = 0;

        int[,] _UpWeights;
        int[,] _DownWeights;

        int _LayerIndex;


        public MiddleKLayer(int LayerIndex ,int dataSize)
        {
            _Data = new int[dataSize];
            _LayerIndex = LayerIndex;
        }

        public void SetUplayer(NLayer.iLayer upLayer, int[,] upWeight)
        {
            _UpLayer = upLayer;
            _UpWeights = upWeight;
        }


        public void SetDownlayer(NLayer.iLayer downLayer, int[,] downWeight)
        {
            _DownLayer = downLayer;
            _DownWeights = downWeight;
        }


        //public void Build()
        //{
        //    if( _UpLayer != null)
        //        _UpWeights = new int[_Data.Length, _UpLayer._Data.Length];
        //}


        public int AddInputData(int dataIndex)
        {
            _Data[_CurCount] = dataIndex;
            _CurCount++;

            int sequenceIndex = -1;
            // 원하는 데이타만큼 다들어왔냐
            if (_Data.Length == _CurCount)
            {
                sequenceIndex = AddInput(new Sequence(_Data));
                _Data = new int[_Data.Length];
                _CurCount = 0;
            }

            return sequenceIndex;

        }

        internal int AddInput(Sequence input)
        {
            var sequence =  FindSequence(input) ;

            if( sequence == null)
            {
                _list.Add(input);
                input._Index = _list.Count-1;

                sequence = input;
            }

            if (_UpLayer != null)
            {
                int upLayerIndex = _UpLayer.AddInputData(input._Index);
                if (upLayerIndex != -1)
                {
                    _UpWeights[input._Index, upLayerIndex]++;
                }
            }

            return sequence._Index;
        }

        private Sequence FindSequence(Sequence input)
        {
            foreach (var sequence in _list)
            {
                if( sequence._Data.SequenceEqual(input._Data) == true)
                    return sequence;
            }

            return null;
        }

    }
}
