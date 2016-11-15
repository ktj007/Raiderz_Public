#pragma once

#include "MVector4.h"
#include "ToolConfig.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Data;
using namespace System::Diagnostics;

namespace rs3 {
	class RS_API RDeviceD3D;
	class RS_API RTexture;
}

namespace EffectTool_NET {
	public ref class MHelp
	{
	public: 
		static MVector4 ColorToMVector4(Color ^color)
		{
			return MVector4(
				static_cast<float>(color->R/ 255.f),
				static_cast<float>(color->G/ 255.f),
				static_cast<float>(color->B/ 255.f),
				static_cast<float>(color->A/ 255.f));
		}

		static Color^ MVector4ToColor(const MVector4 &color)
		{
			return Color::FromArgb(
				static_cast<BYTE>(color.a*255.f),
				static_cast<BYTE>(color.r*255.f),
				static_cast<BYTE>(color.g*255.f),
				static_cast<BYTE>(color.b*255.f));
		}

		static System::Boolean DoColorDialog(Color ^outColor)
		{
			System::Windows::Forms::ColorDialog^ colorDialog = gcnew System::Windows::Forms::ColorDialog();
			colorDialog->FullOpen = true;

			if ( outColor != nullptr )
				colorDialog->Color = *outColor;

			SetColorDialogCustomColors( colorDialog );

			if(colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				if ( outColor != nullptr )
					*outColor = colorDialog->Color;

				GetColorDialogCustomColors( colorDialog );
				return true;
			}

			GetColorDialogCustomColors( colorDialog );
			return false;
		}

		static System::Drawing::Bitmap^ BitmapFromTexture(rs3::RDeviceD3D *pd3dDevice, rs3::RTexture *pTexture);
		static System::Drawing::Bitmap^ BitmapFromTextureAlpha(rs3::RDeviceD3D *pd3dDevice, rs3::RTexture *pTexture);

	private:
		static void SetColorDialogCustomColors( System::Windows::Forms::ColorDialog^ colorDialog )
		{
			array<int>^ customColors = (array<int>^)System::Array::CreateInstance( System::Int32::typeid, 16 );
			for( int i = 0; i < 16; ++i )
				customColors[i] = CToolConfig::GetInstance().m_customColors[i];
			colorDialog->CustomColors = customColors;
		}

		static void GetColorDialogCustomColors( System::Windows::Forms::ColorDialog^ colorDialog )
		{
			for( int i = 0; i < 16; ++i )
				CToolConfig::GetInstance().m_customColors[i] = colorDialog->CustomColors[i];
		}
	};
}