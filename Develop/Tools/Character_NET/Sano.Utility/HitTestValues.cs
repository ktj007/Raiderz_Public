
using System;

namespace Sano.Utility.NativeMethods {

	/// <summary>
	/// Enumeration of HitTest values.
	/// 
	/// See http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/winui/windowsuserinterface/userinput/mouseinput/mouseinputreference/mouseinputmessages/wm_nchittest.asp
	/// for a description of each value.
	/// </summary>

	public struct HitTestValues {

		public const int HTERROR = -2;
		public const int HTTRANSPARENT = -1;
		public const int HTNOWHERE = 0;
		public const int HTCLIENT = 1;
		public const int HTCAPTION = 2;
		public const int HTSYSMENU = 3;
		public const int HTGROWBOX = 4;
		public const int HTMENU = 5;
		public const int HTHSCROLL = 6;
		public const int HTVSCROLL = 7;
		public const int HTMINBUTTON = 8;
		public const int HTMAXBUTTON = 9;
		public const int HTLEFT = 10;
		public const int HTRIGHT = 11;
		public const int HTTOP = 12;
		public const int HTTOPLEFT = 13;
		public const int HTTOPRIGHT = 14;
		public const int HTBOTTOM = 15;
		public const int HTBOTTOMLEFT = 16;
		public const int HTBOTTOMRIGHT = 17;
		public const int HTBORDER = 18;
		public const int HTOBJECT = 19;
		public const int HTCLOSE = 20;
		public const int HTHELP = 21;

	} // HitTestValues

} // Sano.Utility.Win32