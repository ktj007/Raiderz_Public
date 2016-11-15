
using System;
using System.ComponentModel;
using System.Collections;
using System.Xml;
using System.IO;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace SoundManager
{
    public class SoundNode
    {
        public SoundNode Parent;
        public string Name;

        public virtual bool IsGroup()
        {
            return false;
        }
    }
    public class SoundItem : SoundNode
    {
        public string Filename = "";
        public string Type = SoundItemProperty.typeSound[0];
        public int Volume = 100;
        public bool Loop = false;
        public bool Unique = false;
        public int MinDistance = 100;
        public int MaxDistance = 1000;
        public int Priority = 128;
    }
    public class SoundGroup : SoundNode
    {
        public ArrayList Nodes = new ArrayList();


        public override bool IsGroup()
        {
            return true;
        }
        public bool Remove(SoundNode soundNode)
        {
            foreach (SoundNode childNode in Nodes)
            {
                if (childNode == soundNode)
                {
                    Nodes.Remove(childNode);
                    return true;
                }

                if (childNode.IsGroup() == true)
                {
                    if (((SoundGroup)childNode).Remove(soundNode) == true)
                        return true;
                }
            }

            return false;
        }
    }

    public static class SoundContainer
    {
        public static string[] ControlName = { "volume", "direct", "reverb", "lowpass", "panlevel" };

        public static SoundGroup Sounds = new SoundGroup();


        public static bool LoadSource(string fileName)
        {
            Sounds.Nodes.Clear();

            if (File.Exists(fileName) == false)
                return false;

            XmlDocument xmlDocument = new XmlDocument();
            xmlDocument.Load(fileName);

            XmlElement elementRoot = xmlDocument.DocumentElement;
            XmlNodeList nodeLists = elementRoot.ChildNodes;
            for (int i = 0; i < nodeLists.Count; i++)
            {
                XmlNode nodeControls = nodeLists[i];
                if (nodeControls.Name != "Controls")
                    continue;

                XmlNodeList nodelistControl = nodeControls.ChildNodes;
                for (int j = 0; j < nodelistControl.Count; j++)
                {
                    XmlNode nodeControl = nodelistControl[j];
                    if (nodeControl.Name != "Control")
                        continue;

                    string name = "";
                    PointF start = new PointF(0.0f, 0.0f);
                    PointF start_vel = new PointF(0.0f, 0.0f);
                    PointF end = new PointF(0.0f, 0.0f);
                    PointF end_vel = new PointF(0.0f, 0.0f);
                    for (int k = 0; k < nodeControl.Attributes.Count; k++)
                    {
                        XmlAttribute attrSound = nodeControl.Attributes[k];
                        if (attrSound.Name == "name") name = attrSound.Value;
                        else if (attrSound.Name == "pos_sx") start.X = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "pos_sy") start.Y = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "vel_sx") start_vel.X = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "vel_sy") start_vel.Y = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "pos_ex") end.X = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "pos_ey") end.Y = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "vel_ex") end_vel.X = float.Parse(attrSound.Value);
                        else if (attrSound.Name == "vel_ey") end_vel.Y = float.Parse(attrSound.Value);
                    }

                    for (int control = 0; control < (int)RealSound.CONTROLTYPE.MAX; control++)
                    {
                        if (ControlName[control] != name)
                            continue;

                        RealSound.CONTROLS[control].Start = start;
                        RealSound.CONTROLS[control].StartVel = start_vel;
                        RealSound.CONTROLS[control].End = end;
                        RealSound.CONTROLS[control].EndVel = end_vel;
                    }
                }
            }

            return loadResourceParsing(nodeLists, Sounds);
        }
        private static bool loadResourceParsing(XmlNodeList nodeLists, SoundGroup soundParentGroup)
        {
            // Read nodes
            for (int i = 0; i < nodeLists.Count; i++)
            {
                XmlNode nodeList = nodeLists[i];

                // Read sound
                if (nodeList.Name == "SOUND")
                {
                    string name = "";
                    string fname = "";
                    string type = "";
                    int volume = 100;
                    bool loop = false;
                    bool unique = false;
                    int mindist = 100;
                    int maxdist = 1000;
                    int priority = 128;

                    for (int j = 0; j < nodeList.Attributes.Count; j++)
                    {
                        XmlAttribute attrSound = nodeList.Attributes[j];
                        if (attrSound.Name == "name") name = attrSound.Value;
                        else if (attrSound.Name == "filename") fname = attrSound.Value;
                        else if (attrSound.Name == "type") type = attrSound.Value;
                        else if (attrSound.Name == "volume") volume = (int)(double.Parse(attrSound.Value) * 100.0);
                        else if (attrSound.Name == "loop") loop = bool.Parse(attrSound.Value);
                        else if (attrSound.Name == "unique") unique = bool.Parse(attrSound.Value);
                        else if (attrSound.Name == "min_dist") mindist = int.Parse(attrSound.Value);
                        else if (attrSound.Name == "max_dist") maxdist = int.Parse(attrSound.Value);
                        else if (attrSound.Name == "priority") priority = int.Parse(attrSound.Value);
                    }


                    SoundItem soundItem = new SoundItem();
                    soundItem.Parent = soundParentGroup;
                    soundItem.Name = name;
                    soundItem.Filename = fname;
                    soundItem.Type = type;
                    soundItem.Volume = volume;
                    soundItem.Loop = loop;
                    soundItem.Unique = unique;
                    soundItem.MinDistance = mindist;
                    soundItem.MaxDistance = maxdist;
                    soundItem.Priority = priority;
                    soundParentGroup.Nodes.Add(soundItem);


                    // Read controls
                    XmlNodeList nodelistControl = nodeList.ChildNodes;
                    for (int j = 0; j < nodelistControl.Count; j++)
                    {
                        XmlNode nodeControl = nodelistControl[j];
                        if (nodeControl.Name != "CONTROL") continue;


                        string nameControl = "";
                        string typeControl = "";
                        double scaleControl = 1.0;

                        for (int k = 0; k < nodeControl.Attributes.Count; k++)
                        {
                            XmlAttribute attrControl = nodeControl.Attributes[k];
                            if (attrControl.Name == "name") nameControl = attrControl.Value;
                            else if (attrControl.Name == "type") typeControl = attrControl.Value;
                            else if (attrControl.Name == "volume") scaleControl = Double.Parse(attrControl.Value);
                        }
                    }
                }

                // Read group
                else if (nodeList.Name == "GROUP")
                {
                    string name = "";

                    for (int j = 0; j < nodeList.Attributes.Count; j++)
                    {
                        XmlAttribute attrGroup = nodeList.Attributes[j];
                        if (attrGroup.Name == "name") name = attrGroup.Value;
                    }

                    SoundGroup soundGroup = new SoundGroup();
                    soundGroup.Parent = soundParentGroup;
                    soundGroup.Name = name;
                    soundParentGroup.Nodes.Add(soundGroup);

                    XmlNodeList nodelistNext = nodeList.ChildNodes;
                    if (loadResourceParsing(nodelistNext, soundGroup) == false)
                        return false;
                }
            }

            return true;
        }

        public static bool SaveSource(string fileName)
        {
            if (File.Exists(fileName) == false)
                return false;

            XmlDocument xmlDocument = new XmlDocument();
            XmlNode docDeclaration = xmlDocument.CreateXmlDeclaration("1.0", "UTF-8", null);
            xmlDocument.AppendChild(docDeclaration);

            XmlElement elementRoot = xmlDocument.CreateElement("DATA");
            xmlDocument.AppendChild(elementRoot);

            XmlElement elementControls = xmlDocument.CreateElement("Controls");
            elementRoot.AppendChild(elementControls);
            for (int control = 0; control < (int)RealSound.CONTROLTYPE.MAX; control++)
            {
                XmlElement elementControl = xmlDocument.CreateElement("Control");
                elementControls.AppendChild(elementControl);

                elementControl.SetAttribute("name", ControlName[control]);
                elementControl.SetAttribute("pos_sx", RealSound.CONTROLS[control].Start.X.ToString());
                elementControl.SetAttribute("pos_sy", RealSound.CONTROLS[control].Start.Y.ToString());
                elementControl.SetAttribute("vel_sx", RealSound.CONTROLS[control].StartVel.X.ToString());
                elementControl.SetAttribute("vel_sy", RealSound.CONTROLS[control].StartVel.Y.ToString());
                elementControl.SetAttribute("pos_ex", RealSound.CONTROLS[control].End.X.ToString());
                elementControl.SetAttribute("pos_ey", RealSound.CONTROLS[control].End.Y.ToString());
                elementControl.SetAttribute("vel_ex", RealSound.CONTROLS[control].EndVel.X.ToString());
                elementControl.SetAttribute("vel_ey", RealSound.CONTROLS[control].EndVel.Y.ToString());
            }

            if (saveSourceChildNode(xmlDocument, elementRoot, Sounds) == false)
                return false;

            xmlDocument.Save(fileName);

            return true;
        }
        public static bool saveSourceChildNode(XmlDocument xmlDocument, XmlElement xmlParentElement, SoundGroup listSounds)
        {
            foreach (SoundNode node in listSounds.Nodes)
            {
                if (node.IsGroup() == true)
                {
                    SoundGroup soundGroup = (SoundGroup)node;
                    XmlElement elementSoundGroup = xmlDocument.CreateElement("GROUP");
                    elementSoundGroup.SetAttribute("name", soundGroup.Name);
                    xmlParentElement.AppendChild(elementSoundGroup);

                    if (saveSourceChildNode(xmlDocument, elementSoundGroup, soundGroup) == false)
                        return false;
                }
                else
                {
                    SoundItem soundItem = (SoundItem)node;
                    XmlElement elementSoundItem = xmlDocument.CreateElement("SOUND");
                    elementSoundItem.SetAttribute("name", soundItem.Name);
                    elementSoundItem.SetAttribute("filename", soundItem.Filename);
                    elementSoundItem.SetAttribute("type", soundItem.Type);
                    elementSoundItem.SetAttribute("volume", ((double)soundItem.Volume * 0.01).ToString());

                    if (soundItem.Loop == true)
                        elementSoundItem.SetAttribute("loop", soundItem.Loop.ToString());

                    if (soundItem.Unique == true)
                        elementSoundItem.SetAttribute("unique", soundItem.Unique.ToString());

                    if (soundItem.Type == SoundItemProperty.typeSound[1])
                    {
                        elementSoundItem.SetAttribute("min_dist", soundItem.MinDistance.ToString());
                        elementSoundItem.SetAttribute("max_dist", soundItem.MaxDistance.ToString());
                        elementSoundItem.SetAttribute("priority", soundItem.Priority.ToString());
                    }

                    xmlParentElement.AppendChild(elementSoundItem);
                }
            }

            return true;
        }

        public static void InsertSound(SoundNode soundNode, out SoundItem newSoundItem, out int index)
        {
            newSoundItem = null;
            index = -1;


            SoundItem newSound = new SoundItem();
            newSound.Name = "*new sound";

            if (soundNode == null)
            {
                newSound.Parent = Sounds;
                index = Sounds.Nodes.Add(newSound);
            }
            else if (soundNode.IsGroup() == true)
            {
                SoundGroup soundGroup = (SoundGroup)soundNode;
                newSound.Parent = soundNode.Parent;
                index = soundGroup.Nodes.Add(newSound);
            }
            else
            {
                SoundItem soundItem = (SoundItem)soundNode;
                SoundGroup group = (SoundGroup)soundItem.Parent;
                newSound.Parent = group;
                newSound.Type = soundItem.Type;
                newSound.Volume = soundItem.Volume;
                newSound.Loop = soundItem.Loop;
                newSound.Unique = soundItem.Unique;
                newSound.MinDistance = soundItem.MinDistance;
                newSound.MaxDistance = soundItem.MaxDistance;
                newSound.Priority = soundItem.Priority;

                int i;
                for (i = 0; i < group.Nodes.Count; i++)
                {
                    if (group.Nodes[i] == soundNode)
                        break;
                }

                i++;
                if (i >= group.Nodes.Count)
                    i = group.Nodes.Add(newSound);
                else
                    group.Nodes.Insert(i, newSound);

                index = i;
            }

            newSoundItem = newSound;
        }
        public static void InsertGroup(SoundNode soundNode, out SoundGroup newSoundGroup, out int index)
        {
            newSoundGroup = null;
            index = -1;


            SoundGroup newGroup = new SoundGroup();
            newGroup.Name = "*new group";

            if (soundNode == null)
            {
                newGroup.Parent = Sounds;
                index = Sounds.Nodes.Add(newGroup);
            }
            else if (soundNode.IsGroup() == true)
            {
                SoundGroup soundGroup = (SoundGroup)soundNode;
                newGroup.Parent = soundGroup;
                index = soundGroup.Nodes.Add(newGroup);
            }
            else
            {
                SoundGroup parentGroup = (SoundGroup)soundNode.Parent;
                newGroup.Parent = parentGroup;

                int i;
                for (i = 0; i < parentGroup.Nodes.Count; i++)
                {
                    if (parentGroup.Nodes[i] == soundNode)
                        break;
                }

                i++;
                if (i >= parentGroup.Nodes.Count)
                    i = parentGroup.Nodes.Add(newGroup);
                else
                    parentGroup.Nodes.Insert(i, newGroup);

                index = i;
            }

            newSoundGroup = newGroup;
        }

        public static void Clear()
        {
            Sounds.Nodes.Clear();
        }
    }
}