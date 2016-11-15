using System;
using System.Drawing;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Sano.Utility {

	// This class provides functionality that assists with resource management.

	public sealed class Resources {
		
		/// <summary>
		/// Private constructor to prevent the compiler from automatically 
		/// creating a default public constructor.
		/// </summary>

		private Resources() {}

		/// <summary>
		/// Retrieves bitmap resource from assembly.
		/// </summary>
		/// <param name="resourceFile">The name of the resource file that is 
		/// to be retrieved.</param>
		/// <returns>Embedded bitmap object.</returns>
	
		public static Bitmap GetBitmapResource( string resourceFile ) {

			Bitmap bitmapResource = null;

			if ( resourceFile != null & resourceFile.Length > 0 ) {
				
				Stream resourceStream = GetResourceStream( resourceFile );
				bitmapResource = ( Bitmap ) Image.FromStream( resourceStream );
				resourceStream.Close();

			} else {
				throw new ArgumentNullException( "resourceFile", "resourceFile cannot be empty" );
			}

			return bitmapResource;
		
		}

		/// <summary>
		/// Retrieves icon resource from assembly.
		/// </summary>
		/// <param name="resourceFile">The name of the icon file.</param>
		/// <returns>Embedded icon object.</returns>
		
		public static Icon GetIconResource( string resourceFile ) {

			Icon iconResource = null;

			if ( resourceFile != null & resourceFile.Length > 0 ) {
				
				Stream resourceStream = GetResourceStream( resourceFile );
				iconResource = new Icon( resourceStream );
				resourceStream.Close();

			} else {
				throw new ArgumentNullException( "resourceFile", "resourceFile cannot be empty." );
			}

			return iconResource;

		}

		/// <summary>
		/// Retrieves a file resource stream from assembly.
		/// </summary>
		/// <param name="resourceFile">The name of the resource file that is 
		/// to be retrieved.</param>
		/// <returns>A stream containing the contents of the file.</returns>
		
		public static Stream GetFileResource( string resourceFile ) {
			
			Stream fileResource = null;

			if ( resourceFile != null & resourceFile.Length > 0 ) {
				fileResource = GetResourceStream( resourceFile );
			}

			return fileResource;

		}

		/// <summary>
		/// Concatenates resource namespace and file name. Checks to make sure 
		/// that the namespace is not empty before appending the resource file
		/// name.
		/// </summary>
		/// <param name="resourceNamespace">A valid string representing the 
		/// namespace in which the resource resides.</param>
		/// <param name="resourceFile">A valid string representing the file 
		/// name of the resource.</param>
		/// <returns>A concatenated resource path.</returns>
		
		private static string CreateFullResourcePath( string resourceNamespace, string resourceFile ) {
			
			string resourceLocation = String.Empty;

			if ( resourceNamespace != null && resourceNamespace.Length > 0 ) {
				resourceLocation = String.Format( "{0}.{1}", resourceNamespace, resourceFile );	
			} else {
				resourceLocation = resourceFile;
			}

			return resourceLocation;

		}

		
		/// <summary>
		/// Returns resource stream.
		/// </summary>
		/// <param name="resourceFile">A valid string representing the file 
		/// name of the resource.</param>
		/// <returns>A stream representing the resource.</returns>
		
		private static Stream GetResourceStream( string resourceFile ) {

			StackTrace trace = new StackTrace();
			StackFrame parentFrame = trace.GetFrame( 2 );
			MethodBase method = parentFrame.GetMethod();			
			Type parentType = method.DeclaringType;
			Assembly parentAssembly = parentType.Assembly;
				
			return parentAssembly.GetManifestResourceStream( CreateFullResourcePath( parentType.Namespace, resourceFile ) );

		}

	
	} // Resources

} // Sano.Utility
