using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace khtm
{
    class layer
    {
        List<Sequence> _list = new List<Sequence>();

        internal void AddInput(Sequence input)
        {
            if (IsExist(input) == false)
            {
                _list.Add(input);
                input._Index = _list.Count;
            }
        }

        private bool IsExist(Sequence input)
        {
            foreach (var sequence in _list)
            {
                if( sequence._Data.SequenceEqual(input._Data) == true)
                    return true;
            }

            return false;
        }
    }
}
