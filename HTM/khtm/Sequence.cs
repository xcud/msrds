using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    class Sequence
    {
        public Sequence(int dataSize)
        {
            _Data = new int[dataSize];
        }
        public Sequence(int[] data)
        {
            _Data = data;
        }


        public int _Index;
        public int[] _Data;
        
    }
}
