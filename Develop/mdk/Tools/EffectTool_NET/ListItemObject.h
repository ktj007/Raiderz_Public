#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace EffectTool_NET {
	/// <summary>
	/// ParticleListBox에 대한 데이터 오브젝트
	/// </summary>
	public ref class ParticleItemObject : public System::Object
	{
	public:
		System::String ^itemName;
		bool lockState;

		ParticleItemObject(System::String ^_name, bool _lockState)
		{
			itemName = gcnew System::String(_name);
			lockState = _lockState;
		}

		virtual String^ ToString() override
		{
			return itemName;
		}
	};
};