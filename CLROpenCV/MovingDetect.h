#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;




#include "CLRGestureDetect.h"
#include "CLRGestureRecode.h"
#include "CLROpenCV.h"



namespace CLROpenCV {

	/// <summary>
	/// Summary for MovingDetect
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MovingDetect : public System::Windows::Forms::Form
	{
	public:
		MovingDetect(void)
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
		~MovingDetect()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  pictureBox2;
	protected: 

	protected: 
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Timer^  _timer1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::ComponentModel::IContainer^  components;

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
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->_timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox2
			// 
			this->pictureBox2->Location = System::Drawing::Point(384, 44);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(320, 240);
			this->pictureBox2->TabIndex = 14;
			this->pictureBox2->TabStop = false;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(60, 44);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(320, 240);
			this->pictureBox1->TabIndex = 13;
			this->pictureBox1->TabStop = false;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(60, 412);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(103, 25);
			this->button2->TabIndex = 15;
			this->button2->Text = L"Start";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MovingDetect::button2_Click);
			// 
			// _timer1
			// 
			this->_timer1->Tick += gcnew System::EventHandler(this, &MovingDetect::_timer1_Tick);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(60, 336);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(86, 20);
			this->textBox1->TabIndex = 16;
			// 
			// MovingDetect
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(713, 580);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"MovingDetect";
			this->Text = L"MovingDetect";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion




		CLROpenCVs::CLRGestureDetect ^ _gestureDetect;// = gcnew CLRGestureDetect;
		CLROpenCVs::CLRGestureRecode ^ _gestureRecode;


		
		 Bitmap ^ _bmp1;
		 Bitmap ^ _bmp2;

		void Init()
		{
			
			_gestureDetect = gcnew CLROpenCVs::CLRGestureDetect;
			_gestureRecode = gcnew CLROpenCVs::CLRGestureRecode(8);

			 _bmp1 = gcnew Bitmap(320,240,Drawing::Imaging::PixelFormat::Format8bppIndexed);
			 _bmp2 = gcnew Bitmap(320,240,Drawing::Imaging::PixelFormat::Format8bppIndexed);
			 
			 ColorPalette ^pal = _bmp1->Palette;

			 for(int i = 0;i < 256;i++)
			 {
				 pal->Entries[i] = Drawing::Color::FromArgb(i,i,i);
			}
			_bmp1->Palette = pal;

			pictureBox1->Image = _bmp1;


			pal = _bmp2->Palette;
			for(int i = 0;i < 256;i++)
			{
				pal->Entries[i] = Drawing::Color::FromArgb(i,i,i);
			}
			_bmp2->Palette = pal;

			pictureBox2->Image = _bmp2;

		}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			if( this->button2->Text == "Start")
				{
					

					this->button2->Text = "End";

					_gestureDetect->OpenCam(0);
					

					// opencv 초기화 하고 콜해야함
					_timer1->Start();

					
				}else
				{
					this->button2->Text = "Start";
					_timer1->Stop();
				}

			 }

	private: System::Void _timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
	
				IplImage * pCVImage = _gestureDetect->NextGestureInfo(0);
				if( !pCVImage)
					return ;
				
				int index = _gestureRecode->GetMovingIndex(_gestureDetect->GesDetectedInfo(),4);

				CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(_gestureDetect->GesDetectedInfo(),_bmp1);
				pictureBox1->Image = _bmp1;

				
				CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(pCVImage,_bmp2);
				pictureBox2->Image = _bmp2;
				

				if( 0 < index)
				{
					textBox1->Text = String::Format("{0}",index);
				}


				//CLROpenCVs::CLROpenCV::CVImageToBitmap_Gray(_gestureDetect->GetOpticalFlowImage(),_bmp3);
				//pictureBox3->Image = _bmp3;
				


				//pictureBox2->Image->FromHbitmap(_bmp->GetHbitmap());
					//= _bmp;
			 }

			 public : virtual void OnDectect(int index)
					  {
					  
					  }

};
}
