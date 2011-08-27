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
        public layer(int dataSize)
        {
            _Data = new int[dataSize];
 
        }

        void AddInputData(int dataIndex)
        {
            _Data[_CurCount] = dataIndex;
            _CurCount++;
            if (_Data.Length == _CurCount)
            {
                AddInput(new Sequence(_Data));
                _Data = new int[_Data.Length];
                _CurCount = 0;
            }
        }

        internal void AddInput(Sequence input)
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
                _UpLayer.AddInputData(input._Index);
            }
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
