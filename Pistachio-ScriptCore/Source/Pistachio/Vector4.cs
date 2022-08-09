using System;


namespace Pistachio
{

    public struct Vector4
    {
        public float X, Y, Z, W;

        public static Vector4 Zero => new Vector4(0.0f, 0.0f, 0.0f, 0.0f);

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public Vector4(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
            W = scalar;
        }

        public Vector2 XY
        {
            get => new Vector2(X, Y);
            set
            {
                X = value.X;
                Y = value.Y;
            }
        }

        public Vector3 XYZ
        {
            get => new Vector3(X, Y, Z);
            set
            {
                X = value.X;
                Y = value.Y;
                Z = value.Z;
            }
        }

        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z + W * W);
        }

        public Vector4 Normalised()
        {
            float length = Length();
            if (length == 0)
            {
                return this;
            }

            return this / length;
        }

        public static Vector4 operator *(Vector4 vector, float scalar)
        {
            return new Vector4(scalar * vector.X, scalar * vector.Y, scalar * vector.Z, scalar * vector.W);
        }

        public static Vector4 operator *(float scalar, Vector4 vector)
        {
            return new Vector4(scalar * vector.X, scalar * vector.Y, scalar * vector.Z, scalar * vector.W);
        }

        public static Vector4 operator /(Vector4 vector, float scalar)
        {
            return new Vector4(vector.X / scalar, vector.Y / scalar, vector.Z / scalar, vector.W / scalar);
        }

        public static Vector4 operator +(Vector4 vectorA, Vector4 vectorB)
        {
            return new Vector4(vectorA.X + vectorB.X, vectorA.Y + vectorB.Y, vectorA.Z + vectorB.Z, vectorA.W + vectorB.W);
        }

        public static Vector4 operator -(Vector4 vectorA, Vector4 vectorB)
        {
            return new Vector4(vectorA.X - vectorB.X, vectorA.Y - vectorB.Y, vectorA.Z - vectorB.Z, vectorA.W - vectorB.W);
        }
    }

}
