using System;


namespace Pistachio
{

    public class Entity
    {
        public readonly ulong UUID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_Translation(UUID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(UUID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.TransformComponent_Rotation(UUID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.TransformComponent_SetRotation(UUID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.TransformComponent_Scale(UUID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.TransformComponent_SetScale(UUID, ref value);
            }
        }

        protected Entity()
        {
            UUID = 0;
        }

        internal Entity(ulong uuid)
        {
            UUID = uuid;
        }


        public T Component<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
            {
                return null;
            }

            return new T() { Entity = this };
        }

        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                return new T() { Entity = this };
            }

            Type componentType = typeof(T);
            InternalCalls.Entity_AddComponent(UUID, componentType);

            return new T() { Entity = this };
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
            {
                return;
            }

            Type componentType = typeof(T);
            InternalCalls.Entity_RemoveComponent(UUID, componentType);
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(UUID, componentType);
        }

    }

}
