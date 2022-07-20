using System;


namespace Pistachio
{
    public class Main
    {
        public float FloatVar { get; set; }

        public int IntVar = 7;

        public Main()
        {
            Console.WriteLine("Main Constructor!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Greetings, Equestria! - C#");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        public void PrintInts(int valueA, int valueB)
        {
            Console.WriteLine($"C# says: {valueA} & {valueB}");
        }

        public void PrintFloatVar()
        {
            Console.WriteLine($"Current float value: {FloatVar}");
        }

        public void PrintIntVar()
        {
            Console.WriteLine($"Current int value: {IntVar}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
    }

}
