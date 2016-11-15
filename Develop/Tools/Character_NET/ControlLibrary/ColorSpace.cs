using System;
using System.Drawing;
using System.Collections;
using System.Windows.Forms;

namespace Sano.PersonalProjects.ColorPicker.Controls {

	/// <summary>
	/// Abstract base class for the color spaces used in the application.
	/// </summary>

	internal abstract class ColorSpace : UserControl {
		
		// events
		internal event ColorSpaceEventHandler ComponentValueChanged;
		internal event ColorSpaceEventHandler SelectedComponentChanged;

		// member fields
		private ArrayList m_colorSpaceComponents;
		private ColorSpaceComponent m_selectedComponent;
		private bool m_displayRadioButtons;
		private bool m_readOnly;

		/// <summary>
		/// Gets an ArrayList containing the components contained by this color
		/// space.
		/// </summary>

		protected ArrayList ColorSpaceComponents {
			get { return m_colorSpaceComponents; }
		}

		/// <summary>
		/// Gets the currently selected color space component.
		/// </summary>
		
		internal ColorSpaceComponent SelectedComponent {
			get { return m_selectedComponent; }
		}
			
		/// <summary>
		/// Gets or sets a value that indicates whether or not the radio 
		/// buttons in the color space components are displayed.
		/// </summary>

		internal bool DisplayRadioButtons {
			get { return m_displayRadioButtons; }
			set { 
				
				m_displayRadioButtons = value;

				foreach( ColorSpaceComponent csc in m_colorSpaceComponents ) {
					csc.RadioButtonVisible = m_displayRadioButtons;
				}

			}
		}

		/// <summary>
		/// Gets or sets a value that indicates whether or not the contents of
		/// the ColorSpaceComponent can be changed.
		/// </summary>

		internal bool ReadOnly {
			get { return m_readOnly; }
			set {

				m_readOnly = value;

				foreach( ColorSpaceComponent csc in m_colorSpaceComponents ) {
					csc.ReadOnly = m_readOnly;
				}

			}
		
		}
	
		/// <summary>
		/// Constructor.
		/// </summary>

		protected ColorSpace() : base() {
			
			m_colorSpaceComponents = new ArrayList();
			m_displayRadioButtons = true;

		}

		/// <summary>
		/// Handles the ComponentSelected event that the ColorSpaceComponent
		/// raises.
		/// </summary>
		/// <param name="sender">The ColorSpaceComponent that raised the event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>

		protected void ComponentSelected( ColorSpaceComponent sender, EventArgs e ) {
			ChangeCurrentlySelectedComponent( sender );
		}

		/// <summary>
		/// Changes the currently selected color space component.
		/// </summary>
		/// <param name="component">A ColorSpaceComponent representing the 
		/// component that is to be set as the selected component.</param>

		protected void ChangeCurrentlySelectedComponent( ColorSpaceComponent component ) {

			// make sure none of the ColorSpaceComponents are checked.
			ResetComponents();

			component.Selected = true;
			m_selectedComponent = component;
			
			OnSelectedComponentChanged( EventArgs.Empty );

		}

		/// <summary>
		/// Handles the ComponentTextKeyUp event that the ColorSpaceComponent
		/// raises.
		/// </summary>
		/// <param name="sender">The ColorSpaceComponent that raised the event.</param>
		/// <param name="e">An EventArgs containing the event data.</param>
		
		protected void ComponentTextKeyUp( ColorSpaceComponent sender, EventArgs e ) {
			OnComponentValueChanged( e );
		}

		/// <summary>
		/// Sets the selected property of the color space components to false.
		/// </summary>

		internal void ResetComponents() {

			foreach( ColorSpaceComponent csc in m_colorSpaceComponents ) {					
				csc.Selected = false;
			}

		}
		
		/// <summary>
		/// Raises the SelectedComponentChanged event.
		/// </summary>
		/// <param name="e">An EventArgs containing event data.</param>

		protected void OnSelectedComponentChanged( EventArgs e ) {
			
			if ( SelectedComponentChanged != null ) {
				SelectedComponentChanged( this, e );
			}

		}

		/// <summary>
		/// Raises the ComponentValueChanged event.
		/// </summary>
		/// <param name="e">An EventArgs containing event data.</param>

		protected void OnComponentValueChanged( EventArgs e ) {
			
			if ( ComponentValueChanged != null ) {
				ComponentValueChanged( this,  e );
			}

		}
		
		/// <summary>
		/// Sets the default selected component.
		/// </summary>
		
		internal abstract void SetDefaultSelection();

		/// <summary>
		/// Returns a Color object representing the color that the color 
		/// space's coordinates represent.
		/// </summary>
		
		internal abstract Color GetColor();

		/// <summary>
		/// Updates the color space coordinate values.
		/// </summary>
		/// <param name="csStructure">A IColorSpaceStructure object containing
		/// the coordinate values that the color space is to be updated with.</param>

		protected abstract void UpdateValues( IColorSpaceStructure csStructure );

		/// <summary>
		/// Gets or sets the IColorSpace based object that contains the 
		/// coordinate values of the components in the color space.
		/// </summary>
		
		internal abstract IColorSpaceStructure Structure { get; set; }

	} // ColorSpace

} // Sano.PersonalProjects.ColorPicker.Controls
