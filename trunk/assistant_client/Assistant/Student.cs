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
            public Card card;
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
        
        private int cardNumber = 0;
        public int CardNumber
        {
            get { return cardNumber; }
            set { cardNumber = value; }
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

        private int resultMark = 0;
        public int ResultMark
        {
            get { return resultMark; }
            set { resultMark = value; }
        }

        public List<SubjectMark> subjectMarks = null;

        public Student() 
        {
            subjectMarks = new List<SubjectMark>();
        }

        public Student(int id, string surname, string name, string patronymic,
            int cardNumber, int mark1, int mark2, int mark3, int memberMark, int resultMark)
        {
            this.ID = id;
            this.Surname = surname;
            this.Name = name;
            this.Patronymic = patronymic;
            this.cardNumber = cardNumber;
            this.Mark1 = mark1;
            this.Mark2 = mark2;
            this.Mark3 = mark3;
            this.ResultMark = resultMark;
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
