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
                InternalCalls.Entity_Translation(UUID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.Entity_SetTranslation(UUID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {
                InternalCalls.Entity_Rotation(UUID, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.Entity_SetRotation(UUID, ref value);
            }
        }

        public Vector3 Scale
        {
            get
            {
                InternalCalls.Entity_Scale(UUID, out Vector3 scale);
                return scale;
            }
            set
            {
                InternalCalls.Entity_SetScale(UUID, ref value);
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
    }

}
