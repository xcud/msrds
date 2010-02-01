using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WindowsGame2
{
    public class InputRecoder
    {
        public delegate string inputSymbolHandle();

        public class inputInfo
        {
            public inputSymbolHandle _SymbolHandle;
            public string _lastOutput = string.Empty;
            public string _CurOutput = string.Empty;
        }


        

        Dictionary<string, inputInfo> _symbolHandleMap = new Dictionary<string, inputInfo>();

        public void Regist(string inputSimbol,inputSymbolHandle handle)
        {
            inputInfo ii = new inputInfo();
            ii._SymbolHandle = handle;

            _symbolHandleMap.Add(inputSimbol, ii);
        }

        public void EventChange()
        {

            bool bChanged = false;

            foreach (var pair in _symbolHandleMap)
            {
                pair.Value._CurOutput = pair.Value._SymbolHandle();

                if (pair.Value._lastOutput != pair.Value._CurOutput)
                {
                    bChanged = true;
                }
            }

            if (bChanged)
            {

            }
        }

    }
}
