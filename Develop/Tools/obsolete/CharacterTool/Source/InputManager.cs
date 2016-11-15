using System;

namespace CharacterTool
{
	/// <summary>
	/// InputManager에 대한 요약 설명입니다.
	/// </summary>
	public class InputManager
	{
		#region 멤버 변수
		private static InputManager m_pkThis = null;
		#endregion

		public InputManager()
		{

		
		}

		public static InputManager Instance
		{
			get
			{
				if (m_pkThis == null)
				{
					m_pkThis = new InputManager();
				}
				return m_pkThis;
			}
		}

	}
}
