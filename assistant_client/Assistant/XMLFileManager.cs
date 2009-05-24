using System;

using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Windows.Forms;
/* FIXME: remove windows forms from class */

namespace Assistant
{
    public class ExamInfo
    {
        public string examType = "";
        public int examId = 0;
        public int memberId = 0;
        public int studentCount = 0;
    }
    
    class XMLFileManager
    {
        public static ExamInfo GetExamInfo(string fileName)
        {
            ExamInfo exam = new ExamInfo();
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);
                XmlNode root = doc.DocumentElement;

                exam.examType = root.Attributes["examType"].Value;
                exam.examId = Convert.ToInt32(root.Attributes["examId"].Value);
                exam.memberId = Convert.ToInt32(root.Attributes["memberId"].Value);
                exam.studentCount = Convert.ToInt32(root.Attributes["studentCount"].Value);
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения XML: " + err);
            }

            return exam;
        }
        
        public static List<Student> GetStudentList(string fileName)
        {
            List<Student> students = new List<Student>();
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);
                XmlNodeList nodeList = doc.DocumentElement.ChildNodes;

                foreach (XmlNode studentNode in nodeList)
                {
                    Student student = new Student();
                    student.ID = Convert.ToInt32(studentNode.Attributes["id"].Value);
                    student.Surname = studentNode.Attributes["surname"].Value;
                    student.Name = studentNode.Attributes["name"].Value;
                    student.Patronymic = studentNode.Attributes["patronymic"].Value;
                    student.CardNumber = Convert.ToInt32(studentNode.Attributes["cardNumber"].Value);

                    XmlNodeList marks = studentNode.ChildNodes;
                    foreach (XmlNode markNode in marks) 
                    {
                        if (markNode.Name == "marks") 
                        {
                            XmlNodeList studentMarks = markNode.ChildNodes;
                            foreach (XmlNode studentMark in studentMarks)
                            {
                                Student.SubjectMark subjMark = new Student.SubjectMark();
                                subjMark.subject = studentMark.Attributes["subject"].Value;
                                subjMark.mark = Convert.ToInt32(studentMark.InnerText);
                                student.subjectMarks.Add(subjMark);
                            }
                        }
                        else if (markNode.Name == "memberMarks")
                        {
                            XmlNodeList memberMarks = markNode.ChildNodes;
                            foreach (XmlNode memberMark in memberMarks)
                            {
                                if (memberMark.Name == "mark1")
                                    student.Mark1 = Convert.ToInt32(markNode.InnerText);
                                else if (memberMark.Name == "mark2")
                                    student.Mark2 = Convert.ToInt32(markNode.InnerText);
                                else if (memberMark.Name == "mark3")
                                    student.Mark3 = Convert.ToInt32(markNode.InnerText);
                                else if (memberMark.Name == "resMark")
                                    student.ResultMark = Convert.ToInt32(markNode.InnerText);
                            }
                        }
                    }

                    students.Add(student);
                }
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения XML: " + err);
            }

            return students;
        }

        public static void SaveStudent(Student student, string fileName)
        {
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);

                XmlNode stud;
                XmlNode root = doc.DocumentElement;

                stud = root.SelectSingleNode("/students/student[@id='" + student.ID.ToString() + "']/memberMarks");

                XmlNodeList memberMarks = stud.ChildNodes;
                foreach (XmlNode markNode in memberMarks)
                {
                    if (markNode.Name == "mark1")
                        markNode.InnerText = student.Mark1.ToString();
                    else if (markNode.Name == "mark2")
                        markNode.InnerText = student.Mark2.ToString();
                    else if (markNode.Name == "mark3")
                        markNode.InnerText = student.Mark3.ToString();
                    else if (markNode.Name == "resMark")
                        markNode.InnerText = student.ResultMark.ToString();
                }

                doc.Save(fileName);
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения/записи XML: " + err);
            }
        }

        public static void SaveMemberId(string fileName, int memberId)
        {
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);

                XmlNode root = doc.DocumentElement;
                root.Attributes["memberId"].Value = memberId.ToString();

                doc.Save(fileName);
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения/записи XML: " + err);
            }
        }

        public static List<Card> GetCardList(string fileName)
        {
            List<Card> cards = new List<Card>();
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);

                XmlNodeList cardNodes = doc.DocumentElement.ChildNodes;

                foreach (XmlNode cardNode in cardNodes)
                {
                    Card card = new Card();
                    card.CardNumber = Convert.ToInt32(cardNode.Attributes["number"].Value);
                    card.CardContents = cardNode.InnerText;

                    cards.Add(card);
                }
                
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения/записи XML: " + err);
            }

            return cards;
        }

        public static void LoadConfig(string fileName, out string ip, out int port, out string userName)
        {
            XmlDocument doc = new XmlDocument();
            
            try
            {
                doc.Load(fileName);

                XmlNode node = doc.SelectSingleNode("/config/ip");
                ip = node.InnerText;
                node = doc.SelectSingleNode("/config/port");
                port = Convert.ToInt32(node.InnerText);
                node = doc.SelectSingleNode("/config/username");
                userName = node.InnerText;

            }
            catch (Exception err)
            {
                ip = null;
                port = 0;
                userName = null;
                MessageBox.Show("Ошибка чтения/записи XML: " + err);
            }
        }

        public static void SaveConfig(string fileName, string ip, int port, string userName)
        {
            XmlDocument doc = new XmlDocument();
            
            try
            {
                doc.LoadXml("<config><ip></ip><port></port><username></username></config>");
                
                XmlNode node = doc.SelectSingleNode("/config/ip");
                node.InnerText = ip;
                node = doc.SelectSingleNode("/config/port");
                node.InnerText = port.ToString();
                node = doc.SelectSingleNode("/config/username");
                node.InnerText = userName;

                doc.Save(fileName);
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения/записи XML: " + err);
            }
        }
    }
}
