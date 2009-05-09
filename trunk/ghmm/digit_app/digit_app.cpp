// digit_app.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace digit_app;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	

	//try
	{
		Application::Run(gcnew Form1());

	}
	//catch(System::IO::FileNotFoundException ^e)
	//{

	//}

	// Create the main window and run it
	
	return 0;
}
