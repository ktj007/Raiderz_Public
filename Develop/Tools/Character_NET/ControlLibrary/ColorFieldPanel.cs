using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Threading;
using System.Windows.Forms;


// TODO: use Offset() to offset point coordinates instead of subtracting SAMPLE_CIRCLE_RADIUS.

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// Responsible for managing all functionality related to the color field.
	/// </summary>

	internal class ColorFieldPanel : System.Windows.Forms.UserControl {
		
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.Timer cursorTimer;

		private Bitmap m_currentColorBitmap;
		private bool m_isMouseDown;
		private bool m_mousePressedWithinPanel;	
		private char m_component;
		private Color m_color;
		private ColorSpace m_colorSpace;
		private int m_selectedComponentValue;
		private int m_movingPointX;
		private int m_movingPointY;
		private int m_targetPointX;
		private int m_targetPointY;
		private Point m_currentPoint;
		private Point m_tempPoint;

		// constants
		private const int SAMPLE_CIRCLE_RADIUS = 5;


		#region Events and delegates
		
		internal delegate void ColorSelectedHandler( object sender, ColorSelectedEventArgs e );
		internal event ColorSelectedHandler ColorSelected;

		#endregion Events and delegates

		public ColorFieldPanel() {

			InitializeComponent();

			this.SetStyle( ControlStyles.AllPaintingInWmPaint | ControlStyles.DoubleBuffer | ControlStyles.UserPaint, true );
			cursorTimer.Enabled = false;

		}

		private void InvalidateNewAndOldRegions( Point oldPoint, Point newPoint ) {

			Rectangle oldRegion = new Rectangle( 
				oldPoint.X - SAMPLE_CIRCLE_RADIUS,
				oldPoint.Y - SAMPLE_CIRCLE_RADIUS,
				SAMPLE_CIRCLE_RADIUS * 2,
				SAMPLE_CIRCLE_RADIUS * 2 );
				
			// give the region a little buffer
			oldRegion.Inflate( 4, 4 );
			this.Invalidate( oldRegion );

			Rectangle newRegion = new Rectangle(
				newPoint.X - SAMPLE_CIRCLE_RADIUS,
				newPoint.Y - SAMPLE_CIRCLE_RADIUS,
				SAMPLE_CIRCLE_RADIUS * 2, 
				SAMPLE_CIRCLE_RADIUS * 2 );
				
			// give the region a little buffer
			newRegion.Inflate( 4, 4 );				
			this.Invalidate( newRegion );

		}
		
		protected override void OnLoad(EventArgs e) {

			base.OnLoad (e);
			UpdateCurrentColorBitmap( false );
		
		}

		protected override void OnMouseDown( MouseEventArgs e ) {

			m_isMouseDown = true;				
			m_mousePressedWithinPanel = true;
			
			Point newPoint = new Point( e.X , e.Y );
			this.InvalidateNewAndOldRegions( m_currentPoint, newPoint );
			m_currentPoint = newPoint;
			m_tempPoint = m_currentPoint;
	
			ColorSelected( this, new ColorSelectedEventArgs( CalculateSelectedColor( newPoint ) ) ); 
			
		}

		protected override void OnMouseUp(MouseEventArgs e) {				
			m_isMouseDown = false;
		}

		protected override void OnMouseMove(MouseEventArgs e) {

			base.OnMouseMove( e );

			if ( m_isMouseDown ) {

				if ( e.X >= 0 && e.X <= 255 || e.Y >= 0 && e.Y <= 255 ) {

					int x, y = 0;
					
					if ( e.X < 0 ) {
						x = 0;
					} else if ( e.X > 255 ) { 
						x = 255;
					} else {
						x = e.X; 
					}

					if ( e.Y < 0 ) {
						y = 0;
					} else if ( e.Y > 255 ) { 
						y = 255;
					} else {
						y = e.Y; 
					}

					m_movingPointX = m_currentPoint.X;
					m_movingPointY = m_currentPoint.Y;
					m_targetPointX = x;
					m_targetPointY = y;
					Point midPoint = new Point( x, y );

					//InvalidateNewAndOldRegions( m_currentPoint, midPoint );
					this.Invalidate();
					m_currentPoint = midPoint;

					if ( !cursorTimer.Enabled ) {
						cursorTimer.Start();
					}

					//				} else {
					//
				}

			} 

		}
		
		protected override void OnMouseEnter(EventArgs e) {
			this.Cursor = new Cursor( Sano.Utility.Resources.GetFileResource( "ColorFieldPanelCursor.cur" ) );
		}

		protected override void OnMouseLeave(EventArgs e) {
			
			if ( cursorTimer.Enabled ) {
				cursorTimer.Stop();
			}

			Cursor.Show();
			this.ParentForm.Cursor = Cursors.Default;

		}

		protected override void OnPaint(PaintEventArgs e) {
			
			// make sure this painting occurs only during run time.
			if ( !this.DesignMode ) {

				Graphics g = e.Graphics;
				g.DrawImage( m_currentColorBitmap, this.DisplayRectangle );

				int circ = 0; // circ = circumference

//				if ( m_mousePressedWithinPanel ) {

					Color c = m_currentColorBitmap.GetPixel( m_currentPoint.X, m_currentPoint.Y );

					Point circlePoint = new Point( 
						m_currentPoint.X - SAMPLE_CIRCLE_RADIUS,
						m_currentPoint.Y - SAMPLE_CIRCLE_RADIUS );
						
					int average = ( c.R + c.G + c.B ) / 3;
					circ = SAMPLE_CIRCLE_RADIUS * 2; // c = circumference
					
					if ( average > 175 ) {
						g.DrawEllipse( Pens.Black, new Rectangle( circlePoint, new Size( circ, circ ) ) );
					} else {
						g.DrawEllipse( Pens.White, new Rectangle( circlePoint, new Size( circ, circ ) ) );
					}

//				}
				
			}
					
		}

		private void UpdateCurrentColorBitmap( bool resetPreviouslyPickedPoint ) {

			if ( ! m_isMouseDown ) {

				int x = 0;
				int y = 0;
				
				if ( m_currentColorBitmap != null ) {
					m_currentColorBitmap.Dispose();
				}

				if ( m_colorSpace is HsbColorSpace ) {

					HSB hsb = ( HSB ) m_colorSpace.Structure;

					if ( m_component == 'H' ) {

						Color c = m_color;
						if ( c.Equals( Color.FromArgb( 0, 0, 0, 0 ) ) | c.Equals( Color.FromArgb( 0, 0, 0 ) ) ) {
							c = Color.FromArgb( 255, 0, 0 );
						}

						m_currentColorBitmap = ColorRenderingHelper.GetHueColorField( c );

						x = ( int ) Math.Round( hsb.Saturation * 2.55 );
						y = 255 - ( int ) Math.Round( hsb.Brightness * 2.55 );
					
					} else if ( m_component == 'S' ) {

						m_currentColorBitmap = ColorRenderingHelper.GetSaturationColorField( m_selectedComponentValue );						

						x = ( int ) Math.Ceiling( hsb.Hue * ( ( double ) 255 / 360 ) );
						y = ( int ) ( 255 - ( hsb.Brightness * 2.55 ) );
					
					} else if ( m_component == 'B' ) {
						
						m_currentColorBitmap = ColorRenderingHelper.GetBrightnessColorField( m_selectedComponentValue );

						x = ( int ) Math.Ceiling( hsb.Hue * ( ( double ) 255 / 360 ) );
						y = 255 - ( int ) Math.Round( hsb.Saturation * 2.55 );
					
					}

				} else if ( m_colorSpace is RgbColorSpace ) {

					Color c = ( ( RgbColorSpace ) m_colorSpace ).GetColor(); 

					if ( m_component == 'R' ) {
						
						m_currentColorBitmap = ColorRenderingHelper.GetRedColorField( m_selectedComponentValue );						

						x = c.B;
						y = 255 - c.G;
					
					} else if ( m_component == 'G' ) {

						m_currentColorBitmap = ColorRenderingHelper.GetGreenColorField( m_selectedComponentValue );									
					
						x = c.B;
						y = 255 - c.R;
					
					} else if ( m_component == 'B' ) {

						m_currentColorBitmap = ColorRenderingHelper.GetBlueColorField( m_selectedComponentValue );						

						x = c.R;
						y = 255 - c.G;	

					}
				
				}

				if ( resetPreviouslyPickedPoint ) {
					m_currentPoint = new Point( x, y);
				}
			
			}

		}
		
		public void UpdateColor( ColorSpace colorSpace, char component, Color color, bool resetPreviouslyPickedPoint ) {
			
			m_color = color;
			UpdateColor( colorSpace, component, resetPreviouslyPickedPoint );

		}

		public void UpdateColor( ColorSpace colorSpace, char component, int selectedComponentValue, bool resetPreviouslyPickedPoint ) {
			
			m_color = Color.Empty;
			m_selectedComponentValue = selectedComponentValue;
			UpdateColor( colorSpace, component, resetPreviouslyPickedPoint );

		}

		private void UpdateColor( ColorSpace colorSpace, char component, bool resetPreviouslyPickedPoint ) {

			m_colorSpace = colorSpace;
			m_component = component;

			m_mousePressedWithinPanel = resetPreviouslyPickedPoint;
			this.UpdateCurrentColorBitmap( resetPreviouslyPickedPoint );
			this.Invalidate();

		}

		#region Dispose
		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing ) {
			if( disposing ) {
				if(components != null) {
					components.Dispose();
				}
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
			this.cursorTimer = new System.Windows.Forms.Timer(this.components);
			// 
			// cursorTimer
			// 
			this.cursorTimer.Interval = 10;
			this.cursorTimer.Tick += new System.EventHandler(this.cursorTimer_Tick);
			// 
			// ColorFieldPanel
			// 
			this.Name = "ColorFieldPanel";
			this.Size = new System.Drawing.Size(256, 256);

		}
		#endregion

		private void cursorTimer_Tick(object sender, System.EventArgs e) {
		
			if ( m_isMouseDown ) {

				int diffX = m_movingPointX - m_targetPointX;
				int diffY = m_movingPointY - m_targetPointY;
				int nextXValue = ( int ) Math.Round( ( double ) diffX / 2 );
				int nextYValue = ( int ) Math.Round( ( double ) diffY / 2 );
				bool stopTimer = false;

				// X
				if ( nextXValue == 0 ) {
					m_movingPointX = m_targetPointX;
					stopTimer = true;
				} else {
					m_movingPointX += -nextXValue;
				} 

				// Y
				if ( nextYValue == 0 ) {
					m_movingPointY = m_targetPointY;
				
					if ( stopTimer ) {
						cursorTimer.Stop();
					}
					
				} else {
					m_movingPointY += -nextYValue;
				} 

				if ( ColorSelected != null ) {
					ColorSelected( this, new ColorSelectedEventArgs( CalculateSelectedColor( new Point( m_movingPointX, m_movingPointY ) ) ) ); 					
				}

			}

		}

		private Color CalculateSelectedColor( Point p ) {

			IColorSpaceStructure selectedColor = null;
			
			if ( m_colorSpace is HsbColorSpace ) {

				HSB hsb = ( HSB ) m_colorSpace.Structure;

				if ( m_component == 'H' ) {

					int brightness = ( int ) ( ( ( double ) 255 - p.Y ) / 2.55 );
					int saturation = ( int ) ( ( double ) p.X / 2.55 );

					selectedColor = new HSB( hsb.Hue, saturation, brightness );
				
				} else if ( m_component == 'S' ) {

					int hue = ( int ) ( p.X * ( ( double ) 360 / 255 ) );
					int brightness = ( int ) ( ( ( double ) 255 - p.Y ) / 2.55 );

					if ( hue == 360 ) {
						hue = 0;
					}

					selectedColor = new HSB( hue, hsb.Saturation, brightness );
				
				} else if ( m_component == 'B' ) {

					int hue = ( int ) ( p.X * ( ( double ) 360 / 255 ) );
					int saturation = ( int ) ( ( ( double ) 255 - p.Y ) / 2.55 );

					if ( hue == 360 ) {
						hue = 0;
					}
					
					selectedColor = new HSB( hue, saturation, hsb.Brightness );

				}

			} else if ( m_colorSpace is RgbColorSpace ) {

				RGB rgb = ( RGB ) m_colorSpace.Structure; 

				if ( m_component == 'R' ) {					
					selectedColor = new RGB( rgb.Red, 255 - p.Y, p.X );
				} else if ( m_component == 'G' ) {
					selectedColor = new RGB( 255 - p.Y, rgb.Green, p.X );
				} else if ( m_component == 'B' ) {
					selectedColor = new RGB( p.X, 255 - p.Y, rgb.Blue);
				}
			
			}
			
			RGB crgb;
			HSB hsbSelectedColor; 
			if ( ( hsbSelectedColor = selectedColor as HSB ) != null ) {
				crgb = ColorConverter.HsbToRgb( hsbSelectedColor );
			} else {
				crgb = ( RGB ) selectedColor;
			}

			return ColorConverter.RgbToColor( crgb );

		}

	}

}
