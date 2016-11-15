using System.Reflection;
using System.Runtime.CompilerServices;

//
// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly: AssemblyTitle("")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("")]
[assembly: AssemblyCopyright("")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]		

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version 
//      Build Number
//      Revision
//
// You can specify all the values or you can default the Revision and Build Numbers 
// by using the '*' as shown below:

[assembly: AssemblyVersion("1.2.1.0")]

// REVISION HISTORY
// 01-Aug-2003, Version 1.0.0.0
// First released.
// 
// 20-Aug-2003, Version 1.1.0.0
// 1. FIX: Workaround the NativeWindow.AssignHandle/ReleaseHandle bug in .Net Framework 1.1 by using Win32 API.
// 2. FIX: Assign null to ContentWindow.ActiveWindow now throws an exception when there is visible content.
// 3. FIX: Persist IsHidden property into XML file for Content objects.
//
// 02-Sep-2003, Version 1.2.0.0
// 1. FIX (Suggestion from Roger J): Reduce the screen drag-and-drop flickers. Add old status for class DropTarget,
//    and testing code for class DragHandler, DockManager and TabWindow. If same as old status, no drag frame
//    needs to be drawn.
// 2. CHG: Change the design time property category to "Docking". Modifications made in string resource file.
// 3. NEW (suggestion from israeli0): Add CloseButton property to class Content.
// 4. NEW: Add the properties: DockManager.AllowRedocking, FloatWindow.AllowRedocking, ContentWindow.AllowRedocking
//    and Content.AllowRedocking.

// 09-Sep-2003, Version 1.2.1.0
// 1. FIX (reported by adrian73): In DockWindow.OnAutoHide, assign a default value (400) in case of 
//    SystemParametersInfo API returns 0 for SPI_GETMOUSEHOVERTIME. The following setting value "0" to
//    Timer.Interval will throw an exception.
// 2. FIX (reported by Duane Hanes): Not properly dispose of ContentWindow
//    (1) In ContentWindow.Dispose(bool disposing), the Contents object needs to be disposed first;
//    (2) In ContentCollection.Dispose(), call Content.Dispose() instead of Content.Close().
// 3. FIX (reported by Chris Pels): AutoHide window position calculated incorrectly when DockManager control
//    is not at position (0, 0). In DockManager.GetTabStripRectangle and DockManager.OnPaint, use Width/Height
//    instead of Right/Bottom.


// In order to sign your assembly you must specify a key to use. Refer to the 
// Microsoft .NET Framework documentation for more information on assembly signing.
//
// Use the attributes below to control which key is used for signing. 
//
// Notes: 
//   (*) If no key is specified, the assembly is not signed.
//   (*) KeyName refers to a key that has been installed in the Crypto Service
//       Provider (CSP) on your machine. KeyFile refers to a file which contains
//       a key.
//   (*) If the KeyFile and the KeyName values are both specified, the 
//       following processing occurs:
//       (1) If the KeyName can be found in the CSP, that key is used.
//       (2) If the KeyName does not exist and the KeyFile does exist, the key 
//           in the KeyFile is installed into the CSP and used.
//   (*) In order to create a KeyFile, you can use the sn.exe (Strong Name) utility.
//       When specifying the KeyFile, the location of the KeyFile should be
//       relative to the project output directory which is
//       %Project Directory%\obj\<configuration>. For example, if your KeyFile is
//       located in the project directory, you would specify the AssemblyKeyFile 
//       attribute as [assembly: AssemblyKeyFile("..\\..\\mykey.snk")]
//   (*) Delay Signing is an advanced option - see the Microsoft .NET Framework
//       documentation for more information on this.
//
[assembly: AssemblyDelaySign(false)]
[assembly: AssemblyKeyFile("")]
[assembly: AssemblyKeyName("")]
