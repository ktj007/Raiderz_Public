using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using RSManaged;

namespace CharacterTool
{
	public class Global
	{
		public static MainForm			mainform;

		public static MActor			actor			= new MActor();
		public static Renderer			renderer		= new Renderer();
		public static Model				model			= new Model();

		public static MMeshMgr			meshMgr			= new MMeshMgr();
		public static MMeshMgr			weaponMeshMgr	= new MMeshMgr();
		public static MAnimationMgr		aniMgr			= new MAnimationMgr();
		public static MActorMgr			actorMgr		= new MActorMgr();


		// ¼³Á¤°ª
		public static bool bEmptyMesh = true;
	}

	public class OutputText
	{
		public static void Print(System.String text)
		{
			Global.mainform.m_OutputForm.tbOutput.AppendText(text);
			Global.mainform.m_OutputForm.tbOutput.AppendText("\n");
		}
		public static void Clear()
		{

		}
	};

}
