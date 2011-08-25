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
            if (IsExist(input) == true)
            {

            }
        }

        private bool IsExist(Sequence input)
        {
            foreach (var sequence in _list)
            {

                return true;
            }

            return false;
        }
    }
}
