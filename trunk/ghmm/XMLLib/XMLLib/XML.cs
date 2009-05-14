using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.IO;

namespace XMLLib
{

    public class Xml
    {
        static public bool SaveXml<T>(String fileName, T table)
        {
            XmlSerializer s = new XmlSerializer(typeof(T));
            TextWriter myWriter = null;
            bool ret = true;

            try
            {
                myWriter = new StreamWriter(fileName);
                s.Serialize(myWriter, table);
            }
            catch (Exception)
            {
                ret = false;
            }
            if (myWriter != null) myWriter.Close();
            return ret;
        }


        static public T LoadXml<T>(String fileName)
        {
            T mapConfig;

            try
            {
                XmlSerializer s = new XmlSerializer(typeof(T));
                TextReader myReader = new StreamReader(fileName);
                mapConfig = (T)s.Deserialize(myReader);
                myReader.Close();
            }
            catch (Exception)
            {
                return default(T);
            }

            return mapConfig;

        }


    }


}
