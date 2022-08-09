using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Pistachio
{

    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_Translation(Entity.UUID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.UUID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_Rotation(Entity.UUID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(Entity.UUID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_Scale(Entity.UUID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(Entity.UUID, ref value);
            }
        }
    }

    // TODO: Add properties and methods
    public class SpriteRendererComponent : Component
    { 
    
    }

    public class CircleRendererComponent : Component
    {
        public Vector4 Colour
        {
            get
            {
                InternalCalls.CircleRendererComponent_Colour(Entity.UUID, out Vector4 colour);
                return colour;
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetColour(Entity.UUID, ref value);
            }
        }

        public float Thickness
        {
            get
            {
                return InternalCalls.CircleRendererComponent_Thickness(Entity.UUID);
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetThickness(Entity.UUID, value);
            }
        }

        public float Blur
        {
            get
            {
                return InternalCalls.CircleRendererComponent_Blur(Entity.UUID);
            }
            set
            {
                InternalCalls.CircleRendererComponent_SetBlur(Entity.UUID, value);
            }
        }
    }

    // TODO: Add properties and methods
    public class CameraComponent : Component
    {

    }

    // TODO: Add properties and methods
    public class ScriptComponent : Component
    {

    }

    public enum RigidBody2DType
    {
        Static = 0, Dynamic, Kinematic
    }

    public class RigidBody2DComponent : Component
    {
        public RigidBody2DType Type
        {
            get
            {
                return InternalCalls.RigidBody2DComponent_Type(Entity.UUID);
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetType(Entity.UUID, value);
            }
        }

        public bool FixedRotation
        {
            get
            {
                return InternalCalls.RigidBody2DComponent_FixedRotation(Entity.UUID);
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetFixedRotation(Entity.UUID, value);
            }
        }

        public Vector2 Position
        {
            get
            {
                InternalCalls.RigidBody2DComponent_Position(Entity.UUID, out Vector2 position);
                return position;
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetPosition(Entity.UUID, ref value);
            }
        }

        public float Rotation
        {
            get
            {
                return InternalCalls.RigidBody2DComponent_Rotation(Entity.UUID);
            }
            set
            {
                InternalCalls.RigidBody2DComponent_SetRotation(Entity.UUID, value);
            }
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake = true)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(Entity.UUID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake = true)
        {
            InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCentre(Entity.UUID, ref impulse, wake);
        }
    }

    // TODO: Add properties and methods
    public class BoxCollider2DComponent : Component
    {

    }

    // TODO: Add properties and methods
    public class CircleCollider2DComponent : Component
    {

    }

}
