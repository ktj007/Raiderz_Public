
using Sano.Utility;
using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	internal class ColorSwatchPanel : System.Windows.Forms.Panel {

		private System.ComponentModel.IContainer components;
		
		// events
		internal event ColorSwatchSelectedHandler ColorSwatchSelected;  

		// controls
		private ToolTip colorTip;
		private DragForm dragFormSwatch;
		private ContextMenu contextMenu;
		private MenuItem renameSwatchMenuItem;
		private MenuItem deleteSwatchMenuItem;
		private MenuItem addNewSwatchMenuItem;

		// readonly members
		private readonly Rectangle m_swatchRegion = new Rectangle( 0, 0, 164, 263 );
		private readonly string m_customSwatchesFile = "CustomSwatches.xml";

		// constants
		private const int PADDING = 2;
		private const int SWATCH_WIDTH = 10;
		private const int SWATCH_HEIGHT = 10;
		private const int OUTER_PADDING = 6;

		// member fields
		private ArrayList m_swatches;
		private bool m_paintActiveEmptySwatch;
		private Bitmap m_swatchBitmap;
		private ColorSwatch[,] m_swatchArray;	
		private ColorSwatch m_lastSwatch;
		private int m_startX; 
		private int m_startY; 
		private int m_swatchOuterRegionWidth;
		private int m_swatchOuterRegionHeight;
		private int m_totalNumberOfSwatches;
		private int m_numberOfEmptySwatches;
		private int m_swatchRowNum;
		private int m_swatchColumnNum;
		private Point m_nextEmptySwatchPoint;

		/// <summary>
		/// Constructor. 
		/// </summary>

		internal ColorSwatchPanel() {
			
			InitializeComponent();

			m_startX = m_swatchRegion.X + OUTER_PADDING;
			m_startY = m_swatchRegion.Y + OUTER_PADDING;
			this.colorTip.SetToolTip( this, "blah" );	

			dragFormSwatch = new DragForm();

			this.AllowDrop = true;

		} 

		// TODO: get rid of this.

		protected override void OnLayout(LayoutEventArgs levent) {

			base.OnLayout (levent);

			if ( File.Exists( m_customSwatchesFile ) ) {				
				m_swatches = ColorSwatchXml.ReadSwatches( m_customSwatchesFile, false );
			} else {

				CreateCustomSwatchesFile();			
				m_swatches = ColorSwatchXml.ReadSwatches( "ColorSwatches.xml", true );

			}

			m_swatchOuterRegionWidth = this.Width - ( ( this.Width - ( 2 * OUTER_PADDING ) ) ) % ( SWATCH_WIDTH + PADDING );
			m_swatchOuterRegionHeight = this.Height - ( ( this.Height - ( 2 * OUTER_PADDING ) ) ) % ( SWATCH_HEIGHT + PADDING );

			int horizSwatches = (  m_swatchOuterRegionWidth - ( 2 * OUTER_PADDING ) ) / ( SWATCH_WIDTH + PADDING );
			int vertSwatches = ( m_swatchOuterRegionHeight - ( 2 * OUTER_PADDING ) ) / ( SWATCH_HEIGHT + PADDING );
			
			m_swatchArray = new ColorSwatch[ horizSwatches, vertSwatches ];

		}

		private void CreateCustomSwatchesFile() {

			StreamReader fileStreamRead = new StreamReader( Resources.GetFileResource( "ColorSwatches.xml" ) );
				
			if ( fileStreamRead != null ) {

				StreamWriter fileStreamWrite = null;
					
				try {
		
					fileStreamWrite = new StreamWriter( m_customSwatchesFile );
					fileStreamWrite.Write( fileStreamRead.ReadToEnd() );
					fileStreamWrite.Flush();
	
				} finally {
						
					fileStreamRead.Close();

					if ( fileStreamWrite != null ) {
						fileStreamWrite.Close();
					}

				}

			}

		}

		private Bitmap BuildSwatchBitmap() {

			int x = m_startX;
			int y = m_startY;			
			bool isFull = false;

			int swatchColumnNum = 0;
			int swatchRowNum = 0;
			
			ColorSwatch[] swatches = ( ColorSwatch[] ) m_swatches.ToArray( typeof( ColorSwatch ) );

			Bitmap swatchBitmap = new Bitmap( m_swatchOuterRegionWidth, m_swatchOuterRegionHeight );

			using ( Graphics g = Graphics.FromImage( swatchBitmap ) ) {

				for ( int i=0; i < swatches.Length; i++ ) {
					
					swatches[i].Location = new Point( x, y );
					swatches[i].Size = new Size( SWATCH_WIDTH, SWATCH_HEIGHT );

					m_swatchArray[ swatchColumnNum++, swatchRowNum ] = swatches[ i ];

					using ( SolidBrush sb = new SolidBrush( swatches[i].Color ) ) {
						g.FillRectangle( sb, x, y, 10, 10 );
					}
					
					g.DrawRectangle( Pens.Black, x, y, 10, 10 );

					UpdatePositions( ref x, ref y, ref swatchColumnNum, ref swatchRowNum );
				
					if ( y + SWATCH_HEIGHT > m_swatchOuterRegionHeight ) {
						isFull = true;
						break;
					}

					m_totalNumberOfSwatches++;
			
				}

				m_swatchRowNum = swatchRowNum;
				m_swatchColumnNum = swatchColumnNum;

				if ( !isFull ) {
					
					m_numberOfEmptySwatches = 0;

					while ( true ) {
						
						if ( ++m_numberOfEmptySwatches == 1 ) {
							m_nextEmptySwatchPoint = new Point( x, y );
						}
											
						g.DrawRectangle( Pens.DarkGray, x, y, 10, 10 );
						UpdatePositions( ref x, ref y, ref swatchColumnNum, ref swatchRowNum );
						
						if ( y + SWATCH_HEIGHT > m_swatchOuterRegionHeight ) {
							isFull = true;
							break;
						}

						m_totalNumberOfSwatches++;


					}

				}
			
			}

			return swatchBitmap;

		}

		private void UpdatePositions( ref int x, ref int y, ref int swatchColumnNum, ref int swatchRowNum ) {
			
			if ( x + 2 * ( SWATCH_WIDTH + PADDING ) > m_swatchOuterRegionWidth ) {

				x = m_startX;
				y += SWATCH_HEIGHT + PADDING;

				swatchColumnNum = 0;
				swatchRowNum++;

			} else {
				x += SWATCH_WIDTH + PADDING;
			}
	
		}
	
		protected override void OnPaint(PaintEventArgs pe) {

			ControlPaint.DrawBorder3D( pe.Graphics, new Rectangle( 0, 0, m_swatchOuterRegionWidth, m_swatchOuterRegionHeight ) );
		
			if ( m_swatchBitmap == null ) {
				m_swatchBitmap = BuildSwatchBitmap();
			}
			pe.Graphics.DrawImage( m_swatchBitmap, new Rectangle( new Point( 0, 0 ), new Size( m_swatchBitmap.Width, m_swatchBitmap.Height ) ) );
				
			if ( m_paintActiveEmptySwatch ) {

				Rectangle rect = new Rectangle( m_nextEmptySwatchPoint, new Size( SWATCH_WIDTH, SWATCH_HEIGHT ) );
				pe.Graphics.DrawRectangle( Pens.Yellow, rect );
			
			}
		
		}

		protected override void OnMouseMove( MouseEventArgs e ) {
			
			int width = m_swatchOuterRegionWidth - ( 2 * OUTER_PADDING );
			int height = m_swatchOuterRegionHeight - ( 2 * OUTER_PADDING );
			int x = e.X - OUTER_PADDING;
			int y = e.Y - OUTER_PADDING;

			Rectangle r = new Rectangle( m_startX, m_startY, width, height );
			Rectangle c = new Rectangle( e.X, e.Y, 1, 1 );

			if ( c.IntersectsWith( r ) ) {

				int swatchColumnIndex = ( x / ( SWATCH_WIDTH + PADDING ) );
				int swatchRowIndex = ( y / ( SWATCH_HEIGHT + PADDING ) );

				ColorSwatch potentialSwatch = m_swatchArray[ swatchColumnIndex, swatchRowIndex ];
				Rectangle potentialSwatchRectangle = new Rectangle( potentialSwatch.Location, potentialSwatch.Size );
				Point cursorPoint = new Point( e.X, e.Y );
				Rectangle cursorRectangle = new Rectangle( cursorPoint, new Size( 1, 1 ) );
				
				if ( cursorRectangle.IntersectsWith( potentialSwatchRectangle ) ) { 

					// hides the tooltip when moving from swatch to swatch.
					if ( !m_lastSwatch.Equals( potentialSwatch ) ) {
						this.colorTip.Active = false;
					}

					if ( !potentialSwatch.Description.Equals( colorTip.GetToolTip( this ) ) ) {
						this.colorTip.SetToolTip( this, potentialSwatch.Description );
					}

					this.colorTip.Active = true;
					m_lastSwatch = potentialSwatch;
					this.Cursor = Cursors.Hand;
				
				} else {

					this.Cursor = Cursors.Default;
					this.colorTip.SetToolTip( this, "" );
					this.colorTip.Active = false;

				
				}

			}

		}

		protected override void OnMouseUp(MouseEventArgs e) {

			int width = m_swatchOuterRegionWidth - ( 2 * OUTER_PADDING );
			int height = m_swatchOuterRegionHeight - ( 2 * OUTER_PADDING );
			int x = e.X - OUTER_PADDING;
			int y = e.Y - OUTER_PADDING;

			Rectangle r = new Rectangle( m_startX, m_startY, width, height );
			Rectangle c = new Rectangle( e.X, e.Y, 1, 1 );

			if ( c.IntersectsWith( r ) ) {

				int swatchColumnIndex = ( x / ( SWATCH_WIDTH + PADDING ) );
				int swatchRowIndex = ( y / ( SWATCH_HEIGHT + PADDING ) );

				ColorSwatch potentialSwatch = m_swatchArray[ swatchColumnIndex, swatchRowIndex ];
				Rectangle potentialSwatchRectangle = new Rectangle( potentialSwatch.Location, potentialSwatch.Size );
				Point cursorPoint = new Point( e.X, e.Y );
				Rectangle cursorRectangle = new Rectangle( cursorPoint, new Size( 1, 1 ) );
				
				if ( cursorRectangle.IntersectsWith( potentialSwatchRectangle ) ) {
					
					if ( ColorSwatchSelected != null ) {
						ColorSwatchSelected( this, new ColorSelectedEventArgs( potentialSwatch.Color ) );
					}

				}

			} 
				
		}
		
		protected override void OnDragEnter(DragEventArgs drgevent) {
			
			ToggleEmptySwatchState( true );
			base.OnDragEnter (drgevent);
		
		}

		protected override void OnDragLeave(EventArgs e) {
			ToggleEmptySwatchState( false );
			base.OnDragLeave (e);
		}


		protected override void OnDragOver(DragEventArgs e) {
		
			e.Effect = DragDropEffects.Move;
			base.OnDragOver( e );

		} 

		protected override void OnDragDrop(DragEventArgs e) {
			
			Color c = ( Color ) e.Data.GetData( typeof( Color ) );
			AddColor( c );
			e.Effect = DragDropEffects.None;
				
			base.OnDragDrop( e );

		}

		private void AddColor( Color c ) {

			bool writeSwatchesToFile = false;
			Point nextEmptySwatchPoint = m_nextEmptySwatchPoint;

			if ( DoesColorAlreadyExist( c ) ) {
				MessageBox.Show( this.Parent, "Color already exists.", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error );	
			} else if ( m_numberOfEmptySwatches <= 0 ) {
				MessageBox.Show( this.Parent, "There are no empty swatches available.", "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error );	
			} else {

				using( AddNewColorSwatchForm colorForm = new AddNewColorSwatchForm( c ) ) {
				
					colorForm.StartPosition = FormStartPosition.CenterParent;
					colorForm.ShowInTaskbar = false;
				
					if ( colorForm.ShowDialog() == DialogResult.OK ) {

						if ( m_swatchBitmap != null ) {
								
							int x = m_nextEmptySwatchPoint.X;
							int y = m_nextEmptySwatchPoint.Y;
					
							using ( Graphics g = Graphics.FromImage( m_swatchBitmap ) ) {
									
								using ( SolidBrush sb = new SolidBrush( c ) ) {
									g.FillRectangle( sb, x, y, 10, 10 );
								}
											
								g.DrawRectangle( Pens.Black, x, y, 10, 10 );
									
							}
					
							m_numberOfEmptySwatches--;

							ColorSwatch cs = new ColorSwatch( 
								c, 
								colorForm.ColorDescription, 
								new Point( x, y ), 
								new Size( SWATCH_WIDTH, SWATCH_HEIGHT ) );
							
							m_swatchArray[ m_swatchColumnNum++, m_swatchRowNum ] = cs;
							m_swatches.Add( cs );
							writeSwatchesToFile = true;
						
							UpdatePositions( ref x, ref y, ref m_swatchColumnNum, ref m_swatchRowNum );
							m_nextEmptySwatchPoint = new Point( x, y );
			
						}
					
					} 

				}
			
			}

			if ( writeSwatchesToFile ) {
				ColorSwatchXml.WriteSwatches( m_customSwatchesFile, m_swatches );
			}

			m_paintActiveEmptySwatch = false;
			this.InvalidateEmptySwatch( nextEmptySwatchPoint );
		
		}

		private bool DoesColorAlreadyExist( Color c ) {
			
			bool doesColorAlreadyExist = false;

			ColorSwatch[] swatches = ( ColorSwatch[] ) m_swatches.ToArray( typeof( ColorSwatch ) );

			for ( int i=0; i < swatches.Length; i++ ) {

				if ( swatches[ i ].Color.Equals( c ) ) {
					
					doesColorAlreadyExist = true;		
					break;

				}

			}

			return doesColorAlreadyExist;

		}

		private void ToggleEmptySwatchState( bool isActive ) {
			
			if ( m_numberOfEmptySwatches > 0 ) {
		
				m_paintActiveEmptySwatch = isActive;
				InvalidateEmptySwatch();

			} else {
				m_paintActiveEmptySwatch = false;
			}
		
		}

		private void InvalidateEmptySwatch() {
			InvalidateEmptySwatch( m_nextEmptySwatchPoint );
		}

		private void InvalidateEmptySwatch( Point swatchPoint ) {
			
			Rectangle invalidationRegion = new Rectangle( swatchPoint, new Size( SWATCH_WIDTH, SWATCH_HEIGHT ) );
			invalidationRegion.Inflate( 1, 1 );
			
			this.Invalidate( invalidationRegion );

		}

		#region Dispose

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing ) {
			if( disposing ) {
				if( components != null )
					components.Dispose();
			}
			base.Dispose( disposing );
		}

		#endregion Dispose

		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent() {
			
			this.components = new System.ComponentModel.Container();
			this.colorTip = new System.Windows.Forms.ToolTip(this.components);
			this.addNewSwatchMenuItem = new MenuItem( "Add new swatch" );
			this.deleteSwatchMenuItem = new MenuItem( "Delete swatch" );
			this.renameSwatchMenuItem = new MenuItem( "Rename swatch" );
			this.contextMenu = new System.Windows.Forms.ContextMenu( new MenuItem[] { addNewSwatchMenuItem, renameSwatchMenuItem, deleteSwatchMenuItem } );
			// 
			// colorTip
			// 
			this.colorTip.Active = false;

		}
		#endregion

	} // ColorSwatchPanel

} // Sano.PersonalProjects.ColorPicker.Controls
