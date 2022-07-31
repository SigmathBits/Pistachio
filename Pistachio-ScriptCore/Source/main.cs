using System;
using System.Runtime.CompilerServices;

namespace Pistachio
{

    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector3(ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Cross(ref Vector3 vectorA, ref Vector3 vectorB, out Vector3 result);


        public static Vector3 Cross(ref Vector3 vectorA, ref Vector3 vectorB)
        {
            Cross(ref vectorA, ref vectorB, out Vector3 result);
            return result;
        }

        public static void Log(string text, int parameter)
        {
            NativeLog(text, parameter);
        }

        public static void Log(ref Vector3 vector)
        {
            NativeLog_Vector3(ref vector);
        }
    }

    public class Entity
    {

        public float FloatVar { get; set; }

        public int IntVar = 7;

        public Entity()
        {
            Console.WriteLine("Main Constructor!");

            Vector3 position = new Vector3(6.5f, 3.6f, 2.7f);
            InternalCalls.Log(ref position);

            Vector3 vectorA = new Vector3(1.2f, 2.3f, 1.0f);
            Vector3 vectorB = new Vector3(2.2f, 6.0f, 12.2f);
            Vector3 result = InternalCalls.Cross(ref vectorA, ref vectorB);
            InternalCalls.Log(ref result);

            InternalCalls.Log("Sigmath Bits", 6);
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
