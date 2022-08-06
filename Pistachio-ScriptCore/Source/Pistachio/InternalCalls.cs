using System.Runtime.CompilerServices;


namespace Pistachio
{

    internal static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseButtonDown(MouseButtonCode button);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_Translation(ulong uuid, out Vector3 outTranslation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTranslation(ulong uuid, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_Rotation(ulong uuid, out Vector3 outRotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetRotation(ulong uuid, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_Scale(ulong uuid, out Vector3 outScale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetScale(ulong uuid, ref Vector3 scale);
    }

}
