using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows;
using System.Xml.Serialization;
using System.IO;

namespace app
{
    public partial class Form1 : Form
    {

        Pen _Pen = new Pen(Color.Black);

        bool _bRecodeOn= false;
        private int _sY;
        private int _sX;
        List<int> _SimbolList = new List<int>();
        List<SymbolInfo> _SymbolInfoList = new List<SymbolInfo>();

        int MAX_INDEX = 8;
        private int _LastAddIndexSimbol;
        khtm.htm _htm = new khtm.htm();

        public Form1()
        {
            InitializeComponent();



            _g = pictureBox1.CreateGraphics();



			_LastAddIndexSimbol = -1;


        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            //pictureBox1.d
        }

        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != System.Windows.Forms.MouseButtons.Left)
                return ;
            

        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            _bRecodeOn = true;
            _sX = e.Location.X;
            _sY = e.Location.Y;
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            _bRecodeOn = false;
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if( _bRecodeOn)
			{
					
					
                //if( _SimbolList.Count == 10)
                //    return;
                
				System.Windows.Vector v = new System.Windows.Vector(e.Location.X-_sX,e.Location.Y-_sY);
					
				if( v.Length < 10)
					return;

				_g.DrawLine(_Pen,_sX,_sY,e.Location.X,e.Location.Y);
					
				int simbol = GetSimbolIndex(_sX,_sY,e.Location.X,e.Location.Y)/(360/MAX_INDEX);

				this.listBox1.Items.Insert(0,simbol);
					
				//if( _LastAddIndexSimbol != -1 )
				//	return;
					
				_LastAddIndexSimbol = simbol;

				_SimbolList.Add(simbol);

				_sX = e.Location.X;
				_sY = e.Location.Y;
					
                }
        }

        
	    int GetSimbolIndex(int sx,int sy,int ex,int ey)
	    {


		
		    Vector sv = new Vector(1,0);
		    Vector ev = new Vector(ex-sx,ey-sy);

		    double angle = Vector.AngleBetween(ev,sv);

		    //double ang = System::Math::Acos(.Dot(v1, v2) / (v1.Length() * v2.Length()));

		    //double rad = System::Math::Atan(v.Y/v.X);


    //		int angle = -(int)(float)(rad * 180.0f / System::Math::PI);

		    if( 0 <= angle )
			    return (int)angle ;

		    return 360 + (int)angle ;


	}
        public Graphics _g { get; set; }

        private void button1_Click(object sender, EventArgs e)
        {
            AddSymbol addSymbol = new AddSymbol(_SymbolInfoList);
            var result = addSymbol.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
            {

                if (addSymbol.textBox1.Text.Length == 0)
                    return;

                var symbolinfo = new SymbolInfo();

                symbolinfo.ID = addSymbol.textBox1.Text;
                symbolinfo._List.AddRange(_SimbolList);

                AddSymbolInfo(symbolinfo);
            }

        }

        private void AddSymbolInfo(SymbolInfo symbolinfo)
        {
            _SymbolInfoList.Add(symbolinfo);
            listBox2.Items.Add(symbolinfo);
        }



        private void button5_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {

            // Create a new XmlSerializer instance with the type of the test class
            
            XmlSerializer SerializerObj = new XmlSerializer(_SymbolInfoList.GetType());

            // Create a new file stream to write the serialized object to a file
            TextWriter WriteFileStream = new StreamWriter(@"data.xml");
            SerializerObj.Serialize(WriteFileStream, _SymbolInfoList);

            // Cleanup
            WriteFileStream.Close();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           
                XmlSerializer SerializerObj = new XmlSerializer(_SymbolInfoList.GetType());
                

                try
                {

                    // Create a new file stream to write the serialized object to a file
                    TextReader WriteFileStream = new StreamReader(@"data.xml");

                    _SymbolInfoList = SerializerObj.Deserialize(WriteFileStream) as List<SymbolInfo>;

                    WriteFileStream.Close();

                    // Cleanup
                    
                }
                catch (Exception ex)
                {
                    
                }
                finally
                {
                    
                }

                foreach (var item in _SymbolInfoList)
                {

                    this.listBox2.Items.Add(item);
                }

        }

        private void remove_Click(object sender, EventArgs e)
        {
            if (-1 < listBox2.SelectedIndex)
            {
                
                _SymbolInfoList.RemoveAt(listBox2.SelectedIndex);
                listBox2.Items.RemoveAt(listBox2.SelectedIndex);

            }

        }

        private void button4_Click(object sender, EventArgs e)
        {
            _g.Clear(Color.White);
            _SimbolList.Clear();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            int [] network = new int [2]  {2,2};

            _htm.BuildNetwork(network);
        }
    }
}
