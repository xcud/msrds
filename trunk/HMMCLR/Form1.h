#pragma once


#pragma unmanaged

#include "../HMMcpp/HMM.h"


#pragma managed

#include "../CLROpenCV/CLRGestureDetect.h"
#include "../CLROpenCV/CLRGestureRecode.h"
#include "../CLROpenCV/CLROpenCV.h"


namespace HMMCLR {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:

	
	private: System::Windows::Forms::Button^  Dectect;
	private: System::Windows::Forms::Timer^  _timer3;
	public: 


		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			Init();


		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Button^  button2;

	private: System::Windows::Forms::PictureBox^  pictureBox2;
	private: System::Windows::Forms::ListBox^  _Log;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Timer^  _timer1;
	private: System::Windows::Forms::PictureBox^  pictureBox3;

private: System::Data::DataSet^  dataSet1;
private: System::Data::DataTable^  dataTable1;
private: System::Data::DataColumn^  dataColumn1;
private: System::Data::DataColumn^  dataColumn2;
private: System::Windows::Forms::ListBox^  listBox1;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Timer^  _timer2;



	private: System::ComponentModel::IContainer^  components;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->_Log = (gcnew System::Windows::Forms::ListBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->_timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->dataSet1 = (gcnew System::Data::DataSet());
			this->dataTable1 = (gcnew System::Data::DataTable());
			this->dataColumn1 = (gcnew System::Data::DataColumn());
			this->dataColumn2 = (gcnew System::Data::DataColumn());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->_timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->Dectect = (gcnew System::Windows::Forms::Button());
			this->_timer3 = (gcnew System::Windows::Forms::Timer(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataSet1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataTable1))->BeginInit();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(967, 323);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(991, 76);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 12);
			this->label1->TabIndex = 1;
			this->label1->Text = L"label1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(991, 117);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(38, 12);
			this->label2->TabIndex = 2;
			this->label2->Text = L"label2";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(991, 164);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(38, 12);
			this->label3->TabIndex = 3;
			this->label3->Text = L"label3";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(991, 207);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(38, 12);
			this->label4->TabIndex = 4;
			this->label4->Text = L"label4";
			// 
			// pictureBox1
			// 
			this->pictureBox1->ErrorImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.ErrorImage")));
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->InitialImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.InitialImage")));
			this->pictureBox1->Location = System::Drawing::Point(159, 0);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(320, 240);
			this->pictureBox1->TabIndex = 5;
			this->pictureBox1->TabStop = false;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(12, 489);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(120, 23);
			this->button2->TabIndex = 6;
			this->button2->Text = L"Start";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// pictureBox2
			// 
			this->pictureBox2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox2.Image")));
			this->pictureBox2->InitialImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox2.InitialImage")));
			this->pictureBox2->Location = System::Drawing::Point(357, 246);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(320, 240);
			this->pictureBox2->TabIndex = 7;
			this->pictureBox2->TabStop = false;
			// 
			// _Log
			// 
			this->_Log->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->_Log->FormattingEnabled = true;
			this->_Log->ItemHeight = 12;
			this->_Log->Location = System::Drawing::Point(0, 541);
			this->_Log->Name = L"_Log";
			this->_Log->Size = System::Drawing::Size(1065, 208);
			this->_Log->TabIndex = 8;
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Location = System::Drawing::Point(168, 492);
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(75, 21);
			this->numericUpDown1->TabIndex = 10;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(257, 495);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(41, 12);
			this->label5->TabIndex = 11;
			this->label5->Text = L"Frame";
			// 
			// _timer1
			// 
			this->_timer1->Tick += gcnew System::EventHandler(this, &Form1::_timer1_Tick);
			// 
			// pictureBox3
			// 
			this->pictureBox3->ErrorImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox3.ErrorImage")));
			this->pictureBox3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox3.Image")));
			this->pictureBox3->InitialImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox3.InitialImage")));
			this->pictureBox3->Location = System::Drawing::Point(505, 0);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(320, 240);
			this->pictureBox3->TabIndex = 12;
			this->pictureBox3->TabStop = false;
			// 
			// dataSet1
			// 
			this->dataSet1->DataSetName = L"NewDataSet";
			this->dataSet1->Tables->AddRange(gcnew cli::array< System::Data::DataTable^  >(1) {this->dataTable1});
			this->dataSet1->Initialized += gcnew System::EventHandler(this, &Form1::dataSet1_Initialized);
			// 
			// dataTable1
			// 
			this->dataTable1->Columns->AddRange(gcnew cli::array< System::Data::DataColumn^  >(2) {this->dataColumn1, this->dataColumn2});
			this->dataTable1->TableName = L"GestureTable";
			// 
			// dataColumn1
			// 
			this->dataColumn1->ColumnName = L"Name";
			// 
			// dataColumn2
			// 
			this->dataColumn2->ColumnName = L"FileName";
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 12;
			this->listBox1->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"바쁘다", L"알맞다"});
			this->listBox1->Location = System::Drawing::Point(12, 0);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(120, 244);
			this->listBox1->TabIndex = 13;
			this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::listBox1_SelectedIndexChanged);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(12, 360);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(120, 23);
			this->button3->TabIndex = 14;
			this->button3->Text = L"Analysis";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// _timer2
			// 
			this->_timer2->Tick += gcnew System::EventHandler(this, &Form1::_timer2_Tick);
			// 
			// Dectect
			// 
			this->Dectect->Location = System::Drawing::Point(12, 421);
			this->Dectect->Name = L"Dectect";
			this->Dectect->Size = System::Drawing::Size(120, 23);
			this->Dectect->TabIndex = 15;
			this->Dectect->Text = L"Dectect";
			this->Dectect->UseVisualStyleBackColor = true;
			this->Dectect->Click += gcnew System::EventHandler(this, &Form1::Dectect_Click);
			// 
			// _timer3
			// 
			this->_timer3->Tick += gcnew System::EventHandler(this, &Form1::_timer3_Tick);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1065, 749);
			this->Controls->Add(this->Dectect);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->pictureBox3);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->_Log);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataSet1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataTable1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


		CLROpenCVs::CLRGestureDetect ^ _gestureDetect;// = gcnew CLRGestureDetect;
		CLROpenCVs::CLRGestureRecode ^ _gestureRecode;


	
		 long _current_frame;

		 Bitmap ^ _bmp1;
		 Bitmap ^ _bmp2;
		 Bitmap ^ _bmp3;

		void Init()
		{
		
			_gestureDetect = gcnew CLROpenCVs::CLRGestureDetect;
			_gestureRecode = gcnew CLROpenCVs::CLRGestureRecode(8);

			 _bmp1 = gcnew Bitmap(320,240,Drawing::Imaging::PixelFormat::Format8bppIndexed);
			 _bmp2 = gcnew Bitmap(320,240,Drawing::Imaging::PixelFormat::Format8bppIndexed);
			 _bmp3 = gcnew Bitmap(320,240,Drawing::Imaging::PixelFormat::Format8bppIndexed);
			 

//			 Bitmap::GetHbitmap()


//				_bmp->GetHbitmap()
			 //TBXITEMINFO[] tbxInfo;
			 //tbxInfo = new TBXITEMINFO[1];
			 //tbxInfo[0].bstrText = "mytoolboxitem";
			 //tbxInfo[0].hBmp = bmp.GetHbitmap();

			 //ColorPalette pal = bitmap.Palette;
			 //for (int i = 0; i < iCol; i++)
			 //{
				// ColorEntry ce = ct.GetColorEntry(i);
				// pal.Entries[i] = Color.FromArgb(ce.c4, ce.c1, ce.c2, ce.c3);
			 //}
			 //bitmap.Palette = pal;

			 ColorPalette ^pal = _bmp1->Palette;
			 for(int i = 0;i < 256;i++)
			 {
				 pal->Entries[i] = Drawing::Color::FromArgb(i,i,i);
				//pictureBox2->Image->Palette->Entries[i] = Drawing::Color::FromArgb(i,i,i);
			}
			 //pal->Entries[1] = Drawing::Color::FromArgb(255,255,255);
			_bmp1->Palette = pal;

			pictureBox1->Image->Palette = _bmp1->Palette;


			pal = _bmp2->Palette;
			for(int i = 0;i < 256;i++)
			{
				pal->Entries[i] = Drawing::Color::FromArgb(i,i,i);
				//pictureBox2->Image->Palette->Entries[i] = Drawing::Color::FromArgb(i,i,i);
			}
			//pal->Entries[1] = Drawing::Color::FromArgb(255,255,255);
			_bmp2->Palette = pal;

			pictureBox2->Image->Palette = _bmp2->Palette;


			pal = _bmp3->Palette;
			for(int i = 0;i < 256;i++)
			{
				pal->Entries[i] = Drawing::Color::FromArgb(i,i,i);
				//pictureBox2->Image->Palette->Entries[i] = Drawing::Color::FromArgb(i,i,i);
			}
			//pal->Entries[1] = Drawing::Color::FromArgb(255,255,255);
			_bmp3->Palette = pal;

			pictureBox3->Image->Palette = _bmp3->Palette;

		}


	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

				 /*
				 http://en.wikipedia.org/wiki/Viterbi_algorithm
				 states = ('Rainy', 'Sunny')
				 observations = ('walk', 'shop', 'clean')

				 start_probability = {'Rainy': 0.6, 'Sunny': 0.4}

				 transition_probability = {
				 'Rainy' : {'Rainy': 0.7, 'Sunny': 0.3},
				 'Sunny' : {'Rainy': 0.4, 'Sunny': 0.6},
				 }

				 emission_probability = {
				 'Rainy' : {'walk': 0.1, 'shop': 0.4, 'clean': 0.5},
				 'Sunny' : {'walk': 0.6, 'shop': 0.3, 'clean': 0.1},
				 }



				 int N = 2;
				 int M = 3;
				 int K = 3;

				 FArr1D P0(N);
				 P0(1) = 0.6;
				 P0(2) = 0.4;

				 FArr2D A(N,N);
				 A(1,1) = 0.7; A(1,2) = 0.3;
				 A(2,1) = 0.4; A(2,2) = 0.6;

				 FArr2D B(N,M);
				 B(1,1) = 0.1; B(1,2) = 0.4; B(1,3) = 0.5;
				 B(2,1) = 0.6; B(2,2) = 0.3; B(2,3) = 0.1;

				 // ('walk', 'shop', 'clean')
				 IArr1D Idxs(K);
				 Idxs(1) = 1; Idxs(2) = 2; Idxs(3) = 3;

				


				 */



				 /*

				 states = ('Sunny', 'Rainy','Cloudy')
				 observations = ('umbrella', 'no umbrella')

				 start_probability = {'Sunny': 0.33, 'Rainy': 0.33, 'Cloudy': 0.33}

				 transition_probability = {
				 'Sunny' : {'Sunny': 0.8, 'Rainy': 0.05, 'Cloudy': 0.15},
				 'Rainy' : {'Sunny': 0.2, 'Rainy': 0.6, 'Cloudy': 0.2},
				 'Cloudy' : {'Sunny': 0.2, 'Rainy': 0.3, 'Cloudy': 0.5},
				 
				 }

				 emission_probability = {
				 'Sunny' : {'umbrella': 0.1, 'no umbrella': 0.9},
				 'Rainy' : {'umbrella': 0.8, 'no umbrella': 0.2},
				 'Cloudy' : {'umbrella': 0.3, 'no umbrella': 0.7},
				 }



				 */
				 


				 int N = 3;
				 int M = 2;
				 int K = 3;

				 FArr1D P0(N);
				 P0(1) = 0.6;
				 P0(2) = 0.4;

				 FArr2D A(N,N);
				 A(1,1) = 0.7; A(1,2) = 0.3;
				 A(2,1) = 0.4; A(2,2) = 0.6;

				 FArr2D B(N,M);
				 B(1,1) = 0.1; B(1,2) = 0.4; B(1,3) = 0.5;
				 B(2,1) = 0.6; B(2,2) = 0.3; B(2,3) = 0.1;

				 // ('walk', 'shop', 'clean')
				 IArr1D Idxs(K);
				 Idxs(1) = 1; Idxs(2) = 2; Idxs(3) = 3;



				 HMM * hmm = new HMM(A, B, P0);


				 this->label1->Text = "Forward: "+ String::Format("{0}",hmm->GetProbabilityF(Idxs));

				 double f = hmm->GetProbabilityF(Idxs);

				 this->label2->Text = "Backward: "+ String::Format("{0}",hmm->GetProbabilityB(Idxs)); //FloatToStr(hmm->GetProbabilityB(Idxs));

				 double b = hmm->GetProbabilityB(Idxs);

				 IArr1D S = hmm->ViterbiStateIdxs(Idxs);


				 //char * ss = "Viterbi States: ";
				 System::String ^ ss = "Viterbi States: ";
				 for (int i = S.L1(); i <= S.H1(); i++)
					 ss += System::String::Format("{0} , ",S(i)) ;

				 this->label3->Text = ss;
				 //Label3->Caption = ss;


				 //Output:  ("Sunny", "Rainy", "Rainy")

				 S = hmm->PosteriorDecodingIdxs(Idxs);
				 
				 ss = "Posterior States: ";


				 for (int i = S.L1(); i <= S.H1(); i++)
					ss += System::String::Format("{0} , ",S(i)) ;
				 this->label4->Text = ss;

				 delete hmm;

			 }

			 

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {

				if( this->button2->Text == "Start")
				{
					_Log->Items->Add("test");
					_timer1->Start();
					

					this->button2->Text = "End";

					//_gestureDetect->OpenAviFile("gesture.avi");
					_gestureDetect->OpenCam(0);

					_current_frame = 1;
				}else
				{
					this->button2->Text = "Start";
					_timer1->Stop();
				}

			 }



	private: System::Void _timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
				
				_current_frame ++;				
				IplImage * pCVImage = _gestureDetect->NextGestureInfo(_current_frame);
				if( !pCVImage)
					return ;
				
				//Drawing::Bitmap ^ pBitMap = gcnew Drawing::Bitmap;
				//Bitmap ^ bmp = gcnew Bitmap(pictureBox2->Image);
				


				CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(_gestureDetect->GesDetectedInfo(),_bmp1);
				pictureBox1->Image = _bmp1;



				//pictureBox2->Image->ma
				CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(pCVImage,_bmp2);
				pictureBox2->Image = _bmp2;
				

				_gestureRecode->CompareModel(_gestureDetect->GesDetectedInfo(),0);

				//CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(_gestureDetect->GetOpticalFlowImage(),_bmp3);
				//pictureBox3->Image = _bmp3;
				


				//pictureBox2->Image->FromHbitmap(_bmp->GetHbitmap());
					//= _bmp;
				
			 }

	private: System::Void dataGridView1_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 }
private: System::Void dataSet1_Initialized(System::Object^  sender, System::EventArgs^  e) {

//			 System::String ^ myXMLfile = "Gesture.xml";
//			 
//
//			 // Create new FileStream with which to read the schema.
//			 System::IO::FileStream ^ fsReadXml = gcnew System::IO::FileStream (myXMLfile, System::IO::FileMode::Open);
//				 dataSet1->ReadXml(fsReadXml);
//				 //dads.ReadXml(fsReadXml);
//				 dataGridView1->DataSource = dataSet1;
////				 dataGridView1->DataMember = "Cust";
//				 fsReadXml->Close();


		 }


			int _size;

			void OpenAvi(System::String ^ fileName)
			{

				fileName += ".avi";

				System::IntPtr codePtr1 = Marshal::StringToHGlobalAnsi (fileName);

				char* cType = (char*)(void*)codePtr1.ToPointer();									

				_gestureDetect->OpenAviFile(cType);

				_current_frame = 1;

				
				Marshal::FreeHGlobal(codePtr1);
			}

private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {

			 // 타이머가 돌면서 분석한 이미지를 저장한다.
			 
			 _size = this->listBox1->Items->Count - 1;

			_timer2->Start();
			

			OpenAvi((System::String ^)this->listBox1->Items[ _size]);

			//for each(System::String ^ name in this->listBox1->Items)
			//{
			//	
			//}

		 }
private: System::Void _timer2_Tick(System::Object^  sender, System::EventArgs^  e) {
				_current_frame ++;				
				IplImage * pCVImage = _gestureDetect->NextGestureInfo(_current_frame);

				if( !pCVImage)
				{

					_gestureRecode->AddModel((System::String ^)this->listBox1->Items[ _size], _gestureDetect->GesDetectedInfo());

					_size--;

					if( _size < 0)
					{
						_timer2->Stop();
						return;
					}

					OpenAvi((System::String ^)this->listBox1->Items[ _size]);

					return ;
				}

				//Drawing::Bitmap ^ pBitMap = gcnew Drawing::Bitmap;
				//Bitmap ^ bmp = gcnew Bitmap(pictureBox2->Image);
				
				CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(_gestureDetect->GesDetectedInfo(),_bmp1);
				pictureBox1->Image = _bmp1;

			

		 }
private: System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

				
			if( this->listBox1->SelectedItem == nullptr)
				return ;

			CLROpenCVs::DataInfo ^ pDataInfo = _gestureRecode->GetModel((System::String ^) this->listBox1->SelectedItem);
			CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(pDataInfo->_pImage,_bmp1);
			pictureBox1->Image = _bmp1;

		 }
private: System::Void Dectect_Click(System::Object^  sender, System::EventArgs^  e) {

			

		 }
private: System::Void _timer3_Tick(System::Object^  sender, System::EventArgs^  e) {


		 }
};
}

