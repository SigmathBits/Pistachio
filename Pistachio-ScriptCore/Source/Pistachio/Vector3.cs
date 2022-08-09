using System;


namespace Pistachio
{

    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f, 0.0f, 0.0f);

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector2 XY { 
            get => new Vector2(X, Y); 
            set
            {
                X = value.X;
                Y = value.Y;
            }
        }

        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
        }

        public Vector3 Normalised()
        {
            float length = Length();
            if (length == 0)
            {
                return this;
            }

            return this / length;
        }

        public static Vector3 operator *(Vector3 vector, float scalar)
        {
            return new Vector3(scalar * vector.X, scalar * vector.Y, scalar * vector.Z);
        }

        public static Vector3 operator *(float scalar, Vector3 vector)
        {
            return new Vector3(scalar * vector.X, scalar * vector.Y, scalar * vector.Z);
        }

        public static Vector3 operator /(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X / scalar, vector.Y / scalar, vector.Z / scalar);
        }

        public static Vector3 operator +(Vector3 vectorA, Vector3 vectorB)
        {
            return new Vector3(vectorA.X + vectorB.X, vectorA.Y + vectorB.Y, vectorA.Z + vectorB.Z);
        }

        public static Vector3 operator -(Vector3 vectorA, Vector3 vectorB)
        {
            return new Vector3(vectorA.X - vectorB.X, vectorA.Y - vectorB.Y, vectorA.Z - vectorB.Z);
        }
    }

}
