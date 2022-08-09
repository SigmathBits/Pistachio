using System;


namespace Pistachio
{

    public struct Vector2
    {
        public float X, Y;

        public static Vector2 Zero => new Vector2(0.0f, 0.0f);

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public Vector2(float scalar)
        {
            X = scalar;
            Y = scalar;
        }

        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y);
        }

        public Vector2 Normalised()
        {
            float length = Length();
            if (length == 0)
            {
                return this;
            }

            return this / length;
        }

        public static Vector2 operator *(Vector2 vector, float scalar)
        {
            return new Vector2(scalar * vector.X, scalar * vector.Y);
        }

        public static Vector2 operator *(float scalar, Vector2 vector)
        {
            return new Vector2(scalar * vector.X, scalar * vector.Y);
        }

        public static Vector2 operator /(Vector2 vector, float scalar)
        {
            return new Vector2(vector.X / scalar, vector.Y / scalar);
        }

        public static Vector2 operator +(Vector2 vectorA, Vector2 vectorB)
        {
            return new Vector2(vectorA.X + vectorB.X, vectorA.Y + vectorB.Y);
        }

        public static Vector2 operator -(Vector2 vectorA, Vector2 vectorB)
        {
            return new Vector2(vectorA.X - vectorB.X, vectorA.Y - vectorB.Y);
        }
    }

}
