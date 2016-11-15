using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;


namespace CharacterTool
{
	/// <summary>
	/// CharProperty에 대한 요약 설명입니다.
	/// </summary>
	public class CharProperty
	{
		public CharProperty() { }

		private string		_이름;
		private int			_Age;
		private string		_Ani;
		private Color		_색깔;

		[Category("기본 정보"), Description("이름을 적으세요.")]

		public string 이름
		{
			get { return _이름; }
			set { _이름 = value; }
		}

		[Category("기본 정보"), Description("나이를 적으세요.")]

		public int 나이
		{
			get { return _Age; }
			set { _Age = value; }
		}

		[Category("애니메이션"), Description("에니메이션 목록.")]
		public string 애니메이션
		{
			get { return _Ani; }
			set { _Ani = value; }
		}

		[Category("애니메이션"), Description("색깔 입력하세요.")]
		public Color 색깔
		{
			get { return _색깔; }
			set { _색깔 = value; }
		}

	}
}
