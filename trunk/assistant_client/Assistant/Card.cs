using System;

using System.Collections.Generic;
using System.Text;

namespace Assistant
{
    class Card
    {
        private int cardNumber = 0;
        public int CardNumber
        {
            get { return cardNumber; }
            set { cardNumber = value; }
        }

        private string cardContents = "";
        public string CardContents
        {
            get { return cardContents; }
            set { cardContents = value; }
        }

        public Card() { }
        public Card(int num, string contents)
        {
            this.CardNumber = num;
            this.CardContents = contents;
        }

        public static Card GetCardByNum(List<Card> cards, int num)
        {
            Card resCard = null;
            foreach (Card card in cards)
            {
                if (card.CardNumber == num)
                {
                    resCard = card;
                    break;
                }
            }

            return resCard;
        }
    }
}
