#pragma once

#define MAX_INDEX 8

namespace digit_app {

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
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//


			_bRecodeOn = false;
			_g = pictureBox1->CreateGraphics();
			_Pen = gcnew System::Drawing::Pen(System::Drawing::Color::Blue);
			_hmm = gcnew ghmmc_clr::ghmm;

			_hmm->Init();

			//_hmm->label_higher_order_test();

			_hmm->Test_digit_code();

			_hmm->sequence_alloc_print();
			
			//_hmm->libxml_print();

			//_hmm->label_higher_order_test();

			_SimbolList = gcnew System::Collections::Generic::List<int>();

			_LastAddIndexSimbol = -1;
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

		System::Drawing::Graphics ^ _g;
		System::Drawing::Pen ^ _Pen;
		int _sX;
		int _sY;
		ghmmc_clr::ghmm ^ _hmm;
		System::Collections::Generic::List<int>^ _SimbolList;

	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::ListBox^  listBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::ListBox^  listBox2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button7;
	private: System::Windows::Forms::Button^  button8;
	private: System::Windows::Forms::Button^  button9;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->listBox2 = (gcnew System::Windows::Forms::ListBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button9 = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::SystemColors::Window;
			this->pictureBox1->Location = System::Drawing::Point(12, 57);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(219, 198);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseMove);
			this->pictureBox1->Click += gcnew System::EventHandler(this, &Form1::pictureBox1_Click);
			this->pictureBox1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseClick);
			this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseDown);
			this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::pictureBox1_MouseUp);
			// 
			// listBox1
			// 
			this->listBox1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->listBox1->FormattingEnabled = true;
			this->listBox1->ItemHeight = 12;
			this->listBox1->Location = System::Drawing::Point(0, 307);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(487, 124);
			this->listBox1->TabIndex = 1;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 28);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Clear";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(305, 187);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(52, 23);
			this->button2->TabIndex = 3;
			this->button2->Text = L"ADD";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::SEQ_ADD_Click);
			// 
			// listBox2
			// 
			this->listBox2->FormattingEnabled = true;
			this->listBox2->ItemHeight = 12;
			this->listBox2->Items->AddRange(gcnew cli::array< System::Object^  >(10) {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", 
				L"8", L"9"});
			this->listBox2->Location = System::Drawing::Point(305, 57);
			this->listBox2->Name = L"listBox2";
			this->listBox2->Size = System::Drawing::Size(148, 124);
			this->listBox2->TabIndex = 4;
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(156, 28);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 5;
			this->button3->Text = L"GEN";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(363, 187);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(42, 23);
			this->button4->TabIndex = 6;
			this->button4->Text = L"Save";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::SEQ_Save_Click);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(12, 267);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(75, 23);
			this->button5->TabIndex = 7;
			this->button5->Text = L"Eval";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(411, 188);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(42, 23);
			this->button6->TabIndex = 8;
			this->button6->Text = L"Load";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &Form1::SEQ_Load_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(246, 194);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(30, 12);
			this->label1->TabIndex = 9;
			this->label1->Text = L"SEQ";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(246, 224);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 12);
			this->label2->TabIndex = 10;
			this->label2->Text = L"MODEL";
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(411, 219);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(42, 23);
			this->button7->TabIndex = 13;
			this->button7->Text = L"Load";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &Form1::Model_Load_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(363, 219);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(42, 23);
			this->button8->TabIndex = 12;
			this->button8->Text = L"Save";
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &Form1::Model_Save_Click);
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(305, 219);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(52, 23);
			this->button9->TabIndex = 11;
			this->button9->Text = L"Lean";
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &Form1::Model_Lean_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(487, 431);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->button8);
			this->Controls->Add(this->button9);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->listBox2);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->listBox1);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void pictureBox1_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
			 {
				 
				 
					 

			



			 }
	private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }

			 

	private: System::Void pictureBox1_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
			 {
//				 this->listBox1->Items->Insert(0,e->Location);
				 _bRecodeOn = true;
				 _sX = e->Location.X;
				 _sY = e->Location.Y;
				 


			 }

	bool _bRecodeOn;



	int GetSimbolIndex(int sx,int sy,int ex,int ey)
	{


		
		System::Windows::Vector sv(1,0);
		System::Windows::Vector ev(ex-sx,ey-sy);

		double angle = System::Windows::Vector::AngleBetween(ev,sv);

		//double ang = System::Math::Acos(.Dot(v1, v2) / (v1.Length() * v2.Length()));

		//double rad = System::Math::Atan(v.Y/v.X);


//		int angle = -(int)(float)(rad * 180.0f / System::Math::PI);

		if( 0 <= angle )
			return (int)angle ;

		return 360 + (int)angle ;

	}

	int _LastAddIndexSimbol;

	private: System::Void pictureBox1_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
			 {
				if( _bRecodeOn)
				{
					
					
					if( _SimbolList->Count == 10)
						return;

					System::Windows::Vector v(e->Location.X-_sX,e->Location.Y-_sY);
					
					if( v.Length < 10)
						return;

					_g->DrawLine(_Pen,_sX,_sY,e->Location.X,e->Location.Y);
					
					int simbol = GetSimbolIndex(_sX,_sY,e->Location.X,e->Location.Y)/(360/MAX_INDEX);

					this->listBox1->Items->Insert(0,simbol);
					
					//if( _LastAddIndexSimbol != -1 )
					//	return;
					
					_LastAddIndexSimbol = simbol;

					_SimbolList->Add(simbol);

					_sX = e->Location.X;
					_sY = e->Location.Y;
					



					
					
					
				}
					
			 }

	private: System::Void pictureBox1_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
			 {
				_bRecodeOn = false;
				
			 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			_g->Clear(System::Drawing::Color::White);
			_SimbolList->Clear();
			_LastAddIndexSimbol = -1;
		 }
		 // 시퀀스를 특정 길이만큼으로 만든다
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) 
		 {

		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if( this->listBox2->SelectedItem == nullptr)
				 return;
				
			 _hmm->Add_Lean(this->listBox2->SelectedIndex,this->_SimbolList);

			 button1_Click(nullptr,nullptr);
		 }
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			//_hmm->Save();
			
			

		 }
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {

			 _hmm->Load();

			 for each(	int index in _hmm->_ModelList->Keys)
			 {

				 this->listBox1->Items->Insert(0,"load : " + index.ToString());
			 }
			 //_hmm-   
		 }
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {


			 _hmm->Estimate( this->_SimbolList);
		 }

private: System::Void Model_Lean_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( this->listBox2->SelectedItem == nullptr)
				 return;
				
			 _hmm->Model_Lean(this->listBox2->SelectedIndex);
		 }

private: System::Void Model_Save_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( this->listBox2->SelectedItem == nullptr)
				 return;
				
			 _hmm->Model_Save(this->listBox2->SelectedIndex);

		 }

private: System::Void Model_Load_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void SEQ_ADD_Click(System::Object^  sender, System::EventArgs^  e) {
			  if( this->listBox2->SelectedItem == nullptr)
				 return;
				
			 _hmm->Seq_Add(this->listBox2->SelectedIndex,this->_SimbolList);

		 }
private: System::Void SEQ_Save_Click(System::Object^  sender, System::EventArgs^  e) {
 			  if( this->listBox2->SelectedItem == nullptr)
				 return;
				
			 _hmm->Seq_Save(this->listBox2->SelectedIndex);

		 }

private: System::Void SEQ_Load_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

