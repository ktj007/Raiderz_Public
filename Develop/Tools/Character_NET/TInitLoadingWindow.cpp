#include "StdAfx.h"
#include "TInitLoadingWindow.h"


System::Void Character_NET::TInitLoadingWindow::TInitLoadingWindow_Load( System::Object^ sender, System::EventArgs^ e )
{
	this->ControlBox = false;
	String^ path = System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->GetModules()[0]->FullyQualifiedName);

	m_imgInitLoading = gcnew Bitmap(Path::Combine(path,"InitImg.bmp"));

	this->Size = m_imgInitLoading->Size;
	this->CenterToScreen();

	InitLoadingPictureBox->Dock = DockStyle::Fill;
	InitLoadingPictureBox->SizeMode = PictureBoxSizeMode::StretchImage;
	InitLoadingPictureBox->Image = m_imgInitLoading;
	InitLoadingPictureBox->Refresh();

	m_formGraphics				= InitLoadingPictureBox->CreateGraphics();
	m_fontBrush					= gcnew System::Drawing::SolidBrush(System::Drawing::Color::White);
	m_drawFont					= gcnew System::Drawing::Font("±¼¸²", 8);
	m_drawFormat				= gcnew System::Drawing::StringFormat;
	m_drawFormat->LineAlignment	= System::Drawing::StringAlignment::Center;

	m_drawRect = RectangleF(10, m_imgInitLoading->Size.Height - 20 , m_imgInitLoading->Size.Width - 20, 15);

	m_formGraphics->DrawString(gcnew String("Ready..."), m_drawFont, m_fontBrush, m_drawRect, m_drawFormat);
}

System::Void Character_NET::TInitLoadingWindow::InitLoadingInfoText( string strLoadingIndo )
{
	InitLoadingPictureBox->Refresh();

	m_formGraphics->DrawString(String::Format("Loading : {0}", gcnew String(strLoadingIndo.c_str())), m_drawFont, m_fontBrush, m_drawRect, m_drawFormat);
}

System::Void Character_NET::TInitLoadingWindow::InitLoadingPictureBox_Paint( System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e )
{
	InitLoadingPictureBox->Refresh();
	InitLoadingPictureBox->Visible = true;
}