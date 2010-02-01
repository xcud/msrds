using System;
using System.Collections.Generic;
using System.Text;
using System.IO;


namespace Classifier
{

    public class KData
    {
        public List<String> _InputList = new List<string>();
        public string _Output;
    }

    public class KNeural
    {


        public void LoadData()
        {
            StreamReader reader = null;

            // temp buffers (for 50 samples only)



            // min and max X values

            // samples count
            
            

            try
            {
                string str = null;

                // open selected file

                
                reader = File.OpenText(Directory.GetCurrentDirectory() + @"\Data Samples\gamedata.csv");

                // read the data
                while ((str = reader.ReadLine()) != null)
                {
                    KData data = new KData();

                    // split the string
                    string[] strs = str.Split(';');
                    if (strs.Length == 1)
                        strs = str.Split(',');

                    // allocate data array

                    // parse data
                    for (int j = 0; j < strs.Length - 1; j++)
                    {
                        data._InputList.Add( strs[j]);
                    }
                    data._Output = strs[strs.Length - 1];


                    _Data.Add(data);
                    //// search for min value
                    //if (tempData[samples, 0] < minX)
                    //    minX = tempData[samples, 0];
                    //// search for max value
                    //if (tempData[samples, 0] > maxX)
                    //    maxX = tempData[samples, 0];

                    
                }

                // clear current result

            }
            catch (Exception)
            {

            }
            finally
            {
                // close file
                if (reader != null)
                    reader.Close();
            }


        }



        public List<KData> _Data = new List<KData>();

        public void AddKdata(KData data)
        {
            _Data.Add(data);
        }

    }
}
