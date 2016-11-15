using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Reflection;
using Configuration;


namespace SoundManager
{
    public partial class MainForm : Form
    {
        private bool changedValue = false;
        private PointF soundPosition = new PointF( 0.0f, 500.0f);
        private float soundPositionScale = 1.0f;
        enum SoundControlPoint { NONE, START, STARTVEL, END, ENDVEL };
        private SoundControlPoint selectedControlPoint = SoundControlPoint.NONE;


        // General functions
        public MainForm()
        {
            InitializeComponent();
        }

        private int getSoundTypeImageIndex( string type)
        {
            if (type == SoundItemProperty.typeSound[0]) return 0;
            else if (type == SoundItemProperty.typeSound[1]) return 1;
            else if (type == SoundItemProperty.typeSound[2]) return 2;
            return -1;
        }
        private void refreshSoundList()
        {
            treeSoundList.BeginUpdate();
            treeSoundList.Nodes.Clear();
            TreeNode treeNode = treeSoundList.Nodes.Add("Sound List");
            treeNode.StateImageIndex = 3;
            treeNode.ForeColor = System.Drawing.Color.FromArgb(0, 0, 255);

            refreshSoundGroupList(treeNode, SoundContainer.Sounds);
            treeNode.Expand();
            treeSoundList.EndUpdate();
        }
        private void refreshSoundGroupList(TreeNode nodeParentTree, SoundGroup soundGroup)
        {
            foreach (SoundNode soundNode in soundGroup.Nodes)
            {
                TreeNode treeNode = nodeParentTree.Nodes.Add(soundNode.Name);
                treeNode.Tag = soundNode;

                refreshSoundItem(treeNode);

                if (soundNode.IsGroup() == true)
                {
                    SoundGroup soundChildGroup = (SoundGroup)soundNode;
                    refreshSoundGroupList(treeNode, soundChildGroup);
                }
            }
        }
        private void refreshSoundItem(TreeNode treeNode)
        {
            if (treeNode == null || treeNode.Tag == null)
                return;


            SoundNode soundNode = (SoundNode)treeNode.Tag;
            treeNode.Text = soundNode.Name;

            if (soundNode.IsGroup() == true)
            {
                treeNode.StateImageIndex = 3;
                treeNode.ForeColor = System.Drawing.Color.FromArgb(0, 0, 255);
            }
            else
            {
                SoundItem soundItem = (SoundItem)soundNode;
                treeNode.StateImageIndex = getSoundTypeImageIndex(soundItem.Type);
                if (soundItem.Filename == "")
                    treeNode.BackColor = System.Drawing.Color.FromArgb(255, 200, 200);
            }
        }
        private void refreshSoundInfos()
        {
            TreeNode selectedNode = treeSoundList.SelectedNode;
            if (selectedNode == null || selectedNode.Tag == null)
            {
                propertySoundInfo.Tag = null;
                propertySoundInfo.SelectedObject = null;

                listSounds.Tag = null;
                listSounds.Items.Clear();
                
                return;
            }

            SoundNode soundNode = (SoundNode)selectedNode.Tag;
            if (soundNode == null)
                return;

            if (soundNode.IsGroup() == true)
            {
                SoundGroup soundGroup = (SoundGroup)soundNode;
                SoundGroupProperty groupSettings = new SoundGroupProperty();
                groupSettings.Name = soundGroup.Name;

                propertySoundInfo.Tag = selectedNode;
                propertySoundInfo.SelectedObject = groupSettings;

                listSounds.Tag = null;
                listSounds.Items.Clear();
            }
            else
            {
                SoundItem soundItem = (SoundItem)soundNode;
                SoundItemProperty soundSettings = new SoundItemProperty();
                soundSettings.Name = soundItem.Name;
                soundSettings.FileName = soundItem.Filename;
                soundSettings.Type = soundItem.Type;
                soundSettings.Volume = soundItem.Volume;
                soundSettings.Loop = soundItem.Loop;
                soundSettings.Unique = soundItem.Unique;
                soundSettings.MinDistance = soundItem.MinDistance;
                soundSettings.MaxDistance = soundItem.MaxDistance;
                soundSettings.Priority = soundItem.Priority;

                propertySoundInfo.Tag = selectedNode;
                propertySoundInfo.SelectedObject = soundSettings;


                int soundTypeImage = getSoundTypeImageIndex(soundItem.Type);
                listSounds.Tag = selectedNode;
                listSounds.Items.Clear();
                string filename = Path.GetFileName(soundItem.Filename);
                if ( filename != string.Empty)
                {
                    ListViewItem listItem = listSounds.Items.Add("*default", soundTypeImage);
                    ListViewItem.ListViewSubItem subListItem = listItem.SubItems.Add(Path.GetFileName(soundItem.Filename));
                    listItem.Selected = true;
                }
            }
        }
        private void refreshSoundPosition()
        {
            pictureSoundPositionInfo.Refresh();
        }
        private void refreshSoundColtrol()
        {
            pictureSoundControlInfo.Refresh();
        }
        private void refreshStatusBar()
        {
            string text = Config.GetOption("SOURCE");
            if (changedValue == true)
                text += "*";

            toolStripStatusLabel.Text = text;
        }
        private bool loadSoundList(string fileName)
        {
            changedValue = false;
            Config.SetOption("SOURCE", fileName);

            bool retVal = SoundContainer.LoadSource(Config.GetOption("SOURCE"));
            refreshSoundList();
            refreshStatusBar();

            return retVal;
        }
        private bool saveSoundList(string fileName)
        {
            changedValue = false;
            Config.SetOption("SOURCE", fileName);

            bool retVal = SoundContainer.SaveSource(Config.GetOption("SOURCE"));
            refreshStatusBar();

            return retVal;
        }

        // Main form event handler
        private void MainForm_Load(object sender, EventArgs e)
        {
            // Init FMOD
            RealSound.Init();

            // Load config
            Config.LoadConfig();

            // Modify UI
            this.Left = int.Parse(Config.GetOption("X", "50"));
            this.Top = int.Parse(Config.GetOption("Y", "50"));
            this.Width = int.Parse(Config.GetOption("WIDTH", "1024"));
            this.Height = int.Parse(Config.GetOption("HEIGHT", "768"));
            splitContainer1.SplitterDistance = int.Parse(Config.GetOption("SPLIT1", "250"));
            splitContainer2.SplitterDistance = int.Parse(Config.GetOption("SPLIT2", "250"));
            splitContainer3.SplitterDistance = int.Parse(Config.GetOption("SPLIT3", "250"));
            splitContainer4.SplitterDistance = int.Parse(Config.GetOption("SPLIT4", "250"));
            comboReverb.SelectedIndex = 0;


            if (bool.Parse(Config.GetOption("MAXIMIZE", "False")) == true)
                this.WindowState = FormWindowState.Maximized;

            if (propertySoundInfo != null)
            {
                SoundReverbProperty reverbInfo = new SoundReverbProperty();
                propertyReverb.SelectedObject = reverbInfo;

                object propertyGridView = typeof(PropertyGrid).InvokeMember("gridView", BindingFlags.GetField | BindingFlags.NonPublic | BindingFlags.Instance, null, propertySoundInfo, null);
                propertyGridView.GetType().InvokeMember("MoveSplitterTo", BindingFlags.InvokeMethod | BindingFlags.NonPublic | BindingFlags.Instance, null, propertyGridView, new object[] { 110 });
            }

            if (propertyReverb != null)
            {
                object propertyGridView = typeof(PropertyGrid).InvokeMember("gridView", BindingFlags.GetField | BindingFlags.NonPublic | BindingFlags.Instance, null, propertyReverb, null);
                propertyGridView.GetType().InvokeMember("MoveSplitterTo", BindingFlags.InvokeMethod | BindingFlags.NonPublic | BindingFlags.Instance, null, propertyGridView, new object[] { 130 });
            }

            soundPositionScale = float.Parse(Config.GetOption("SNDPOSSCALE", "1.0"));

            if ( panelComponent != null)
                panelComponent.AutoScrollMinSize = new Size(15000, 2015);

            labelSoundControl.Text = "거리에 따른 사운드 직접음의 정도를 그래프로 표시합니다. 높을 수록 직접음이 커지고, 낮을 수록 작아집니다.";

            updateReverbProperty();


            // Check working directory
            if (Config.GetOption("WORKINGDIR") == string.Empty)
            {
                FolderBrowserDialog folderBrowser = new FolderBrowserDialog();
                folderBrowser.RootFolder = Environment.SpecialFolder.Desktop;
                folderBrowser.SelectedPath = Config.GetOption("WORKINGDIR").Replace('/', '\\');
                folderBrowser.Description = "시작 프로젝트의 위치를 설정해주시기 바랍니다.";

                if (folderBrowser.ShowDialog() == DialogResult.OK)
                {
                    string pathName = folderBrowser.SelectedPath;
                    Config.SetOption("WORKINGDIR", pathName.Replace('\\', '/') + "/");
                }

                folderBrowser.Dispose();
            }

            loadSoundList(Config.GetOption("SOURCE"));

            // Run timer
            timerUpdate.Enabled = true;
        }
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (changedValue == true)
            {
                DialogResult result = MessageBox.Show("아직 저장하지 않았습니다. 저장 후 종료하시겠습니까?", "종료 확인", MessageBoxButtons.YesNoCancel);
                if (result == DialogResult.Yes)
                    saveSoundList(Config.GetOption("SOURCE"));
                else if (result == DialogResult.Cancel)
                {
                    e.Cancel = true;
                    return;
                }
            }

            this.Visible = false;

            Config.SetOption("MAXIMIZE", (this.WindowState == FormWindowState.Maximized).ToString());
            this.WindowState = FormWindowState.Normal;
            this.UpdateBounds( this.RestoreBounds.X, this.RestoreBounds.Y, this.RestoreBounds.Width, this.RestoreBounds.Height);

            Config.SetOption("X", this.RestoreBounds.Left.ToString());
            Config.SetOption("Y", this.RestoreBounds.Top.ToString());
            Config.SetOption("WIDTH", this.RestoreBounds.Width.ToString());
            Config.SetOption("HEIGHT", this.RestoreBounds.Height.ToString());
            Config.SetOption("SPLIT1", splitContainer1.SplitterDistance.ToString());
            Config.SetOption("SPLIT2", splitContainer2.SplitterDistance.ToString());
            Config.SetOption("SPLIT3", splitContainer3.SplitterDistance.ToString());
            Config.SetOption("SPLIT4", splitContainer4.SplitterDistance.ToString());
            Config.SetOption("SNDPOSSCALE", soundPositionScale.ToString());
            Config.SaveConfig();

            e.Cancel = false;
        }

        // Main menu event handler
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string pathName = (Config.GetOption("SOURCE") != string.Empty) ? Config.GetOption("SOURCE") : Config.GetOption("WORKINGDIR");

            OpenFileDialog dialogFile = new OpenFileDialog();
            dialogFile.Title = "Select a source file";
            dialogFile.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            if (pathName != string.Empty)
            {
                dialogFile.InitialDirectory = Path.GetDirectoryName(pathName);
                dialogFile.FileName = Path.GetFileName(pathName);
            }

            if (dialogFile.ShowDialog() != DialogResult.OK || dialogFile.FileName == string.Empty)
                return;

            loadSoundList(dialogFile.FileName);

            dialogFile.Dispose();
        }
        private void reloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            loadSoundList(Config.GetOption("SOURCE"));
        }
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveSoundList(Config.GetOption("SOURCE"));
        }
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string pathName = (Config.GetOption("SOURCE") != string.Empty) ? Config.GetOption("SOURCE") : Config.GetOption("WORKINGDIR");

            OpenFileDialog dialogFile = new OpenFileDialog();
            dialogFile.Title = "Select a source file";
            dialogFile.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            if (pathName != string.Empty)
            {
                dialogFile.InitialDirectory = Path.GetDirectoryName(pathName);
                dialogFile.FileName = Path.GetFileName(pathName);
            }

            if (dialogFile.ShowDialog() != DialogResult.OK || dialogFile.FileName == string.Empty)
                return;

            saveSoundList(dialogFile.FileName);

            dialogFile.Dispose();
        }
        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (changedValue == true)
            {
                DialogResult result = MessageBox.Show("아직 저장하지 않았습니다. 저장 하시겠습니까?", "닫기 확인", MessageBoxButtons.YesNoCancel);
                if (result == DialogResult.Yes)
                    saveSoundList(Config.GetOption("SOURCE"));
                else if (result == DialogResult.Cancel)
                    return;
            }

            propertySoundInfo.Tag = null;
            propertySoundInfo.SelectedObject = null;

            SoundContainer.Clear();

            loadSoundList("");
        }
        private void setworkdirToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowser = new FolderBrowserDialog();
            folderBrowser.RootFolder = Environment.SpecialFolder.Desktop;
            folderBrowser.SelectedPath = Config.GetOption("WORKINGDIR").Replace('/', '\\');
            folderBrowser.Description = "시작 프로젝트의 위치를 설정해주시기 바랍니다.";

            if (folderBrowser.ShowDialog() == DialogResult.OK)
            {
                string pathName = folderBrowser.SelectedPath;
                Config.SetOption("WORKINGDIR", pathName.Replace('\\', '/') + "/");
            }

            folderBrowser.Dispose();
        }
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }
        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }
        private void validationToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox aboutBox = new AboutBox();
            aboutBox.ShowDialog();
            aboutBox.Dispose();
        }

        // Sound list event handler
        private void treeSoundList_AfterSelect(object sender, TreeViewEventArgs e)
        {
            RealSound.Stop();

            refreshSoundInfos();
            refreshSoundPosition();
            refreshSoundColtrol();
        }
        private void treeSoundList_ItemDrag(object sender, ItemDragEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                TreeView treeView = (TreeView)sender;
                treeView.SelectedNode = (TreeNode)e.Item;

                DoDragDrop(e.Item, DragDropEffects.Move);
            }
        }
        private void treeSoundList_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }
        private void treeSoundList_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent("System.Windows.Forms.TreeNode", false))
            {
                TreeView treeView = (TreeView)sender;
                Point pt = treeView.PointToClient(new Point(e.X, e.Y));
                TreeNode destNode = treeView.GetNodeAt(pt);
                TreeNode dropNode = (TreeNode)e.Data.GetData("System.Windows.Forms.TreeNode");
                if (treeView == dropNode.TreeView)
                {
                    TreeNode node = destNode;
                    while (node != null)
                    {
                        if (node == dropNode)
                            return;

                        node = node.Parent;
                    }

                    int index = destNode.Index;
                    treeView.Nodes.Remove(dropNode);

                    SoundNode soundNode = (SoundNode)dropNode.Tag;
                    SoundGroup soundGroup = (SoundGroup)soundNode.Parent;
                    SoundContainer.Sounds.Remove(soundNode);


                    if (destNode.Tag == null || ((SoundNode)destNode.Tag).IsGroup() == true)
                    {
                        destNode.Nodes.Add(dropNode);

                        if (destNode.Tag == null)
                            SoundContainer.Sounds.Nodes.Add(soundNode);
                        else
                            ((SoundGroup)destNode.Tag).Nodes.Add(soundNode);
                    }
                    else
                    {
                        destNode.Parent.Nodes.Insert(index, dropNode);

                        if (destNode.Parent.Tag == null)
                            SoundContainer.Sounds.Nodes.Insert( index, soundNode);
                        else
                            ((SoundGroup)destNode.Parent.Tag).Nodes.Insert(index, soundNode);
                    }

                    treeView.SelectedNode = dropNode;
                    refreshSoundItem(dropNode);

                    changedValue = true;
                    refreshStatusBar();
                }
            }
        }

        // Sound info property event handler
        private void propertySoundInfo_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (s == null || propertySoundInfo.SelectedObject == null)
                return;

            TreeNode treeNode = (TreeNode)((PropertyGrid)s).Tag;
            if (treeNode == null || treeNode.Tag == null)
                return;

            RealSound.Stop();

            SoundNode soundNode = (SoundNode)treeNode.Tag;
            if (soundNode.IsGroup() == true)
            {
                SoundGroup soundGroup = (SoundGroup)soundNode;
                SoundGroupProperty soundSettings = (SoundGroupProperty)propertySoundInfo.SelectedObject;
                soundGroup.Name = soundSettings.Name;
            }
            else
            {
                SoundItem soundItem = (SoundItem)soundNode;
                SoundItemProperty soundSettings = (SoundItemProperty)propertySoundInfo.SelectedObject;
                soundItem.Name = soundSettings.Name;
                soundItem.Filename = soundSettings.FileName;
                soundItem.Type = soundSettings.Type;
                soundItem.Volume = soundSettings.Volume;
                soundItem.Loop = soundSettings.Loop;
                soundItem.Unique = soundSettings.Unique;
                soundItem.MinDistance = soundSettings.MinDistance;
                soundItem.MaxDistance = soundSettings.MaxDistance;
                soundItem.Priority = soundSettings.Priority;

            }

            changedValue = true;

            refreshSoundItem(treeNode);
            refreshSoundInfos();
            refreshSoundPosition();
            refreshSoundColtrol();
            refreshStatusBar();
        }

        // Sound list
        private void listSounds_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            ListView listView = (ListView)sender;
            if (e.IsSelected == false)
                e.Item.Selected = true;
        }
        private void listSounds_Resize(object sender, EventArgs e)
        {
            ListView listView = (ListView)sender;
            listView.Columns[1].Width = listView.ClientRectangle.Width - listView.Columns[0].Width;
        }

        // Sound environment tab event handler
        private void tabSoundEnvironment_SizeChanged(object sender, EventArgs e)
        {
            ((TabControl)sender).Refresh();
        }

        // Add/Remove sound buttons event handler
        private void buttonAddSound_Click(object sender, EventArgs e)
        {
            TreeNode treeNode = treeSoundList.SelectedNode;
            if (treeNode != null)
            {
                SoundNode soundNode = (SoundNode)treeNode.Tag;
                SoundItem newSound;
                int index;
                SoundContainer.InsertSound(soundNode, out newSound, out index);


                TreeNode newTreeNode = null;
                if (treeNode.Parent == null)
                    newTreeNode = treeNode.Nodes.Insert(index, newSound.Name);
                else if (soundNode.IsGroup() == true)
                    newTreeNode = treeNode.Nodes.Insert(index, newSound.Name);
                else
                    newTreeNode = treeNode.Parent.Nodes.Insert(index, newSound.Name);

                newTreeNode.Tag = newSound;
                treeSoundList.SelectedNode = newTreeNode;

                refreshSoundItem(newTreeNode);

                changedValue = true;
                refreshStatusBar();
            }

            treeSoundList.Focus();
        }
        private void buttonAddGroup_Click(object sender, EventArgs e)
        {
            TreeNode treeNode = treeSoundList.SelectedNode;
            if (treeNode != null)
            {
                SoundNode soundNode = (SoundNode)treeNode.Tag;
                SoundGroup newGroup;
                int index;
                SoundContainer.InsertGroup(soundNode, out newGroup, out index);


                TreeNode newTreeNode = null;
                if (treeNode.Parent == null)
                    newTreeNode = treeNode.Nodes.Insert(index, newGroup.Name);
                else if (soundNode.IsGroup() == true)
                    newTreeNode = treeNode.Nodes.Insert(index, newGroup.Name);
                else
                    newTreeNode = treeNode.Parent.Nodes.Insert(index, newGroup.Name);

                newTreeNode.Tag = newGroup;
                treeSoundList.SelectedNode = newTreeNode;

                refreshSoundItem(newTreeNode);

                changedValue = true;
                refreshStatusBar();
            }

            treeSoundList.Focus();
        }
        private void buttonRemove_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("선택한 사운드 항목을 정말로 삭제하시겠습니까?", "삭제 확인", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                TreeNode treeNode = treeSoundList.SelectedNode;
                if (treeNode != null)
                {
                    TreeNode selectTreeNode = treeNode.NextVisibleNode;
                    if (selectTreeNode == null)
                        selectTreeNode = treeNode.PrevVisibleNode;

                    SoundNode soundNode = (SoundNode)treeNode.Tag;
                    SoundContainer.Sounds.Remove(soundNode);

                    treeSoundList.Nodes.Remove(treeNode);
                    treeSoundList.SelectedNode = selectTreeNode;

                    refreshSoundInfos();

                    changedValue = true;
                    refreshStatusBar();
                }
            }

            treeSoundList.Focus();
        }

        // Reverb
        private void comboReverb_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox combobox = (ComboBox)sender;
            RealSound.SetReverb((RealSound.REVERBTYPE)combobox.SelectedIndex);

            updateReverbProperty();
        }
        private void propertyReverb_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            PropertyGrid property = (PropertyGrid)s;
            if (property.SelectedObject == null)
                return;

            FMOD.REVERB_PROPERTIES prop = new FMOD.REVERB_PROPERTIES();
            RealSound.GetreverbProp(RealSound.GetReverb(), ref prop);

            SoundReverbProperty reverbprop = (SoundReverbProperty)property.SelectedObject;
            prop.AirAbsorptionHF = reverbprop.AirAbsorptionHF;
            prop.DecayHFRatio = reverbprop.DecayHFRatio;
            prop.DecayLFRatio = reverbprop.DecayLFRatio;
            prop.DecayTime = reverbprop.DecayTime;
            prop.Density = reverbprop.Density;
            prop.Diffusion = reverbprop.Diffusion;
            prop.EchoDepth = reverbprop.EchoDepth;
            prop.EchoTime = reverbprop.EchoTime;
            prop.EnvDiffusion = reverbprop.EnvDiffusion;
            prop.Environment = reverbprop.Environment;
            prop.EnvSize = reverbprop.EnvSize;
            prop.HFReference = reverbprop.HFReference;
            prop.LFReference = reverbprop.LFReference;
            prop.ModulationDepth = reverbprop.ModulationDepth;
            prop.ModulationTime = reverbprop.ModulationTime;
            prop.Reflections = reverbprop.Reflections;
            prop.ReflectionsDelay = reverbprop.ReflectionsDelay;
            prop.Reverb = reverbprop.Reverb;
            prop.ReverbDelay = reverbprop.ReverbDelay;
            prop.Room = reverbprop.Room;
            prop.RoomHF = reverbprop.RoomHF;
            prop.RoomLF = reverbprop.RoomLF;
            prop.RoomRolloffFactor = reverbprop.RoomRolloffFactor;
            RealSound.SetReverbProp(RealSound.GetReverb(), prop);
        }
        private void updateReverbProperty()
        {
            SoundReverbProperty reverbprop = (SoundReverbProperty)propertyReverb.SelectedObject;
            if (reverbprop != null)
            {
                FMOD.REVERB_PROPERTIES prop = new FMOD.REVERB_PROPERTIES();
                RealSound.GetreverbProp(RealSound.GetReverb(), ref prop);

                reverbprop.AirAbsorptionHF = prop.AirAbsorptionHF;
                reverbprop.DecayHFRatio = prop.DecayHFRatio;
                reverbprop.DecayLFRatio = prop.DecayLFRatio;
                reverbprop.DecayTime = prop.DecayTime;
                reverbprop.Density = prop.Density;
                reverbprop.Diffusion = prop.Diffusion;
                reverbprop.EchoDepth = prop.EchoDepth;
                reverbprop.EchoTime = prop.EchoTime;
                reverbprop.EnvDiffusion = prop.EnvDiffusion;
                reverbprop.Environment = prop.Environment;
                reverbprop.EnvSize = prop.EnvSize;
                reverbprop.HFReference = prop.HFReference;
                reverbprop.LFReference = prop.LFReference;
                reverbprop.ModulationDepth = prop.ModulationDepth;
                reverbprop.ModulationTime = prop.ModulationTime;
                reverbprop.Reflections = prop.Reflections;
                reverbprop.ReflectionsDelay = prop.ReflectionsDelay;
                reverbprop.Reverb = prop.Reverb;
                reverbprop.ReverbDelay = prop.ReverbDelay;
                reverbprop.Room = prop.Room;
                reverbprop.RoomHF = prop.RoomHF;
                reverbprop.RoomLF = prop.RoomLF;
                reverbprop.RoomRolloffFactor = prop.RoomRolloffFactor;
                RealSound.SetReverbProp(RealSound.GetReverb(), prop);

                propertyReverb.Refresh();
            }
        }

        // Sound position info event handler
        private void pictureSoundPositionInfo_Paint(object sender, PaintEventArgs e)
        {
            Graphics grphx = e.Graphics;

            SoundItem soundItem = null;
            if (propertySoundInfo.Tag != null)
            {
                TreeNode treeNode = (TreeNode)propertySoundInfo.Tag;
                if (treeNode.Tag != null)
                {
                    SoundNode soundNode = (SoundNode)treeNode.Tag;
                    if (soundNode.IsGroup() == false && ((SoundItem)soundNode).Type == SoundItemProperty.typeSound[1])
                        soundItem = (SoundItem)soundNode;
                }
            }


            Rectangle r = ((Control)sender).ClientRectangle;
            if (r.Width <= 0 || r.Height <= 0)
                return;

            Point p = new Point();
            p.X = r.X + (r.Width >> 1);
            p.Y = r.Y + (r.Height >> 1);

            float scale_ratio = (float)Math.Max(r.Width, r.Height) / (20.0f * soundPositionScale);

            Point sp = new Point(p.X + (int)(soundPosition.X * 0.01f * (float)scale_ratio), p.Y - (int)(soundPosition.Y * 0.01f * (float)scale_ratio));



            // Draw background
            int radius = (int)(Math.Sqrt((double)(r.Width * r.Width + r.Height * r.Height)) + 0.5);
            int half_rad = radius >> 1;
            {
                {
                    Rectangle fillrect = new Rectangle(p.X - half_rad, p.Y - half_rad, radius, radius);
                    GraphicsPath paths = new GraphicsPath();
                    paths.AddEllipse(fillrect);
                    PathGradientBrush gradient = new PathGradientBrush(paths);
                    Color[] colors = { Color.FromArgb(195, 210, 230) };
                    gradient.SurroundColors = colors;
                    gradient.CenterColor = Color.FromArgb(50, 100, 150);
                    gradient.CenterPoint = new Point(p.X, p.Y);
                    grphx.FillRectangle(gradient, r);
                
                    paths.Dispose();
                    gradient.Dispose();
                }
                {
                    Font font = new Font("Arial", 12, FontStyle.Bold);
                    SolidBrush brush = new SolidBrush(Color.FromArgb(64, 255, 255, 255));
                    StringFormat strformat = new StringFormat();
                    Rectangle tr = new Rectangle(r.X + 2, r.Y + 2, r.Width - 4, r.Height - 4);
                    strformat.Alignment = StringAlignment.Center;
                    strformat.LineAlignment = StringAlignment.Near;
                    grphx.DrawString("FRONT", font, brush, tr, strformat);
                    strformat.LineAlignment = StringAlignment.Far;
                    grphx.DrawString("BACK", font, brush, tr, strformat);
                    strformat.Alignment = StringAlignment.Near;
                    strformat.LineAlignment = StringAlignment.Center;
                    grphx.DrawString("LEFT", font, brush, tr, strformat);
                    strformat.Alignment = StringAlignment.Far;
                    grphx.DrawString("RIGHT", font, brush, tr, strformat);

                    font.Dispose();
                    brush.Dispose();
                    strformat.Dispose();
                }
            }


            // Draw range
            {
                Pen pen = new Pen(Color.FromArgb(64, 255, 255, 255));
                grphx.DrawLine(pen, r.X, p.Y, r.X + r.Width, p.Y);
                grphx.DrawLine(pen, p.X, r.Y, p.X, r.Y + r.Height);
                grphx.DrawLine(pen, p.X - half_rad, p.Y - half_rad, p.X + radius, p.Y + radius);
                grphx.DrawLine(pen, p.X + half_rad, p.Y - half_rad, p.X - radius, p.Y + radius);

                float deltad_d = scale_ratio;
                while (deltad_d < 20.0f)
                    deltad_d *= 2.0f;

                float delta_subd = deltad_d / 5.0f;
                for (float rad = 0; rad <= half_rad; rad += deltad_d)
                {
                    pen.Color = Color.FromArgb(64, 255, 255, 255);
                    grphx.DrawEllipse(pen, p.X - rad, p.Y - rad, rad * 2.0f, rad * 2.0f);

                    pen.Color = Color.FromArgb(16, 255, 255, 255);
                    for (int i = 1; i < 5; i++)
                    {
                        int _subrad = (int)(rad + delta_subd * (float)i);
                        grphx.DrawEllipse(pen, p.X - _subrad, p.Y - _subrad, _subrad * 2.0f, _subrad * 2.0f);
                    }
                }

                pen.Dispose();
            }


            // Draw min/max distance
            if (soundItem != null)
            {
                float _rad = (float)soundItem.MaxDistance * 0.01f * scale_ratio;
                Rectangle fillrect = new Rectangle((int)(sp.X - _rad), (int)(sp.Y - _rad), (int)(_rad * 2.0f), (int)(_rad * 2.0f));
                GraphicsPath paths = new GraphicsPath();
                paths.AddEllipse(fillrect);
                PathGradientBrush gradient = new PathGradientBrush(paths);
                Color[] colors = { Color.FromArgb(48, 255, 255, 255) };
                gradient.SurroundColors = colors;
                gradient.CenterColor = Color.FromArgb(0, 255, 255, 255);
                gradient.CenterPoint = new Point(sp.X, sp.Y);
                grphx.FillRectangle(gradient,sp.X - _rad, sp.Y - _rad, _rad * 2.0f, _rad * 2.0f);

                paths.Dispose();
                gradient.Dispose();
            }


            // Draw sound position shadow/sound
            {
                Rectangle fillrect = new Rectangle(sp.X - 13, sp.Y - 13, 26, 26);
                GraphicsPath paths = new GraphicsPath();
                paths.AddEllipse(fillrect);
                PathGradientBrush gradient = new PathGradientBrush(paths);
                Color[] colors = { Color.FromArgb(0, 0, 0, 0) };
                gradient.SurroundColors = colors;
                gradient.CenterColor = Color.FromArgb(100, 0, 0, 0);
                gradient.CenterPoint = new Point(sp.X, sp.Y);
                grphx.FillRectangle(gradient, r);

                paths.Dispose();
                gradient.Dispose();
            }
            {
                Rectangle fillrect = new Rectangle(sp.X - 9, sp.Y - 9, 18, 18);
                GraphicsPath paths = new GraphicsPath();
                paths.AddEllipse(fillrect);
                PathGradientBrush gradient = new PathGradientBrush(paths);
                if (soundItem != null)
                {
                    Color[] colors = { Color.FromArgb(255, 32, 0) };
                    gradient.SurroundColors = colors;
                    gradient.CenterColor = Color.FromArgb(255, 200, 0);
                }
                else
                {
                    Color[] colors = { Color.FromArgb(160, 160, 160) };
                    gradient.SurroundColors = colors;
                    gradient.CenterColor = Color.FromArgb(255, 255, 255);
                }
                gradient.CenterPoint = new Point(sp.X - 4, sp.Y - 4);
                grphx.FillRectangle(gradient, r);


                Font font = new Font("Arial", 8, FontStyle.Italic | FontStyle.Bold);
                SolidBrush brush = new SolidBrush(Color.FromArgb(128, 255, 255, 255));
                StringFormat strformat = new StringFormat();
                Rectangle tr = new Rectangle(r.X + 5, r.Y + 3, r.Width - 10, r.Height - 6);
                strformat.Alignment = StringAlignment.Center;
                strformat.LineAlignment = StringAlignment.Near;
                double distance = Math.Sqrt(soundPosition.X * soundPosition.X + soundPosition.Y * soundPosition.Y) * 0.01;
                grphx.DrawString(string.Format("{0:F1} m", distance), font, brush, new Rectangle(sp.X - 50, sp.Y + 10, 100, 20), strformat);

                paths.Dispose();
                gradient.Dispose();
                font.Dispose();
                strformat.Dispose();
            }


            // Draw frame
            {
                Pen pen = new Pen(Color.LightGray);
                grphx.DrawRectangle(pen, r.X, r.Y, r.Width, r.Height);
                grphx.DrawRectangle(pen, r.X + 1, r.Y + 1, r.Width - 2, r.Height - 2);
                grphx.DrawRectangle(pen, r.X + 2, r.Y + 2, r.Width - 4, r.Height - 4);
                pen.Color = Color.FromArgb(64, 255, 255, 255);
                grphx.DrawRectangle(pen, r.X + 2, r.Y + 2, r.Width - 4, r.Height - 4);
            }
        }
        private void pictureSoundPositionInfo_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
                setSoundPosition(((Control)sender).ClientRectangle, e.Location);
        }
        private void pictureSoundPositionInfo_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
                setSoundPosition(((Control)sender).ClientRectangle, e.Location);
        }
        private void buttonPositionScaleUp_Click(object sender, EventArgs e)
        {
            soundPositionScale = Math.Max(1.0f, soundPositionScale * 0.5f);
            pictureSoundPositionInfo.Focus();
            pictureSoundPositionInfo.Refresh();
        }
        private void buttonPositionScaleDown_Click(object sender, EventArgs e)
        {
            soundPositionScale = Math.Min(50.0f, soundPositionScale * 2.0f);
            pictureSoundPositionInfo.Focus();
            pictureSoundPositionInfo.Refresh();
        }
        private void setSoundPosition(Rectangle r, Point p)
        {
            int half_rad = (int)(Math.Sqrt((double)(r.Width * r.Width + r.Height * r.Height)) + 0.5) >> 1;
            float delta_d = (float)Math.Max(r.Width, r.Height) / (20.0f * soundPositionScale);
            soundPosition.X = (float)(p.X - r.X - (r.Width >> 1)) / (0.01f * (float)delta_d);
            soundPosition.Y = (float)(-p.Y + r.Y + (r.Height >> 1)) / (0.01f * (float)delta_d);

            pictureSoundPositionInfo.Refresh();

            RealSound.SetSoundPosition( soundPosition.X, soundPosition.Y);
        }

        // Sound control info event handler
        private void radioDirect_CheckedChanged(object sender, EventArgs e)
        {
            selectedControlPoint = SoundControlPoint.NONE;
            pictureSoundControlInfo.Refresh();
            labelSoundControl.Text = "거리에 따른 사운드 직접음의 정도를 그래프로 표시합니다. 높을 수록 직접음이 커지고, 낮을 수록 작아집니다.";
        }
        private void radioReverb_CheckedChanged(object sender, EventArgs e)
        {
            selectedControlPoint = SoundControlPoint.NONE;
            pictureSoundControlInfo.Refresh();
            labelSoundControl.Text = "거리에 따른 사운드 반향음의 정도를 그래프로 표시합니다. 높을 수록 반향음이 커지고, 낮을 수록 작아집니다.";
        }
        private void radioLowpass_CheckedChanged(object sender, EventArgs e)
        {
            selectedControlPoint = SoundControlPoint.NONE;
            pictureSoundControlInfo.Refresh();
            labelSoundControl.Text = "거리에 따른 사운드 로우패스 필터(Low-pass filter)의 cut-off 주파수를 그래프로 표시합니다. 높을 수록 대부분의 주파수 사운드가 들리고, 낮을 수록 낮은 주파수의 사운드만 들립니다.";
        }
        private void radioRolloff_CheckedChanged(object sender, EventArgs e)
        {
            selectedControlPoint = SoundControlPoint.NONE;
            pictureSoundControlInfo.Refresh();
            labelSoundControl.Text = "거리에 따른 사운드 감쇠의 정도를 그래프로 표시합니다. 높을 수록 소리가 커지고, 낮을 수록 작아집니다.";
        }
        private void radioPanlevel_CheckedChanged(object sender, EventArgs e)
        {
            selectedControlPoint = SoundControlPoint.NONE;
            pictureSoundControlInfo.Refresh();
            labelSoundControl.Text = "거리에 따른 사운드 좌우 분리 정도(Pan level)를 그래프로 표시합니다. 높을 수록 좌우 분리가 커지며, 낮을 수록 좌우 음량이 같게됩니다.";
        }
        private void pictureSoundControlInfo_Paint(object sender, PaintEventArgs e)
        {
            Graphics grphx = e.Graphics;
            Rectangle r = ((Control)sender).ClientRectangle;
            if (r.Width <= 0 || r.Height <= 0)
                return;

            // Draw background
            {
                LinearGradientBrush gradient = new LinearGradientBrush(r, Color.FromArgb(195, 210, 230), Color.FromArgb(70, 120, 170), LinearGradientMode.Vertical);
                grphx.FillRectangle(gradient, r);
                gradient.Dispose();
            }


            // Draw grid
            Rectangle _r = r;
            _r.X += 10;
            _r.Y += 10;
            _r.Width -= 20;
            _r.Height -= 20;
            {
                Pen pen = new Pen(Color.FromArgb(64, 255, 255, 255));
                double dx = (double)_r.Width / 10.0;
                for (double x = 0.0; x <= 1.0; x += 0.1)
                {
                    int px = (int)((double)_r.X + (double)_r.Width * x);
                    grphx.DrawLine(pen, px, _r.Y, px, _r.Y + _r.Height);
                }
                for (double y = 0.0; y <= 1.0; y += 0.1)
                {
                    int py = (int)((double)(_r.Y + _r.Height) - ((double)_r.Height * y));
                    grphx.DrawLine(pen, _r.X, py, _r.X + _r.Width, py);
                }

                pen.Dispose();
            }


            // Draw graph
            if (radioDirect.Checked == true)
                drawControlGraph(grphx, _r, Color.FromArgb(48, 0, 0, 255), RealSound.CONTROLTYPE.DIRECT);
            else if (radioReverb.Checked == true)
                drawControlGraph(grphx, _r, Color.FromArgb(48, 0, 0, 255), RealSound.CONTROLTYPE.REVERB);
            else if (radioLowpass.Checked == true)
                drawControlGraph(grphx, _r, Color.FromArgb(48, 0, 0, 255), RealSound.CONTROLTYPE.LOWPASS);
            else if (radioRolloff.Checked == true)
                drawControlGraph(grphx, _r, Color.FromArgb(48, 0, 0, 255), RealSound.CONTROLTYPE.ROLLOFF);
            else if (radioPanlevel.Checked == true)
                drawControlGraph(grphx, _r, Color.FromArgb(48, 0, 0, 255), RealSound.CONTROLTYPE.PANLEVEL);


            // Draw frame
            {
                Pen pen = new Pen(Color.LightGray);
                grphx.DrawRectangle(pen, r.X, r.Y, r.Width, r.Height);
                grphx.DrawRectangle(pen, r.X + 1, r.Y + 1, r.Width - 2, r.Height - 2);
                grphx.DrawRectangle(pen, r.X + 2, r.Y + 2, r.Width - 4, r.Height - 4);
                pen.Color = Color.FromArgb(64, 255, 255, 255);
                grphx.DrawRectangle(pen, r.X + 2, r.Y + 2, r.Width - 4, r.Height - 4);
            }
        }
        private void pictureSoundControlInfo_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left)
            {
                selectedControlPoint = SoundControlPoint.NONE;

                pictureSoundControlInfo.Refresh();
                return;
            }

            SoundControlPoint restoreControlPoint = selectedControlPoint;
            selectedControlPoint = SoundControlPoint.NONE;

            Rectangle r = ((Control)sender).ClientRectangle;
            r.X += 10;
            r.Y += 10;
            r.Width -= 20;
            r.Height -= 20;

            int control = 0;
            if (radioDirect.Checked == true) control = (int)RealSound.CONTROLTYPE.DIRECT;
            else if (radioReverb.Checked == true) control = (int)RealSound.CONTROLTYPE.REVERB;
            else if (radioLowpass.Checked == true) control = (int)RealSound.CONTROLTYPE.LOWPASS;
            else if (radioRolloff.Checked == true) control = (int)RealSound.CONTROLTYPE.ROLLOFF;
            else if (radioPanlevel.Checked == true) control = (int)RealSound.CONTROLTYPE.PANLEVEL;

            RealSound.ControlInfo info = RealSound.CONTROLS[control];
            if (selectedControlPoint == SoundControlPoint.NONE)
            {
                PointF p = new PointF((float)r.X + (float)r.Width * info.Start.X, (float)(r.Y + r.Height) - (float)r.Height * info.Start.Y);
                if (e.X >= (p.X - 5) && e.X < (p.X + 5) && e.Y >= (p.Y - 5) && e.Y < (p.Y + 5))
                    selectedControlPoint = SoundControlPoint.START;
            }

            if (selectedControlPoint == SoundControlPoint.NONE)
            {
                PointF p = new PointF((float)r.X + (float)r.Width * info.End.X, (float)(r.Y + r.Height) - (float)r.Height * info.End.Y);
                if (e.X >= (p.X - 5) && e.X < (p.X + 5) && e.Y >= (p.Y - 5) && e.Y < (p.Y + 5))
                    selectedControlPoint = SoundControlPoint.END;
            }

            if (selectedControlPoint == SoundControlPoint.NONE)
            {
                PointF p = new PointF((float)r.X + (float)r.Width * (info.Start.X + info.StartVel.X), (float)(r.Y + r.Height) - (float)r.Height * (info.Start.Y + info.StartVel.Y));
                if (e.X >= (p.X - 3) && e.X < (p.X + 3) && e.Y >= (p.Y - 3) && e.Y < (p.Y + 3))
                    selectedControlPoint = SoundControlPoint.STARTVEL;
            }
            
            if (selectedControlPoint == SoundControlPoint.NONE)
            {
                PointF p = new PointF((float)r.X + (float)r.Width * (info.End.X + info.EndVel.X), (float)(r.Y + r.Height) - (float)r.Height * (info.End.Y + info.EndVel.Y));
                if (e.X >= (p.X - 3) && e.X < (p.X + 3) && e.Y >= (p.Y - 3) && e.Y < (p.Y + 3))
                selectedControlPoint = SoundControlPoint.ENDVEL;
            }

            if (restoreControlPoint != selectedControlPoint)
                pictureSoundControlInfo.Refresh();
        }
        private void pictureSoundControlInfo_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left || selectedControlPoint == SoundControlPoint.NONE)
                return;

            Rectangle r = ((Control)sender).ClientRectangle;
            r.X += 5;
            r.Y += 5;
            r.Width -= 10;
            r.Height -= 10;

            PointF p = new PointF();
            p.X = Math.Max( 0.0f, Math.Min( 1.0f, (float)(e.X - r.X) / (float)r.Width));
            p.Y = Math.Max( 0.0f, Math.Min( 1.0f, 1.0f - (float)(e.Y - r.Y) / (float)r.Height));

            int control = 0;
            if (radioDirect.Checked == true) control = (int)RealSound.CONTROLTYPE.DIRECT;
            else if (radioReverb.Checked == true) control = (int)RealSound.CONTROLTYPE.REVERB;
            else if (radioLowpass.Checked == true) control = (int)RealSound.CONTROLTYPE.LOWPASS;
            else if (radioRolloff.Checked == true) control = (int)RealSound.CONTROLTYPE.ROLLOFF;
            else if (radioPanlevel.Checked == true) control = (int)RealSound.CONTROLTYPE.PANLEVEL;
            switch (selectedControlPoint)
            {
                case SoundControlPoint.START:
                    {
                        RealSound.CONTROLS[control].Start = p;
                        break;
                    }

                case SoundControlPoint.END:
                    {
                        RealSound.CONTROLS[control].End = p;
                        break;
                    }

                case SoundControlPoint.STARTVEL:
                    {
                        PointF dp = new PointF();
                        dp.X = p.X - RealSound.CONTROLS[control].Start.X;
                        dp.Y = p.Y - RealSound.CONTROLS[control].Start.Y;
                        RealSound.CONTROLS[control].StartVel = dp;
                        break;
                    }

                case SoundControlPoint.ENDVEL:
                    {
                        PointF dp = new PointF();
                        dp.X = p.X - RealSound.CONTROLS[control].End.X;
                        dp.Y = p.Y - RealSound.CONTROLS[control].End.Y;
                        RealSound.CONTROLS[control].EndVel = dp;
                        break;
                    }
            }

            pictureSoundControlInfo.Refresh();

            changedValue = true;
            refreshStatusBar();

            if (control == (int)RealSound.CONTROLTYPE.ROLLOFF)
            {
                RealSound.SetSoundPosition(soundPosition.X + 1.0f, soundPosition.Y);
                RealSound.SetSoundPosition(soundPosition.X, soundPosition.Y);
            }
        }
        private void drawControlGraph(Graphics grphx, Rectangle r, Color c, RealSound.CONTROLTYPE type)
        {
            SolidBrush brush = new SolidBrush(c);
            PointF[] points = new PointF[104];
            int index = 0;
            for (double ratio = 0.0; ratio <= 1.0; ratio += 0.01)
            {
                float value = RealSound.GetDistanceRatioValue(type, (float)ratio);
                points[index].X = (float)r.X + (float)r.Width * (float)ratio;
                points[index].Y = (float)(r.Y + r.Height) - (float)r.Height * value;

                index++;
            }
            points[100] = new PointF(r.X + r.Width, (float)(r.Y + r.Height) - (float)r.Height * RealSound.GetDistanceRatioValue(type, 1.0f));
            points[101] = new PointF(r.X + r.Width, r.Y + r.Height);
            points[102] = new PointF(r.X, r.Y + r.Height);
            points[103] = new PointF(r.X, (float)(r.Y + r.Height) - (float)r.Height * RealSound.GetDistanceRatioValue(type, 0.0f));
            grphx.FillPolygon(brush, points);


            Pen pen = new Pen(Color.FromArgb(c.R, c.G, c.B));
            brush.Color = Color.FromArgb(c.R, c.G, c.B);
            PointF p1, p2;
            p1 = new PointF((float)r.X + (float)r.Width * RealSound.CONTROLS[(int)type].Start.X, (float)(r.Y + r.Height) - (float)r.Height * RealSound.CONTROLS[(int)type].Start.Y);
            grphx.FillRectangle(brush, p1.X - 5, p1.Y - 5, 10, 10);
            if (selectedControlPoint == SoundControlPoint.START || selectedControlPoint == SoundControlPoint.STARTVEL)
            {
                p2 = new PointF((float)r.X + (float)r.Width * (RealSound.CONTROLS[(int)type].Start.X + RealSound.CONTROLS[(int)type].StartVel.X), (float)(r.Y + r.Height) - (float)r.Height * (RealSound.CONTROLS[(int)type].Start.Y + RealSound.CONTROLS[(int)type].StartVel.Y));
                grphx.DrawLine(pen, p1, p2);
                grphx.FillEllipse(brush, p2.X - 4, p2.Y - 4, 8, 8);
            }

            p1 = new PointF((float)r.X + (float)r.Width * RealSound.CONTROLS[(int)type].End.X, p1.Y = (float)(r.Y + r.Height) - (float)r.Height * RealSound.CONTROLS[(int)type].End.Y);
            grphx.FillRectangle(brush, p1.X - 5, p1.Y - 5, 10, 10);
            if (selectedControlPoint == SoundControlPoint.END || selectedControlPoint == SoundControlPoint.ENDVEL)
            {
                p2 = new PointF((float)r.X + (float)r.Width * (RealSound.CONTROLS[(int)type].End.X + RealSound.CONTROLS[(int)type].EndVel.X), (float)(r.Y + r.Height) - (float)r.Height * (RealSound.CONTROLS[(int)type].End.Y + RealSound.CONTROLS[(int)type].EndVel.Y));
                grphx.DrawLine(pen, p1, p2);
                grphx.FillEllipse(brush, p2.X - 4, p2.Y - 4, 8, 8);
            }

            brush.Dispose();
            pen.Dispose();
        }
   
        // Component panel event handler
        private void panelComponent_Paint(object sender, PaintEventArgs e)
        {
            Graphics grphx = e.Graphics;
            Rectangle cr = ((Control)sender).ClientRectangle;
            if (cr.Width <= 0 || cr.Height <= 0)
                return;

            Rectangle r = cr;
            r.Y += 15;
            r.Height -= 15;

            // Draw background
            {
                SolidBrush brush = new SolidBrush(Color.FromArgb(230, 230, 230));
                grphx.FillRectangle(brush, cr);

                Rectangle fr = new Rectangle(cr.X, cr.Y, cr.Width, 15);
                LinearGradientBrush gradient = new LinearGradientBrush(fr, Color.FromArgb(195, 210, 230), Color.FromArgb(70, 120, 170), LinearGradientMode.Vertical);
                grphx.FillRectangle(gradient, fr);

                brush.Dispose();
                gradient.Dispose();
            }

            // Draw grid
            {
                Font font = new Font("Arial", 8, FontStyle.Bold | FontStyle.Italic);
                SolidBrush brush = new SolidBrush(Color.FromArgb(160, 255, 255, 255));
                StringFormat strformat = new StringFormat();
                strformat.Alignment = StringAlignment.Near;
                strformat.LineAlignment = StringAlignment.Far;
                Pen pen = new Pen(Color.FromArgb(128, 255, 255, 255));
                int sec = panelComponent.HorizontalScroll.Value / 50;
                for (int x = 0; x <= r.Width; x += 50)
                {
                    grphx.DrawLine(pen, r.X + x, cr.Y, r.X + x, cr.Y + cr.Height);
                    grphx.DrawString(string.Format("{0:D2}:{1:D2}", sec / 60, sec % 60), font, brush, new Rectangle(r.X + x, cr.Y, 50, 15), strformat);
                    sec++;
                }

                strformat.Alignment = StringAlignment.Center;
                strformat.LineAlignment = StringAlignment.Center;
                for (int y = 0; y <= r.Height; y += 20)
                    grphx.DrawLine(pen, cr.X, r.Y + y, cr.X + cr.Width, r.Y + y);

                font.Dispose();
                brush.Dispose();
                pen.Dispose();
                strformat.Dispose();
            }
        }
        private void panelComponent_SizeChanged(object sender, EventArgs e)
        {
            ((Control)sender).Refresh();
        }
        private void panelComponent_Scroll(object sender, ScrollEventArgs e)
        {
            ((Control)sender).Refresh();
        }

        // Sound play
        private void buttonPlay_Click(object sender, EventArgs e)
        {
            if (RealSound.IsPlaying() == false)
            {
                SoundItemProperty soundProp = (SoundItemProperty)propertySoundInfo.SelectedObject;
                if (soundProp == null)
                    return;

                RealSound.Play(Config.GetOption("WORKINGDIR"), soundProp);
            }
            else
                RealSound.Stop();
        }
        private void trackTimeLine_Scroll(object sender, EventArgs e)
        {
            RealSound.SetPlayPosition((uint)trackTimeLine.Value);
        }

        // Update timer
        private void timerUpdate_Tick(object sender, EventArgs e)
        {
            RealSound.SetSoundPosition(soundPosition.X, soundPosition.Y);
            RealSound.Update();

            buttonPlay.Text = RealSound.IsPlaying() ? "Stop" : "Play";

            int length = (int)RealSound.GetPlayTime();
            int pos = (int)RealSound.GetPlayPosition();
            trackTimeLine.SetRange(0, length);
            trackTimeLine.Value = pos;

            length /= 1000;
            pos /= 1000;
            labelPlayTime.Text = string.Format("{0:D2}:{1:D2} / {2:D2}:{3:D2}", (int)(pos / 60), pos % 60, (int)(length / 60), length % 60);
        }
    }
}
