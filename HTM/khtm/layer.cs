using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    class layer
    {
        layer _UpLayer ;//= new layer();
        List<Sequence> _list = new List<Sequence>();

        Sequence _UpInput ;//= new Sequence();
        int[] _Data;
        int _CurCount  = 0;

        int[,] _Weights;
        public layer(int dataSize,int upLayerDataSize)
        {
            _Data = new int[dataSize];

            _Weights = new int[dataSize, upLayerDataSize];
 
        }

        int AddInputData(int dataIndex)
        {
            _Data[_CurCount] = dataIndex;
            _CurCount++;

            int sequenceIndex = 0;
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
                input._Index = _list.Count;

                sequence = input;
            }


            if (_UpLayer != null)
            {
                int upLayerIndex = _UpLayer.AddInputData(input._Index);
                if (upLayerIndex != 0)
                {

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
