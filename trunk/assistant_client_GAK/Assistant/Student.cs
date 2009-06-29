using System;

using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace Assistant
{
    public class Student
    {

        public class SubjectMark 
        {
            public string subject = "";
            public int mark = 0;
        }

        public class StudentUpdate
        {
            public Student student;
            public Student newStudent;
        }

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

        private int mark6 = 0;
        public int Mark6
        {
            get { return mark6; }
            set { mark6 = value; }
        }

        public List<SubjectMark> subjectMarks = null;

        public Student() 
        {
            subjectMarks = new List<SubjectMark>();
        }

        public Student(int id, string surname, string name, string patronymic, string theme,
            int mark1, int mark2, int mark3, int mark4, int mark5, int mark6)
        {
            this.ID = id;
            this.Surname = surname;
            this.Name = name;
            this.Patronymic = patronymic;
            this.Theme = theme;
            this.Mark1 = mark1;
            this.Mark2 = mark2;
            this.Mark3 = mark3;
            this.Mark4 = mark4;
            this.Mark5 = mark5;
            this.Mark6 = mark6;
            subjectMarks = new List<SubjectMark>();
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

    }
}
