using System;

using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Windows.Forms;

namespace Assistant
{
    class Student
    {
        private int id = 0;
        public int ID
        {
            get { return id; }
            set { id = value; }
        }

        private string surname = "";
        public string Surname
        {
            get { return surname; }
            set { surname = value; }
        }

        private string name = "";
        public string Name
        {
            get { return name; }
            set { name = value; }
        }

        private string patronymic = "";
        public string Patronymic
        {
            get { return patronymic; }
            set { patronymic = value; }
        }

        public string FIO
        {
            get { return (this.Surname + " " + this.Name + " " + this.Patronymic); }
        }

        private string theme = "";
        public string Theme
        {
            get { return theme; }
            set { theme = value; }
        }

        private string instructor = "";
        public String Instructor
        {
            get { return instructor; }
            set { instructor = value; }
        }

        private string consultant = "";
        public string Consultant
        {
            get { return consultant; }
            set { consultant = value; }
        }

        private int mark1 = 0;
        public int Mark1
        {
            get { return mark1; }
            set { mark1 = value; }
        }

        private int mark2 = 0;
        public int Mark2
        {
            get { return mark2; }
            set { mark2 = value; }
        }

        private int mark3 = 0;
        public int Mark3
        {
            get { return mark3; }
            set { mark3 = value; }
        }

        private int mark4 = 0;
        public int Mark4
        {
            get { return mark4; }
            set { mark4 = value; }
        }

        private int mark5 = 0;
        public int Mark5
        {
            get { return mark5; }
            set { mark5 = value; }
        }

        private int resultMark = 0;
        public int ResultMark
        {
            get { return resultMark; }
            set { resultMark = value; }
        }

        public Student() { }

        public Student(int id, string surname, string name, string patronymic,
            string theme, string instructor, string consultant,
            int mark1, int mark2, int mark3, int mark4, int mark5,
            int memberMark, int resultMark)
        {
            this.ID = id;
            this.Surname = surname;
            this.Name = name;
            this.Patronymic = patronymic;
            this.Theme = theme;
            this.Instructor = instructor;
            this.Consultant = consultant;
            this.Mark1 = mark1;
            this.Mark2 = mark2;
            this.Mark3 = mark3;
            this.Mark4 = mark4;
            this.Mark5 = mark5;
            this.ResultMark = resultMark;
        }

        public static List<Student> GetStudentList(string fileName)
        {
            List<Student> students = new List<Student>();
            //XmlTextReader reader = null;
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
                    student.Theme = studentNode.Attributes["theme"].Value; 
                    student.Instructor = studentNode.Attributes["instructor"].Value;
                    student.Consultant = studentNode.Attributes["consultant"].Value;

                    XmlNodeList marks = studentNode.ChildNodes;
                    foreach (XmlNode markNode in marks)
                    {
                        if (markNode.Name == "mark1")
                            student.Mark1 = Convert.ToInt32(markNode.Attributes["val"].Value);
                        else if (markNode.Name == "mark2")
                            student.Mark2 = Convert.ToInt32(markNode.Attributes["val"].Value);
                        else if (markNode.Name == "mark3")
                            student.Mark3 = Convert.ToInt32(markNode.Attributes["val"].Value);
                        else if (markNode.Name == "mark4")
                            student.Mark4 = Convert.ToInt32(markNode.Attributes["val"].Value);
                        else if (markNode.Name == "mark5")
                            student.Mark5 = Convert.ToInt32(markNode.Attributes["val"].Value);
                        else if (markNode.Name == "resultMark")
                            student.ResultMark = Convert.ToInt32(markNode.Attributes["val"].Value);
                    }

                    students.Add(student);
                }
                /*reader = new XmlTextReader(fileName);
                while (reader.Read())
                {
                    Student student = new Student();
                    if (reader.NodeType == XmlNodeType.Element)
                    {
                        if (reader.Name == "student")
                        {
                            student.ID = Convert.ToInt32(reader.GetAttribute("id"));
                            student.Surname = reader.GetAttribute("surname");
                            student.Name = reader.GetAttribute("name");
                            student.Patronymic = reader.GetAttribute("patronymic");

                            while (reader.Read())
                            {
                                if (reader.Name == "Theme")
                                    student.Theme = reader.Value.ToString();
                                if (reader.Name == "Instructor")
                                    student.Instructor = reader.Value.ToString();
                                if (reader.Name == "Consultant")
                                    student.Consultant = reader.Value.ToString();
                                if (reader.Name == "mark1")
                                {
                                    reader.Read();
                                    student.Mark1 = Convert.ToInt32(reader.Value.ToString());
                                }
                                if (reader.Name == "mark2")
                                    student.Mark2 = Convert.ToInt32(reader.Value.ToString());
                                if (reader.Name == "mark3")
                                    student.Mark3 = Convert.ToInt32(reader.Value.ToString());
                                if (reader.Name == "mark4")
                                    student.Mark4 = Convert.ToInt32(reader.Value.ToString());
                                if (reader.Name == "mark5")
                                    student.Mark5 = Convert.ToInt32(reader.Value.ToString());
                                if (reader.Name == "resultMark")
                                {
                                    student.resultMark = Convert.ToInt32(reader.Value.ToString());
                                    break;
                                }
                            }
                            students.Add(student);
                        }
                    }
                }*/
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения XML: " + err);
            }
            /*finally
            {
                if (reader != null)
                    reader.Close();
            }*/

            return students;
        }

        public static Student GetStudentById(List<Student> students, int id)
        {
            Student student = null;
            foreach (Student st in students)
            {
                if (st.ID == id)
                {
                    student = st;
                    break;
                }
            }
            return student;
        }

        public static void SaveStudentList(List<Student> students, string fileName)
        {
            XmlDocument doc = new XmlDocument();

            try
            {
                doc.Load(fileName);

                foreach (Student student in students)
                {
                    XmlElement el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() +"']/mark1");
                    el.SetAttribute("val", student.Mark1.ToString());
                    el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() + "']/mark2");
                    el.SetAttribute("val", student.Mark2.ToString());
                    el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() + "']/mark3");
                    el.SetAttribute("val", student.Mark3.ToString());
                    el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() + "']/mark4");
                    el.SetAttribute("val", student.Mark4.ToString());
                    el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() + "']/mark5");
                    el.SetAttribute("val", student.Mark5.ToString());
                    el = (XmlElement)doc.SelectSingleNode("/students/student [@id='" + student.ID.ToString() + "']/resultMark");
                    el.SetAttribute("val", student.ResultMark.ToString());
                }

                doc.Save(fileName);
            }
            catch (Exception err)
            {
                MessageBox.Show("Ошибка чтения XML: " + err);
            }
        }
    }
}
