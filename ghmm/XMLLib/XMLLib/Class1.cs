using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.IO;

namespace XMLLib
{

   

    public class Digit
    {
        public uint _NUM;

        public uint[] _SIMBOL;
        //[XmlArrayItem(ElementName = "uint", IsNullable = true, Type = typeof(uint), Namespace = "")]
        //[XmlArray]
        //public uint[] SIMBOL
        //{
        //    get { return _SIMBOL; }
        //    set { _SIMBOL = value; }
        //}

    }
}
