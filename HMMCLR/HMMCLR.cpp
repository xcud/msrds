// HMMCLR.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

#include "../CLROpenCV/MovingDetect.h"



using namespace HMMCLR;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	Application::Run(gcnew CLROpenCV::MovingDetect());
	return 0;
}
