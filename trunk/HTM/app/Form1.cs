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
        List<Vector> _VectorList = new List<Vector>();
        int MAX_INDEX = 8;
        private int _LastAddIndexSimbol;
        khtm.htm _htm = new khtm.htm();

        const int INPUT = 14;

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

            _VectorList.Add(new System.Windows.Vector(e.Location.X, e.Location.Y));
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            _bRecodeOn = false;




            for (int i = 1; i < _VectorList.Count -1 ; i++)
            {

                var v1 = _VectorList[i] - _VectorList[i - 1];
                var v2 = _VectorList[i+1] - _VectorList[i];

                var len = Vector.AngleBetween(v2, v1);
                listBox1.Items.Add(len);


            }

            while (INPUT+1 < _VectorList.Count)
            {
                double shortLength = double.MaxValue;
                //double shortLength = 0;
                int shortLengthIndex = 0;
                for (int i = 2; i < _VectorList.Count-1; i++)
                {
                    var v1 = _VectorList[i] - _VectorList[i - 1];
                    var v2 = _VectorList[i + 1] - _VectorList[i];

                    var len = Vector.AngleBetween(v2, v1);

                    //var len = Vector.AngleBetween(_VectorList[i], _VectorList[i - 1]);
                    //var len = Vector.Determinant(_VectorList[i],_VectorList[i-1]);
                    if (Math.Abs(len) < shortLength)
                    //if (shortLength < len )
                    {
                        shortLengthIndex = i;
                        shortLength = Math.Abs(len);
                    }
                }

                _VectorList.RemoveAt(shortLengthIndex);
                listBox1.Items.Add("remove");

            }




            _g.Clear(Color.White);


            for (int i = 1; i < _VectorList.Count; i++)
            {
                _g.DrawLine(_Pen, (int)_VectorList[i].X, (int)_VectorList[i].Y, (int)_VectorList[i - 1].X, (int)_VectorList[i - 1].Y);

                int simbol = GetSimbolIndex(_VectorList[i].X, _VectorList[i].Y, _VectorList[i-1].X, _VectorList[i-1].Y) / (360 / MAX_INDEX);

                this.listBox1.Items.Insert(0, simbol);

                //if( _LastAddIndexSimbol != -1 )
                //	return;

                _LastAddIndexSimbol = simbol;

                _SimbolList.Add(simbol);
            }


        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if( _bRecodeOn)
			{
					
					
                //if( _SimbolList.Count == 10)
                //    return;
                
				//System.Windows.Vector v = new System.Windows.Vector(e.Location.X,e.Location.Y);


                System.Windows.Vector v = new System.Windows.Vector(e.Location.X - _sX, e.Location.Y - _sY);

                
                if( v.Length < 5)
                    return;
                
                _VectorList.Add(new System.Windows.Vector(e.Location.X,e.Location.Y));

				_g.DrawLine(_Pen,_sX,_sY,e.Location.X,e.Location.Y);
					

				_sX = e.Location.X;
				_sY = e.Location.Y;
					
            }
        }


        int GetSimbolIndex(double sx, double sy, double ex, double ey)
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

        private void add_new(object sender, EventArgs e)
        {

            if (_SimbolList.Count != INPUT)
            {
                MessageBox.Show("입력 패턴길이가 너무 짧습니다.");
                return;
            }

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

            clear_Click(null,null);

        }

        private void AddSymbolInfo(SymbolInfo symbolinfo)
        {
            _SymbolInfoList.Add(symbolinfo);
            listBox2.Items.Add(symbolinfo);
        }



        private void add_selected(object sender, EventArgs e)
        {
            if (_SymbolInfoList.Count != INPUT)
            {
                MessageBox.Show("입력 패턴길이가 너무 짧습니다.");
                return;
            }

        }

        private void save(object sender, EventArgs e)
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


            //listBox1.Items.Add( Vector.AngleBetween(new Vector(1,0), new Vector(0,1)));
            //listBox1.Items.Add(Vector.AngleBetween(new Vector(1, 0), new Vector(0,-1)));

            //listBox1.Items.Add(Vector.AngleBetween(new Vector(1, 0), new Vector(1,0)));
            //listBox1.Items.Add(Vector.AngleBetween(new Vector(1, 0), new Vector(-1, 0)));
            





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


        private void clear_Click(object sender, EventArgs e)
        {
            _g.Clear(Color.White);
            _SimbolList.Clear();
            _VectorList.Clear();
            listBox1.Items.Clear();
        }

        private void learn(object sender, EventArgs e)
        {
            int[] network = new int[] { 2, 2, _SymbolInfoList.Count };

            _htm.BuildNetwork(network);

            foreach (var dataList in _SymbolInfoList)
            {
                _htm.SetTarget(dataList.ID);
                foreach (var data in dataList._List)
                {
                    _htm.AddInputData(data);
                }
            }
            
        }

        private void evaluate(object sender, EventArgs e)
        {

            if (_SimbolList.Count != INPUT)
            {
                MessageBox.Show("입력 패턴길이가 너무 짧습니다.");
                return;
            }



            int symbolID = _htm.Evaluate(_SimbolList);

            this.listBox1.Items.Insert(0, "evaluate =>" + symbolID.ToString());

        }

    }
}
