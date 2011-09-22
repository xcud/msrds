using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace app
{
    public class SymbolInfo
    {
        public string ID;
        public List<int> _List = new List<int>();
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(ID + "(");
            foreach (var item in _List)
            {
                sb.Append(item + ",");
            }
            sb.Append(")");
            return sb.ToString();
        }
    }
}
