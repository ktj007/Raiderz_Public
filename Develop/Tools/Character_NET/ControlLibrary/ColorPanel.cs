using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

using System.Diagnostics;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	// TODO: change data types of RGB from int16 to byte
	
	[ComVisible(false)]
	public class ColorPanel : UserControl {

		private IContainer components;

		// controls
		private ColorFieldPanel colorFieldPanel;
		private ColorSwatchPanel colorSwatchPanel;
		private DragForm dragForm;
		private HexTextBox hexTextBox;
		private HsbColorSpace hsbColorSpace;
		private Label hexLabel;
		public PictureBox picCurrentColor;
		private PictureBox picColorSlider;
		private RgbColorSpace rgbColorSpace;
		private System.Windows.Forms.Timer sliderTimer;	
		
		// member fields
		private bool m_isLeftMouseButtonDown;
		private bool m_isLeftMouseButtonDownAndMoving;
		private int m_currentColorSliderArrowYLocation;
		private int m_currentYValue;
		private int m_oldValue;
		private int m_targetYValue;
        private int m_HueValue;
        private Rectangle m_selectedColorRegion;
		private Rectangle m_selectedColorColorRegion;
		private Bitmap m_colorSliderBitmap = new Bitmap( 18, 256 );
		private Rectangle m_leftColorSliderArrowRegion;
		private Rectangle m_rightColorSliderArrowRegion;
		private ColorSpace m_currentColorSpace;
		private BitVector32 m_panelState;

		// rectangles for drawing.
		private readonly Rectangle m_colorSliderOuterRegion = new Rectangle( 296, 6, 21, 259 );
		private readonly Rectangle m_colorSliderInnerRegion = new Rectangle( 298, 8, 18, 256 );
		private readonly Rectangle m_colorFieldOuterRegion = new Rectangle( 12, 4, 265, 265 );
		private readonly Rectangle m_colorFieldInnerRegion = new Rectangle( 15, 7, 257, 257 );
		private readonly Rectangle m_valueFieldOuterRegion = new Rectangle( 295, 4, 27, 265 );
		private readonly Rectangle m_valueFieldInnerRegion = new Rectangle( 298, 7, 19, 257 );
		private readonly Rectangle m_swatchRegion = new Rectangle( 464, 4, 164, 263 );

		// constants
		private const int PANELSTATE_isLeftMouseDown = 0x1;								// 1
		private const int PANELSTATE_isLeftMouseDownAndMoving = 0x8;					// 8
		private const int ARROW_HEIGHT = 10;
		private const int ARROW_WIDTH = 6;
		
		/// <summary>
		/// Constructor. Initializes all of the components and member fields 
		/// and configures the control for double buffering support.
		/// </summary>

		public ColorPanel() {

			InitializeComponent();
			InitializeCustomComponents();

			m_currentColorSliderArrowYLocation = m_colorSliderOuterRegion.Bottom - 2;
			m_leftColorSliderArrowRegion = this.GetLeftTrianglePointerInvalidationRegion( m_currentColorSliderArrowYLocation );
			m_rightColorSliderArrowRegion = this.GetRightTrianglePointerInvalidationRegion( m_currentColorSliderArrowYLocation );
			m_selectedColorRegion = new Rectangle( 344, 8, 88, 40 );
			m_selectedColorColorRegion = new Rectangle( 346, 10, 85, 37 );
			m_colorSliderBitmap = new Bitmap( 18, 256 );
			m_panelState = new BitVector32( 0 );

			this.SetStyle( ControlStyles.DoubleBuffer | ControlStyles.UserPaint, true );

			this.AllowDrop = true;

		}
		
		/// <summary>
		/// Returns the hex value of the colors in the RGB color space.
		/// </summary>

		private string HexValue {
			get { return rgbColorSpace.ConvertToHex().ToString(); }
		}

		/// <summary>
		/// Overrides the base class' OnLoad method and instantiates a new
		/// DragForm object that will be used to create the visual drag effect
		/// when adding the currently selected color to the color swatch panel.
		/// </summary>
		/// <param name="e">An EventArgs that contains the event data.</param>

		protected override void OnLoad(EventArgs e) {
			
			dragForm = new DragForm();
		
		}

		/// <summary>
		/// Overrides the panel's OnPaint method to performs all of the painting 
		/// operations.
		/// </summary>
		/// <param name="e">A PaintEventArgs that contains the event data.</param>

		protected override void OnPaint(PaintEventArgs e) {

			base.OnPaint (e);

			// using defines a scope at the end of which the graphics object is disposed.
			using ( Graphics g = e.Graphics ) {
								
				Point[] leftTriangle = CreateLeftTrianglePointer( m_currentColorSliderArrowYLocation );
				Point[] rightTriangle = CreateRightTrianglePointer( m_currentColorSliderArrowYLocation ); 

				g.DrawPolygon( Pens.Black, leftTriangle );
				g.DrawPolygon( Pens.Black, rightTriangle );

				ControlPaint.DrawBorder3D( g, m_colorFieldOuterRegion );
				g.DrawRectangle( Pens.Black, m_colorFieldInnerRegion );

				ControlPaint.DrawBorder3D( g, m_valueFieldOuterRegion );
				g.DrawRectangle( Pens.Black, m_valueFieldInnerRegion );

				ControlPaint.DrawBorder3D( g, new Rectangle( 
					picCurrentColor.Location.X - 3,
					picCurrentColor.Location.Y - 3,
					picCurrentColor.Width + 7,
					picCurrentColor.Height + 7 ) );
				
			}
			
		}
		
		/// <summary>
		/// Calculates the points needed to draw the left triangle pointer for
		/// the value strip.
		/// </summary>
		/// <param name="y">Current cursor y-value</param>
		/// <returns>An array of points representing the left triangle pointer 
		/// for the value strip.</returns>

		private Point[] CreateLeftTrianglePointer( int y ) {

			Point[] points = { 
				new Point( m_valueFieldOuterRegion.Left - ARROW_WIDTH - 1, y - ( ARROW_HEIGHT / 2 ) ), 
				new Point( m_valueFieldOuterRegion.Left - 2, y ), 
				new Point( m_valueFieldOuterRegion.Left - ARROW_WIDTH - 1, y + ( ARROW_HEIGHT / 2 ) ) };
			
			return points;
		
		}

		/// <summary>
		/// Calculates the points needed to draw the right triangle pointer for
		/// the color slider.
		/// </summary>
		/// <param name="y">Current cursor y-value</param>
		/// <returns>An array of points representing the right triangle pointer 
		/// for the color slider.</returns>
		
		private Point[] CreateRightTrianglePointer( int y ) {

			Point[] points = { 
				new Point( m_valueFieldOuterRegion.Right - 1 + ARROW_WIDTH, y - ( ARROW_HEIGHT / 2 ) ), 
				new Point( m_valueFieldOuterRegion.Right, y ), 
				new Point( m_valueFieldOuterRegion.Right - 1 + ARROW_WIDTH, y + ( ARROW_HEIGHT / 2 ) ) };
			
			return points;
			
		}
		
		/// <summary>
		/// Determines the color slider left triangle pointer invalidation 
		/// region.
		/// </summary>
		/// <param name="arrowY">Current cursor y-value.</param>
		/// <returns>A rectangle object representing the area to be 
		/// invalidated.</returns>

		private Rectangle GetLeftTrianglePointerInvalidationRegion( int arrowY ) {

			int leftPadding = ARROW_WIDTH + 2;
			int x = m_valueFieldOuterRegion.Left - leftPadding;
			int y = arrowY - ( ARROW_HEIGHT / 2 ) - 1;
			int width = ARROW_WIDTH + 2;
			int height = ARROW_HEIGHT + 3;
			
			return new Rectangle( x, y, width, height );

		}
		
		/// <summary>
		/// Determines the color slider right triangle pointer invalidation 
		/// region.
		/// </summary>
		/// <param name="arrowY">Current cursor y-value</param>
		/// <returns>A rectangle object representing the area to be 
		/// invalidated.</returns>
		
		private Rectangle GetRightTrianglePointerInvalidationRegion( int arrowY ) {

			int x = m_valueFieldOuterRegion.Right;
			int y = arrowY - ( ARROW_HEIGHT / 2 ) - 1;
			int width = ARROW_WIDTH + 2;
			int height = ARROW_HEIGHT + 3;
			
			return new Rectangle( x, y, width, height );

		}

		#region Overridden methods

		protected override void OnMouseMove(MouseEventArgs e) {
			
			if ( m_isLeftMouseButtonDown ) {	
				
				m_isLeftMouseButtonDownAndMoving = true;
				CheckCursorYRegion( e.Y );			
			
			}

			base.OnMouseMove( e );

		}

		protected override void OnMouseDown(MouseEventArgs e) {
			
			base.OnMouseDown( e );

			if ( e.Button == MouseButtons.Left ) {
			
				// TODO: constrict X position. too much flexibility is given there.
				
				m_isLeftMouseButtonDown = true;
				CheckCursorYRegion( e.Y );
				
				UpdateColorPanels( false, false, true );

			}
		
		}

		protected override void OnMouseUp(MouseEventArgs e) {
			
			base.OnMouseUp( e );

			if ( m_isLeftMouseButtonDown ) {
				UpdateColorField( false );
			}

			m_isLeftMouseButtonDown = false;
			m_isLeftMouseButtonDownAndMoving = false;

		}

		#endregion Overridden methods

		/// <summary>
		/// Handles the MouseDown event raised by the picColorSlider object.
		/// When the user clicks on the color slider, the arrow regions are
		/// updated to assume the clicked y-coordinate as their new
		/// vertical position.
		/// </summary>
		/// <param name="sender">The object that raised the event.</param>
		/// <param name="e">A MouseEventArgs containing event data.</param>
		
		private void picColorSlider_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e) {

            if (e.Button == MouseButtons.Left)
            {

                // TODO: constrict X position. too much flexibility is given there.

                m_isLeftMouseButtonDown = true;
                CheckCursorYRegion(e.Y + this.m_colorSliderInnerRegion.Top);

                UpdateColorPanels(false, false, true);

            }
		}

		/// <summary>
		/// Handles the SelectedColorSpaceComponentChanged event raised by the
		/// color spaces. When this occurs, the color slider arrow regions and 
		/// the color panels are updated.
		/// </summary>
		/// <param name="sender">The ColorSpace object that raised the event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>

		private void SelectedColorSpaceComponentChanged( ColorSpace sender, EventArgs e ) {

			if ( sender is RgbColorSpace ) {
				hsbColorSpace.ResetComponents();
			} else if ( sender is HsbColorSpace ) {
				rgbColorSpace.ResetComponents();
			}

			m_currentColorSpace = sender;
			
			UpdateColorSliderArrowRegions();			
			UpdateColorPanels( true, true, true );

		}

		/// <summary>
		/// Handles the ComponentValueChanged event that the ColorSpace raises 
		/// when the value of one of its components is changed by way of a
		/// keyboard user input. The color spaces are synced up and the color
		/// panels updated.
		/// </summary>
		/// <param name="sender">The ColorSpace object that raised the event.</param>
		/// <param name="e">An EventArgs object containing the event data.</param>

		private void ColorSpaceComponentValueChanged( ColorSpace sender, EventArgs e ) {

			if ( sender is RgbColorSpace ) {
                hsbColorSpace.Structure = ColorConverter.RgbToHsb((RGB)rgbColorSpace.Structure, m_HueValue);
			} else if ( sender is HsbColorSpace ) {
				rgbColorSpace.Structure = ColorConverter.HsbToRgb( ( HSB ) hsbColorSpace.Structure ); //, ( 255 - ( this.m_currentColorSliderArrowYLocation - m_colorSliderInnerRegion.Top ) ) );
			}

			UpdateColorSliderArrowRegions();
			UpdateColorPanels( true, true, true );

		}

		/// <summary>
		/// Handles the Tick event raised by the sliderTimer object. This is
		/// used to guarantee fluidity between two different points on the 
		/// color slider when the arrows are being dragged. Without this, the
		/// currently selected color panel and the updated structure values
		/// (RGB/HEX) jump awkwardly between points.
		/// 
		/// This will probably be changed to a threaded solution since FXCop
		/// is complaining about the tick frequency.
		/// </summary>
		/// <param name="sender">The object that raised this event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>

		private void sliderTimer_Tick(object sender, System.EventArgs e) {
					
			int difference = Math.Abs( m_currentYValue - m_targetYValue );												
			int nextValue = ( int ) Math.Round( ( double ) difference / 2 );

			if ( nextValue == 0  ) {
			
				m_currentYValue = m_targetYValue;
				sliderTimer.Stop();

			} else  {

				if ( m_currentYValue < m_targetYValue ) {
					m_currentYValue += nextValue;
				} else {
					m_currentYValue += -nextValue;
				}

			} 
					
			if ( m_currentColorSpace is RgbColorSpace ) {
                hsbColorSpace.Structure = ColorConverter.RgbToHsb((RGB)rgbColorSpace.Structure, m_HueValue);
			} else if ( m_currentColorSpace is HsbColorSpace ) {
				rgbColorSpace.Structure = ColorConverter.HsbToRgb( ( HSB ) hsbColorSpace.Structure ); 
			}
											
			UpdateColorPanels( false, false, true );
								
		}

		/// <summary>
		/// Processes the selected color.
		/// </summary>
		/// <param name="color">A Color object containing the color that was
		/// recently selected.</param>
		
		private void ColorSelected( Color color, bool updateSliderPosition ) {
			
			// make sure the color that was just clicked isn't the color that
			// is currently displayed (performance enhancement).

			if ( !ColorConverter.ColorToRgb( color ).Equals( rgbColorSpace.Structure ) ) {
				
				RGB rgb = ColorConverter.ColorToRgb( color );

				rgbColorSpace.Structure = rgb;
                hsbColorSpace.Structure = ColorConverter.RgbToHsb(rgb, m_HueValue);
				
				if ( updateSliderPosition ) {
					UpdateColorSliderArrowRegions();
				}

				this.UpdateColorPanels( true, true, true );

			}

		}

		private void colorSwatchPanel_ColorSwatchSelected( object sender, ColorSelectedEventArgs e ) {
			ColorSelected( e.Color, true );
		}

		private void colorFieldPanel_ColorSelected( object sender, ColorSelectedEventArgs e ) {
			ColorSelected( e.Color, false );
		}

		#region Helper methods

		private void InitializeCustomComponents() {

			this.SuspendLayout();

			// hexTextBox
			hexTextBox = new HexTextBox();
			hexTextBox.Location = new Point( 376, 231 );
			hexTextBox.Size = new Size( 55, 20 );
			hexTextBox.BorderStyle = BorderStyle.FixedSingle;
			hexTextBox.ReadOnly = false;
			hexTextBox.KeyUp += new KeyEventHandler(hexTextBox_KeyUp);
			hexTextBox.KeyDown += new KeyEventHandler(hexTextBox_KeyDown);
			this.Controls.Add( hexTextBox );

			// colorFieldPanel
			colorFieldPanel = new ColorFieldPanel();
			colorFieldPanel.BackColor = System.Drawing.SystemColors.Desktop;
			colorFieldPanel.Location = new System.Drawing.Point(16, 8);
			colorFieldPanel.Name = "colorFieldPanel";
			colorFieldPanel.Size = new System.Drawing.Size(256, 256);
			colorFieldPanel.TabIndex = 52;
			colorFieldPanel.ColorSelected += new ColorFieldPanel.ColorSelectedHandler(colorFieldPanel_ColorSelected);
			Controls.Add( colorFieldPanel );

			// colorSwatchPanel
			colorSwatchPanel = new ColorSwatchPanel();
			colorSwatchPanel.Location = new System.Drawing.Point(464, 4);
			colorSwatchPanel.Name = "colorSwatchPanel";
			colorSwatchPanel.Size = new System.Drawing.Size(96, 264);
			colorSwatchPanel.TabIndex = 53;
			colorSwatchPanel.ColorSwatchSelected += new ColorSwatchSelectedHandler(colorSwatchPanel_ColorSwatchSelected);
			Controls.Add( colorSwatchPanel );

			// rgbColorSpace
			rgbColorSpace = new RgbColorSpace();
			rgbColorSpace.DisplayRadioButtons = true;
			rgbColorSpace.Location = new System.Drawing.Point(336, 138);
			rgbColorSpace.Name = "rgbColorSpace";
			rgbColorSpace.ReadOnly = false;
			rgbColorSpace.Size = new System.Drawing.Size(112, 96);
			rgbColorSpace.TabIndex = 48;
			rgbColorSpace.ComponentValueChanged += new ColorSpaceEventHandler(ColorSpaceComponentValueChanged);
			rgbColorSpace.SelectedComponentChanged += new ColorSpaceEventHandler(SelectedColorSpaceComponentChanged);
			Controls.Add(rgbColorSpace);

			// hsbColorSpace
			hsbColorSpace = new HsbColorSpace();
			hsbColorSpace.DisplayRadioButtons = true;
			hsbColorSpace.Location = new System.Drawing.Point(336, 58);
			hsbColorSpace.Name = "hsbColorSpace";
			hsbColorSpace.ReadOnly = false;
			hsbColorSpace.Size = new System.Drawing.Size(112, 96);
			hsbColorSpace.TabIndex = 47;
			hsbColorSpace.ComponentValueChanged += new ColorSpaceEventHandler(ColorSpaceComponentValueChanged);
			hsbColorSpace.SelectedComponentChanged += new ColorSpaceEventHandler(SelectedColorSpaceComponentChanged);
			hsbColorSpace.SetDefaultSelection();
			Controls.Add(hsbColorSpace);
			
			this.ResumeLayout( false );

		}

		
		private void CheckCursorYRegion( int y ) {

			int mValue = y;

			if ( m_isLeftMouseButtonDown && !m_isLeftMouseButtonDownAndMoving ) {

				if ( y < m_colorSliderInnerRegion.Top || y > m_colorSliderInnerRegion.Bottom ) {
					return;
				}

			} else if ( m_isLeftMouseButtonDownAndMoving ) {

				if ( y < m_colorSliderInnerRegion.Top ) {
					mValue = m_colorSliderInnerRegion.Top;
				} else if ( y >= m_colorSliderInnerRegion.Bottom ) {
					mValue = m_colorSliderInnerRegion.Bottom - 1;
				} 

			} else {
				return;
			}

			this.m_currentColorSliderArrowYLocation = mValue;

			this.Invalidate( m_leftColorSliderArrowRegion );
			this.Invalidate( m_rightColorSliderArrowRegion );
			
			m_leftColorSliderArrowRegion = this.GetLeftTrianglePointerInvalidationRegion( mValue );
			m_rightColorSliderArrowRegion = this.GetRightTrianglePointerInvalidationRegion( mValue );

			this.Invalidate( m_leftColorSliderArrowRegion );
			this.Invalidate( m_rightColorSliderArrowRegion );
							
			ValueChanged( 255 - ( mValue - m_colorSliderInnerRegion.Top ) );
				
		}


		private void ValueChanged( int newValue ) {
				
			int mValue = newValue;

			switch ( m_currentColorSpace.SelectedComponent.Unit ) {
		
				case ComponentUnit.Percentage:
					mValue = ( int ) Math.Ceiling(  mValue / ( ( ( double ) 255 ) / 100 ) );
					break;
		
				case ComponentUnit.Degree:
					mValue = ( int ) Math.Ceiling( mValue / ( ( ( double ) 255 ) / 360 ) );
					if ( mValue == 360 ) {
						mValue = 0;
					}
					break;
				
			}

			m_currentColorSpace.SelectedComponent.Value = mValue;

			if ( m_oldValue != mValue && m_isLeftMouseButtonDownAndMoving ) {
					
				m_targetYValue = newValue;
				m_currentYValue = m_oldValue;
				m_oldValue = newValue;
                m_HueValue = mValue;
                sliderTimer.Start();				

			} else {
				UpdateColorField( false );
			}

		}

		private void SetCurrentSliderArrowYLocation( double sValue ) {		
			m_currentColorSliderArrowYLocation = ( int )( m_colorSliderInnerRegion.Top + ( 255 - sValue ) );
		}

		private void UpdateColorSliderArrowRegions() {

			this.InvalidateColorSliderArrowRegions();
			
			CalculateValueRegionMapping( this.m_currentColorSpace.SelectedComponent );
			
			m_leftColorSliderArrowRegion = this.GetLeftTrianglePointerInvalidationRegion( m_currentColorSliderArrowYLocation );
			m_rightColorSliderArrowRegion = this.GetRightTrianglePointerInvalidationRegion( m_currentColorSliderArrowYLocation );

			this.InvalidateColorSliderArrowRegions();

		}
		
		/// <summary>
		/// Invalidate the color slider arrow regions.
		/// </summary>

		private void InvalidateColorSliderArrowRegions() {

			this.Invalidate( m_leftColorSliderArrowRegion );
			this.Invalidate( m_rightColorSliderArrowRegion );

		}


		private void CalculateValueRegionMapping( ColorSpaceComponent csc ) {
			
			int mValue = csc.Value;
			
			switch ( csc.Unit ) {
			
				case ComponentUnit.Percentage:
					mValue = ( int ) Math.Ceiling( ( ( ( double ) 255 ) / 100 ) * mValue );
					break;
				
				case ComponentUnit.Degree:
					mValue = ( int ) Math.Ceiling( ( ( ( double ) 255 ) / 360 ) * mValue );
					break;
					
			}

			SetCurrentSliderArrowYLocation( mValue );

		}
		
		/// <summary>
		/// Updates the color panels and the hex value.
		/// </summary>
		/// <param name="updateSlider">A boolean value indicating whether or 
		/// not the color slider should be updated.</param>
		/// <param name="resetPreviouslyPickedPointOnColorField">A boolean 
		/// value indicating whether or not the previously picked point on the 
		/// color field should be reset.</param>
		/// <param name="updateHexValue">A boolean value indicating whether or
		/// not the hex value should be updated.</param>

		private void UpdateColorPanels( bool updateSlider, bool resetPreviouslyPickedPointOnColorField, bool updateHexValue ) {
						
			picCurrentColor.BackColor =	rgbColorSpace.GetColor();
			
			if ( updateSlider ) {
				this.UpdateColorSlider();
			}

			this.UpdateColorField( resetPreviouslyPickedPointOnColorField );
			
			if ( updateHexValue ) {
				this.UpdateHexValue();
			}
			
		}
		
		/// <summary>
		/// Updates the hexadecimal text value.
		/// </summary>

		private void UpdateHexValue() {
			hexTextBox.Text = this.HexValue;
			hexTextBox.SelectionStart = this.HexValue.Length;
		}

		/// <summary>
		/// Updates the color field panel.
		/// </summary>
		/// <param name="resetPreviouslyPickedPoint">A boolean value indicating
		/// whether or not the previously picked point should be reset.</param>

		private void UpdateColorField( bool resetPreviouslyPickedPoint ) {

			int sValue = this.m_currentColorSpace.SelectedComponent.Value;
			char component = m_currentColorSpace.SelectedComponent.DisplayCharacter;
						
			if ( m_currentColorSpace is HsbColorSpace ) {
				
				if ( component == 'H' ) {

					int arrowLocation = m_currentColorSliderArrowYLocation - m_colorSliderInnerRegion.Top;
					Color color = ( ( Bitmap ) picColorSlider.Image ).GetPixel( 10, arrowLocation );
					colorFieldPanel.UpdateColor( m_currentColorSpace, component, color, resetPreviouslyPickedPoint );
				
				} else if ( component == 'S' || component == 'B' ) {
					colorFieldPanel.UpdateColor( m_currentColorSpace, component, sValue, resetPreviouslyPickedPoint );
				}
							
			} else if ( m_currentColorSpace is RgbColorSpace ) {
				colorFieldPanel.UpdateColor( m_currentColorSpace, component, sValue, resetPreviouslyPickedPoint );
			}

		}

		/// <summary>
		/// Updates the color slider.
		/// </summary>

		private void UpdateColorSlider() {

			RGB rgb = ( RGB ) this.rgbColorSpace.Structure;
			HSB hsb = ( HSB ) this.hsbColorSpace.Structure;
			Rectangle region = new Rectangle( 0, -1, 18, 257 );
			
			using ( Graphics g = Graphics.FromImage( m_colorSliderBitmap ) ) {

				if ( this.m_currentColorSpace is RgbColorSpace ) {
						
					char dChar = this.m_currentColorSpace.SelectedComponent.DisplayCharacter;
					int red = rgb.Red;
					int green = rgb.Green;
					int blue = rgb.Blue;
						
					Color startColor;
					Color endColor;

					switch ( dChar ) {
							
						case 'R':
							startColor = Color.FromArgb( 0, green, blue );
							endColor = Color.FromArgb( 255, green, blue );
							break;

						case 'G':
							startColor = Color.FromArgb( red, 0, blue );
							endColor = Color.FromArgb( red, 255, blue );
							break;

						default:
							startColor = Color.FromArgb( red, green, 0 );
							endColor = Color.FromArgb( red, green, 255 );
							break;

					}

					using ( LinearGradientBrush lgb = new LinearGradientBrush( region, startColor, endColor, 270f ) ) {
						g.FillRectangle( lgb, region );
					}
					
				} else if ( this.m_currentColorSpace is HsbColorSpace ) {
					
					if ( this.m_currentColorSpace.SelectedComponent.DisplayCharacter == 'H' ) { 
										
						Rectangle rect = new Rectangle( 0, 0, 20, 256 );

						using ( LinearGradientBrush brBrush = new LinearGradientBrush( rect, Color.Blue, Color.Red, 90f, false ) ) {

							Color[] colorArray = { Color.Red, Color.Magenta, Color.Blue, Color.Cyan, Color.FromArgb( 0, 255, 0 ), Color.Yellow, Color.Red };
							float[] posArray = { 0.0f, 0.1667f, 0.3372f, 0.502f, 0.6686f, 0.8313f, 1.0f };

							ColorBlend colorBlend = new ColorBlend();
							colorBlend.Colors = colorArray;
							colorBlend.Positions = posArray;
							brBrush.InterpolationColors = colorBlend;

							g.FillRectangle( brBrush, region );
								
						}

					} else if ( this.m_currentColorSpace.SelectedComponent.DisplayCharacter == 'B' ) {
							
						RGB sRgb = ColorConverter.HsbToRgb( new HSB( hsb.Hue, hsb.Saturation, 100 ) );
						RGB eRgb = ColorConverter.HsbToRgb( new HSB( hsb.Hue, hsb.Saturation, 0 ) );
							
						using ( LinearGradientBrush lgb = new LinearGradientBrush( region, Color.FromArgb( sRgb.Red, sRgb.Green, sRgb.Blue ), Color.FromArgb( eRgb.Red, eRgb.Green, eRgb.Blue ), 90f ) ) {
							g.FillRectangle( lgb, region );
						}

					} else {

						RGB sRgb = ColorConverter.HsbToRgb( new HSB( hsb.Hue, 100, hsb.Brightness ) );
						RGB eRgb = ColorConverter.HsbToRgb( new HSB( hsb.Hue, 0, hsb.Brightness ) );

						using ( LinearGradientBrush lgb = new LinearGradientBrush( region, Color.FromArgb( sRgb.Red, sRgb.Green, sRgb.Blue ), Color.FromArgb( eRgb.Red, eRgb.Green, eRgb.Blue ), 90f ) ) {
							g.FillRectangle( lgb, region );
						}

					}

				}
			
			}

			this.picColorSlider.Image = m_colorSliderBitmap;

		}

		#endregion Helper methods

		/// <summary>
		/// Handles the MouseDown event raised by the picCurrentColor object.
		/// </summary>

		private void picCurrentColor_MouseDown( object sender, System.Windows.Forms.MouseEventArgs e ) {

			if ( e.Button == MouseButtons.Left ) {

				// update form properties
				dragForm.UpdateLocation( this.PointToScreen( picCurrentColor.Location ) );

				// calculate the difference between the current cursor position and
				// the upper left corner of the picCurrentColor picture box. this
				// allows us to persist the distance between the cursor and the 
				// upper left corner of the drag form as it is being dragged.
				dragForm.CursorXDifference =  Cursor.Position.X - dragForm.Location.X;
				dragForm.CursorYDifference = Cursor.Position.Y - dragForm.Location.Y;

				dragForm.BackColor = picCurrentColor.BackColor;
				dragForm.ChangeSize( picCurrentColor.Size );
				dragForm.ShowForm();

				// initiate drag
				picCurrentColor.DoDragDrop( rgbColorSpace.GetColor(), DragDropEffects.Move );
			
			}
		
		}

		/// <summary>
		/// Handles the KeyUp event raised by the hexTextBox object.
		/// </summary>
		
		private void hexTextBox_KeyUp( object sender, KeyEventArgs e ) {
			UpdateColorSpacesWithNewHexValue();		
		}

		/// <summary>
		/// Handles the KeyDown event raised by the hexTextBox object.
		/// </summary>
		/// <param name="sender"> The object that raised the event.</param>
		/// <param name="e">A KeyEventArgs containing the event data.</param>

		private void hexTextBox_KeyDown(object sender, KeyEventArgs e) {
			UpdateColorSpacesWithNewHexValue();
		}

		/// <summary>
		/// Updates the color spaces with the new hex value and makes sure that
		/// the necessary components are updated (color field, color slider, 
		/// etc).
		/// </summary>

		private void UpdateColorSpacesWithNewHexValue() {

			RGB rgb = ColorConverter.HexToRgb( hexTextBox.Text );							

			rgbColorSpace.Structure = rgb;
            hsbColorSpace.Structure = ColorConverter.RgbToHsb(rgb, m_HueValue);
			
			UpdateColorSliderArrowRegions();

			// reset the location of the arrows in the value region.
			this.UpdateColorPanels( true, true, false );

		}

		/// <summary>
		/// Handles the GetFeedback event raised by picCurrentColor. We don't
		/// want to use the default cursors (I think they are intrusive). 
		/// Setting the UseDefaultCursors property of the GiveFeedbackEventArgs
		/// object and setting the cursor to the hand cursor takes care of this
		/// for us.
		/// </summary>
		/// <param name="sender"> The object that raised the event.</param>
		/// <param name="e">A GiveFeedbackEventArgs containing the event data.</param>
		
		private void picCurrentColor_GiveFeedback(object sender, System.Windows.Forms.GiveFeedbackEventArgs e) {
		
			e.UseDefaultCursors = false;
			Cursor.Current = Cursors.Hand;
		
		}
	
		/// <summary>
		/// Handles the QueryContinueDrag event raised by picCurrentColor. If
		/// the Action is anything other than Cancel or Drop, the location of
		/// the transparent form is updated.
		/// </summary>
		/// <param name="sender">The object that raised the event.</param>
		/// <param name="e">A QueryContinueDragEventArgs containing the event
		/// data.</param>

		private void picCurrentColor_QueryContinueDrag(object sender, System.Windows.Forms.QueryContinueDragEventArgs e) {

			if ( e.Action == DragAction.Cancel || e.Action == DragAction.Drop ) {
				dragForm.Hide();
			} else {
				dragForm.Location = new Point( Cursor.Position.X - dragForm.CursorXDifference, Cursor.Position.Y - dragForm.CursorYDifference );
			}
		
		}

		#region Dispose
		
		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>

		protected override void Dispose( bool disposing ) {

			if( disposing ) {

				if( components != null ) {
					components.Dispose();
				}

				if ( m_colorSliderBitmap != null ) {
					m_colorSliderBitmap.Dispose();
				}

				if ( dragForm != null ) {
					dragForm.Dispose();
				}

			}

			base.Dispose( disposing );
		
		}
		
		#endregion

		#region Component Designer generated code
		
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>

		private void InitializeComponent() {
            this.components = new System.ComponentModel.Container();
            this.picCurrentColor = new System.Windows.Forms.PictureBox();
            this.hexLabel = new System.Windows.Forms.Label();
            this.picColorSlider = new System.Windows.Forms.PictureBox();
            this.sliderTimer = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.picCurrentColor)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picColorSlider)).BeginInit();
            this.SuspendLayout();
            // 
            // picCurrentColor
            // 
            this.picCurrentColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.picCurrentColor.Location = new System.Drawing.Point(352, 7);
            this.picCurrentColor.Name = "picCurrentColor";
            this.picCurrentColor.Size = new System.Drawing.Size(85, 37);
            this.picCurrentColor.TabIndex = 42;
            this.picCurrentColor.TabStop = false;
            this.picCurrentColor.QueryContinueDrag += new System.Windows.Forms.QueryContinueDragEventHandler(this.picCurrentColor_QueryContinueDrag);
            this.picCurrentColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picCurrentColor_MouseDown);
            this.picCurrentColor.GiveFeedback += new System.Windows.Forms.GiveFeedbackEventHandler(this.picCurrentColor_GiveFeedback);
            // 
            // hexLabel
            // 
            this.hexLabel.Location = new System.Drawing.Point(344, 231);
            this.hexLabel.Name = "hexLabel";
            this.hexLabel.Size = new System.Drawing.Size(31, 23);
            this.hexLabel.TabIndex = 40;
            this.hexLabel.Text = "HEX:";
            this.hexLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // picColorSlider
            // 
            this.picColorSlider.Location = new System.Drawing.Point(299, 8);
            this.picColorSlider.Name = "picColorSlider";
            this.picColorSlider.Size = new System.Drawing.Size(18, 256);
            this.picColorSlider.TabIndex = 50;
            this.picColorSlider.TabStop = false;
            this.picColorSlider.MouseMove += new System.Windows.Forms.MouseEventHandler(this.picColorSlider_MouseMove);
            this.picColorSlider.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picColorSlider_MouseDown);
            this.picColorSlider.MouseUp += new System.Windows.Forms.MouseEventHandler(this.picColorSlider_MouseUp);
            // 
            // sliderTimer
            // 
            this.sliderTimer.Interval = 10;
            this.sliderTimer.Tick += new System.EventHandler(this.sliderTimer_Tick);
            // 
            // ColorPanel
            // 
            this.Controls.Add(this.picColorSlider);
            this.Controls.Add(this.hexLabel);
            this.Controls.Add(this.picCurrentColor);
            this.Name = "ColorPanel";
            this.Size = new System.Drawing.Size(584, 272);
            this.Load += new System.EventHandler(this.ColorPanel_Load);
            ((System.ComponentModel.ISupportInitialize)(this.picCurrentColor)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picColorSlider)).EndInit();
            this.ResumeLayout(false);

		}

		#endregion Component Designer generated code

        private void ColorPanel_Load(object sender, EventArgs e)
        {

        }

        private void picColorSlider_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_isLeftMouseButtonDown)
            {

                m_isLeftMouseButtonDownAndMoving = true;
                CheckCursorYRegion(e.Y + this.m_colorSliderInnerRegion.Top);

            }
        }
        private void picColorSlider_MouseUp(object sender, MouseEventArgs e)
        {
            if (m_isLeftMouseButtonDown)
            {
                UpdateColorField(false);
            }

            m_isLeftMouseButtonDown = false;
            m_isLeftMouseButtonDownAndMoving = false;
        }

	} // ColorPanel

} // Sano.PersonalProjects.ColorPicker.Controls
