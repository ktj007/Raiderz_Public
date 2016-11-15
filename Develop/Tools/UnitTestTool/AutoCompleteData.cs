using System;
using System.Collections.Generic;
using System.Text;

namespace UnitTestTool
{
    enum STAGE_TYPE
    {
        ST_NONE = 0,

        ST_1,
        ST_2,
        ST_3,
        ST_DB,
        ST_ALL,

        ST_MAX
    }

    class AutoCompleteNode
    {
        private char _char = '\0';
        private bool isWord = false;
        private List<AutoCompleteNode> nodeList = new List<AutoCompleteNode>();

        public AutoCompleteNode(char key)
        {
            this._char = key;
        }

        public char _Char
        {
            get { return _char; }
            set { _char = value; }
        }

        public bool IsWord
        {
            get { return isWord; }
            set { isWord = value; }
        }

        public List<AutoCompleteNode> NodeList
        {
            get { return nodeList; }
            set { nodeList = value; }
        }
    }

    class AutoCompleteData
    {
        private AutoCompleteNode stage1SuiteNode = new AutoCompleteNode('\0');
        private AutoCompleteNode stage1TestNode = new AutoCompleteNode('\0');
        private AutoCompleteNode stage2SuiteNode = new AutoCompleteNode('\0');
        private AutoCompleteNode stage2TestNode = new AutoCompleteNode('\0');
        private AutoCompleteNode stage3SuiteNode = new AutoCompleteNode('\0');
        private AutoCompleteNode stage3TestNode = new AutoCompleteNode('\0');
        private AutoCompleteNode DBSuiteNode = new AutoCompleteNode('\0');
        private AutoCompleteNode DBTestNode = new AutoCompleteNode('\0');
        
        public void InsertWord(string word, STAGE_TYPE type, bool isSuite)
        {
            AutoCompleteNode curNode = null;
            switch (type)
            {
                case STAGE_TYPE.ST_1: curNode = (true == isSuite) ? stage1SuiteNode : stage1TestNode; break;
                case STAGE_TYPE.ST_2: curNode = (true == isSuite) ? stage2SuiteNode : stage2TestNode; break;
                case STAGE_TYPE.ST_3: curNode = (true == isSuite) ? stage3SuiteNode : stage3TestNode; break;
                case STAGE_TYPE.ST_DB: curNode = (true == isSuite) ? DBSuiteNode : DBTestNode; break;
                default: return;
            }
                        
            foreach (char _char in word)
            {
                compareKey = _char;
                AutoCompleteNode existNode = curNode.NodeList.Find(IsEqualKeay);
                if (null == existNode)
                {
                    curNode.NodeList.Insert(0, new AutoCompleteNode(_char));
                    curNode = curNode.NodeList[0];
                }
                else
                {
                    curNode = existNode;
                }
            }

            curNode.IsWord = true;
        }

        private static char compareKey;
        private static bool IsEqualKeay(AutoCompleteNode node)
        {
            if (compareKey != node._Char) return false;

            return true;
        }

        public void GetWordByStage(Queue<string> qWord, STAGE_TYPE stage, bool isSuite)
        {
            AutoCompleteNode curNode = null;
            switch (stage)
            {
                case STAGE_TYPE.ST_1: curNode = (true == isSuite) ? stage1SuiteNode : stage1TestNode; break;
                case STAGE_TYPE.ST_2: curNode = (true == isSuite) ? stage2SuiteNode : stage2TestNode; break;
                case STAGE_TYPE.ST_3: curNode = (true == isSuite) ? stage3SuiteNode : stage3TestNode; break;
                case STAGE_TYPE.ST_DB: curNode = (true == isSuite) ? DBSuiteNode : DBTestNode; break;
                default: return;
            }

            foreach (AutoCompleteNode childNode in curNode.NodeList)
            {
                string word = "";
                GetWordByStage(qWord, word, childNode);
            }
        }

        private void GetWordByStage(Queue<string> qWord, string word, AutoCompleteNode node)
        {
            word += node._Char;
            if (true == node.IsWord)
            {
                qWord.Enqueue(word);
            }

            foreach (AutoCompleteNode childNode in node.NodeList)
            {
                GetWordByStage(qWord, word, childNode);
            }
        }

        public void GetWordByKeyWord(Queue<string> qWord, string strStage, string label, string keyWord)
        {
            STAGE_TYPE nStage = STAGE_TYPE.ST_NONE;            
            if ("stage1" == strStage)
            {
                nStage = STAGE_TYPE.ST_1;
            }
            else if ("stage2" == strStage)
            {
                nStage = STAGE_TYPE.ST_2;
            }
            else if ("stage3" == strStage)
            {
                nStage = STAGE_TYPE.ST_3;
            }
            else if ("db" == strStage)
            {
                nStage = STAGE_TYPE.ST_DB;
            }
            else if ("all" == strStage)
            {
                nStage = STAGE_TYPE.ST_ALL;
            }
            else
            {
                return;
            }

            bool bSuite=false;            
            if ("Test1" == label || "Test2" == label || "Test3" == label || "Test4" == label)
            {
                bSuite = false;
            }
            else if ("TestSuite1" == label || "TestSuite2" == label || "TestSuite3" == label || "TestSuite4" == label ||
                "DisabledSuite1" == label || "DisabledSuite2" == label || "DisabledSuite3" == label || "DisabledSuite4" == label)
            {
                bSuite = true;
            }
            else
            {
                return;
            }

            GetWordByKeyWord(qWord, nStage, bSuite, keyWord);
        }

        private void GetWordByKeyWord(Queue<string> qWord, STAGE_TYPE stage, bool isSuite, string keyWord)
        {
            int nIndex = 0;

            AutoCompleteNode curNode = null;
            switch (stage)
            {
                case STAGE_TYPE.ST_1:
                    {
                        curNode = (true == isSuite) ? stage1SuiteNode : stage1TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                    }
                    break;
                case STAGE_TYPE.ST_2:
                    {
                        curNode = (true == isSuite) ? stage2SuiteNode : stage2TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                    }
                    break;
                case STAGE_TYPE.ST_3:
                    {
                        curNode = (true == isSuite) ? stage3SuiteNode : stage3TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                    }
                    break;
                case STAGE_TYPE.ST_DB:
                    {
                        curNode = (true == isSuite) ? DBSuiteNode : DBTestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                    }
                    break;

                case STAGE_TYPE.ST_ALL:
                    {
                        curNode = (true == isSuite) ? stage1SuiteNode : stage1TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                        curNode = (true == isSuite) ? stage2SuiteNode : stage2TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                        curNode = (true == isSuite) ? stage3SuiteNode : stage3TestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                        curNode = (true == isSuite) ? DBSuiteNode : DBTestNode;
                        foreach (AutoCompleteNode childNode in curNode.NodeList)
                        {
                            GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
                        }
                    }
                    break;
                default: return;
            }

            
        }

        private void GetWordByKeyWord(Queue<string> qWord, string keyWord, int nIndex, AutoCompleteNode node)
        {
            if (nIndex >= keyWord.Length) return;
            if (node._Char != keyWord[nIndex]) return;
            
            if (true == node.IsWord)
            {
                string word = keyWord.Substring(0, nIndex);
                qWord.Enqueue(word);
            }

            nIndex++;

            foreach (AutoCompleteNode childNode in node.NodeList)
            {
                GetWordByKeyWord(qWord, keyWord, nIndex, childNode);
            }
        }
    }
}
