using System;
using System.Collections.Generic;
using System.Text;
using System.IO;


namespace gameAI.NerualNetwork
{


    

    public class KData
    {
        public List<String> _InputList = new List<string>();
        public string _Output;
    }



    public class KNeural
    {
        enum ACTION
        {
            ATTACK,
            STAND,
            NEAR,
            FAR,
            DODGE,
        }

        const int row = 4;
        const int col = 11;
        const int col2 = col-1;

        static public KLearnData[] GetLearnData()
        {

            double[,] dumyData = new double[row,col]


        //                       DISTANCE,EA_ATTACK,EA_STAND,EA_NEAR,EA_DODGE,MA_DODGE,MA_ATTACK,MA_STAND,MA_NEAR,MA_FAR ,    OUTPUT
                             {      {0,       1   ,   0    ,    0    ,   0   ,   1    ,   0   ,    0  ,  0    ,  0    ,(double) ACTION.DODGE },
                                    {0.1,     0   ,   1    ,    0    ,   0   ,   0    ,   1   ,    0  ,  0    ,  0    ,(double) ACTION.ATTACK },
                                    {0.2,     0   ,   1    ,    0    ,   0   ,   0    ,   1   ,    0  ,  0    ,  0    ,(double) ACTION.ATTACK },
                                    {0.4,     0   ,   1    ,    0    ,   0   ,   0    ,   0   ,    0  ,  1    ,  0    ,(double) ACTION.ATTACK },
                              };


            KLearnData[] data = new KLearnData[row];

            for (int j = 0; j < row; j++)
            {
                data[j] = new KLearnData();
                int size = col - 1;
                data[j]._Input = new double[size];

                for (int i = 0; i < col; i++)
                {

                    if (i == col - 1)
                    {
                        int na = (int) dumyData[j, i];
                        data[j]._Symbol = ((ACTION)na).ToString();
                    }
                    else
                    {
                        data[j]._Input[i] = dumyData[j, i];
                    }
                    
                    
                }


            }

            return data;      

        }




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
