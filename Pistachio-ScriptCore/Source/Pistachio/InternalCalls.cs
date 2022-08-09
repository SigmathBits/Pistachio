using System;
using System.Runtime.CompilerServices;


namespace Pistachio
{

    internal static class InternalCalls
    {
        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseButtonDown(MouseButtonCode button);
        #endregion


        #region Entity Component
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityUUID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_AddComponent(ulong entityUUID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_RemoveComponent(ulong entityUUID, Type componentType);
        #endregion


        #region Circle Renderer Component
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_Colour(ulong entityUUID, out Vector4 outColour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetColour(ulong entityUUID, ref Vector4 colour);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleRendererComponent_Thickness(ulong entityUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetThickness(ulong entityUUID, float thickness);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float CircleRendererComponent_Blur(ulong entityUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CircleRendererComponent_SetBlur(ulong entityUUID, float blur);
        #endregion


        #region Transform Component
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_Translation(ulong entityUUID, out Vector3 outTranslation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityUUID, ref Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_Rotation(ulong entityUUID, out Vector3 outRotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong entityUUID, ref Vector3 rotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_Scale(ulong entityUUID, out Vector3 outScale);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong entityUUID, ref Vector3 scale);
        #endregion


        #region Rigid Body 2D Component
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static RigidBody2DType RigidBody2DComponent_Type(ulong entityUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetType(ulong entityUUID, RigidBody2DType type);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool RigidBody2DComponent_FixedRotation(ulong entityUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetFixedRotation(ulong entityUUID, bool fixedRotation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_Position(ulong entityUUID, out Vector2 outPosition);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetPosition(ulong entityUUID, ref Vector2 position);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float RigidBody2DComponent_Rotation(ulong entityUUID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_SetRotation(ulong entityUUID, float rotation);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulse(ulong entityUUID, ref Vector2 impulse, ref Vector2 worldPosition, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void RigidBody2DComponent_ApplyLinearImpulseToCentre(ulong entityUUID, ref Vector2 impulse, bool wake);
        #endregion
    }

}
