using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using RSManaged;

namespace CharacterTool
{
	public class Renderer
	{
		// 내부 변수
		private bool m_bDrawGrid = true;


		// 내부 함수 ------------------------------------
		private void RenderGrid()
		{
			float step = 10.0f;
			int cnt = 50;
			uint color = 4094967295;
			MHelper.DrawGrid(new Vector3(0,0,0), step, step, cnt, cnt, color);
			MHelper.DrawGrid(new Vector3(0,0,0), -step, step, cnt, cnt, color);
			MHelper.DrawGrid(new Vector3(0,0,0), step, -step, cnt, cnt, color);
			MHelper.DrawGrid(new Vector3(0,0,0), -step, -step, cnt, cnt, color);		
		}

		private void RenderMesh()
		{
			if (Global.bEmptyMesh == true) return;

			float fTime = DateTime.Now.Ticks * 0.001f;

			MActor actor = Global.actorMgr.GetFast(0);
			if (actor.Valid)
			{
//				actor.Frame(fTime);
//				actor.Render(fTime);
			}
		}

		// 공개 함수 ------------------------------------
		public void Render()
		{
			MFramework.Instance.Device.Clear(0);
			MFramework.Instance.Device.BeginScene();



			MCameraSceneNode camera = MFramework.Instance.SceneManager.GetActiveCamera();

			MFramework.Instance.SceneManager.Update();
			MFramework.Instance.SceneManager.Render();

			if (IsDrawGrid)
			{
				RenderGrid();
			}

			RenderMesh();

			MFramework.Instance.Device.EndScene();
			MFramework.Instance.Device.Flip();
		}

		public void Init()
		{
			MLightSceneNode lightNode = new MLightSceneNode();
			MFramework.Instance.SceneManager.AddChild(lightNode);
			MFramework.Instance.SceneManager.AddLight(lightNode);

			ResetCamera();


			MFramework.Instance.Device.SetCullMode(RCULL.RCULL_CW);
			MFramework.Instance.Device.SetTextureFilter(0, RTEXTUREFILTERTYPE.RTF_LINEAR);
			MFramework.Instance.Device.SetTextureFilter(1, RTEXTUREFILTERTYPE.RTF_LINEAR);
			MFramework.Instance.Device.SetLighting(false);
			MFramework.Instance.Device.SetDepthBias(0, 0);
		}

		// 공개 속성 ------------------------------------
		public bool IsDrawGrid
		{
			get { return m_bDrawGrid; }
			set
			{
				m_bDrawGrid = value;
			}
		}

		public void ResetCamera()
		{
			MSceneNode cameraNode = new MPolarCameraSceneNode();
			if (cameraNode.Valid)
			{
				MPolarCameraSceneNode cam = (cameraNode as MPolarCameraSceneNode);
				

				MFramework.Instance.SceneManager.AddChild(cameraNode);
				MFramework.Instance.SceneManager.SetActiveCamera(cameraNode as MCameraSceneNode);

				Vector3 at = new Vector3(0,0,100);

				RSMPolarCoord coord = cam.Coord;
				coord.fPhi = 0.0f;
				coord.fRadius = 200.0f;
				cam.Coord = coord;

				cam.LookAt = at;

				cam.UpdateCamera();
			}




			//				MCameraSceneNode camera = MFramework.Instance.SceneManager.GetActiveCamera();
			//				if (camera.Valid)
			//				{
			//					Vector3 dir = new Vector3(0, 0, 100);
			//					Vector3 pos = new Vector3(-200,0,100);
			//					dir = dir - pos;
			//					dir.Normalize();
			//
			//					camera.Set(pos, dir, new Vector3(0,0,1));
			//				}

		}
	}
}
