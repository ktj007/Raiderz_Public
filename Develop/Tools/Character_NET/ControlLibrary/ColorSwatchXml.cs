
using Sano.Utility;
using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Text;
using System.Xml;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// Reads and writes color swatch data.
	/// </summary>

	internal class ColorSwatchXml {

		/// <summary>
		/// Private constructor to prevent the compiler from automatically 
		/// creating a default public constructor.
		/// </summary>

		private ColorSwatchXml() {}

		/// <summary>
		/// Reads color swatches.
		/// </summary>
		/// <param name="file">The path to the XML file containing the swatch 
		/// data.</param>
		/// <param name="isResourceFile">A boolean value indicating whether or 
		/// not the file is to be loaded from the assembly.</param>
		/// <returns>A list of color swatches.</returns>

		internal static ArrayList ReadSwatches( string file, bool isResourceFile ) {
			
			ArrayList swatches = new ArrayList();
			XmlTextReader xmlReader = null;
			
			try {

				if ( isResourceFile ) {
					xmlReader = new XmlTextReader( Resources.GetFileResource( file ) );
				} else {
					xmlReader = new XmlTextReader( file );
				}

				int r = 0;
				int g = 0;
				int b = 0;
				bool insideColorElement = false;

				while ( xmlReader.Read() ) {
				
					if ( xmlReader.NodeType == XmlNodeType.Element && xmlReader.Name.CompareTo( "color" ) == 0 ) {
						
						r = Int32.Parse( xmlReader.GetAttribute( "red" ).ToString() );
						g = Int32.Parse( xmlReader.GetAttribute( "green" ).ToString() );
						b = Int32.Parse( xmlReader.GetAttribute( "blue" ).ToString() );
						insideColorElement = true;

					} else if ( xmlReader.NodeType == XmlNodeType.Text && insideColorElement ) {

						Color c = Color.FromArgb( r, g, b );
						string description = xmlReader.ReadString();
						swatches.Add( new ColorSwatch( c, description ) );
						insideColorElement = false;

					} 


				}

			} finally {

				if ( xmlReader != null ) {
					xmlReader.Close();
				}

			}

			return swatches;

		}

		/// <summary>
		/// Writes custom color swatches to the file system.
		/// </summary>
		/// <param name="file">The name of the file.</param>
		/// <param name="colors">A list of the custom color 
		/// swatches.</param>
		
		internal static void WriteSwatches( string file, ArrayList colors ) {
			
			XmlTextWriter xmlWriter = null;

			try {
				
				xmlWriter = new XmlTextWriter( file, Encoding.UTF8 );
				xmlWriter.Formatting = Formatting.Indented;

				xmlWriter.WriteStartDocument( false );
				xmlWriter.WriteStartElement( "swatches" );
				xmlWriter.WriteStartElement( "swatch" );
				xmlWriter.WriteAttributeString( "id", "CustomSwatches" );
				xmlWriter.WriteStartElement( "colors" );
				
				foreach( ColorSwatch cs in colors ) {
					
					xmlWriter.WriteStartElement( "color" );
					xmlWriter.WriteAttributeString( "red", cs.Color.R.ToString() );
					xmlWriter.WriteAttributeString( "green", cs.Color.G.ToString() );
					xmlWriter.WriteAttributeString( "blue", cs.Color.B.ToString() );
					xmlWriter.WriteString( cs.Description );
					xmlWriter.WriteEndElement();

				}

				xmlWriter.WriteEndElement();
				xmlWriter.WriteEndElement();
				xmlWriter.WriteEndElement();

				xmlWriter.WriteEndDocument();

			} catch ( IOException ) {
				throw;
			} finally {

				if ( xmlWriter != null ) {
					xmlWriter.Close();
				}
			
			}

		}

	} // ColorSwatchXml

} // Sano.PersonalProjects.ColorPicker.Controls
