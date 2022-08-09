using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Pistachio;


namespace Sandbox
{

    public class Player : Entity
    {
        const float SPEED = 10.0f;  // Units/second
        const float IMPULSE = 20.0f;  // Force/second
        const float ROTATIONAL_SPEED = (float)Math.PI / 2;  // Radians/second  

        private TransformComponent m_Transform;
        private RigidBody2DComponent m_RigidBody;
        private BoxCollider2DComponent m_BoxCollider;

        private bool m_RLastPressed;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate: {UUID}");

            m_Transform = Component<TransformComponent>();
            m_RigidBody = Component<RigidBody2DComponent>();
            m_BoxCollider = Component<BoxCollider2DComponent>();
        }

        void OnUpdate(float timestep)
        {
            if (Input.IsKeyDown(KeyCode.SPACE) && !HasComponent<CircleRendererComponent>())
            {
                RemoveComponent<SpriteRendererComponent>();
                CircleRendererComponent component = AddComponent<CircleRendererComponent>();
                component.Colour = new Vector4(1.0f, 0.0f, 1.0f, 1.0f);
            }

            if (Input.IsKeyDown(KeyCode.R) && !m_RLastPressed)
            {
                m_RigidBody.Type = m_RigidBody.Type == RigidBody2DType.Kinematic ? RigidBody2DType.Dynamic : RigidBody2DType.Kinematic;
                //m_RigidBody.FixedRotation = !m_RigidBody.FixedRotation;
            }
            m_RLastPressed = Input.IsKeyDown(KeyCode.R);


            Vector2 input = new Vector2(
                (Input.IsKeyDown(KeyCode.D) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.A) ? 1.0f : 0.0f),
                (Input.IsKeyDown(KeyCode.W) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.S) ? 1.0f : 0.0f)
            ).Normalised();

            if (m_RigidBody.Type == RigidBody2DType.Kinematic)
            {
                Vector2 velocity = SPEED * input;
                m_RigidBody.Position += velocity * timestep;
                m_RigidBody.Rotation += ((Input.IsKeyDown(KeyCode.Q) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.E) ? 1.0f : 0.0f)) * timestep;
            }
            else
            {
                Vector2 impulse = IMPULSE * input;
                m_RigidBody.ApplyLinearImpulse(impulse * timestep);
            }
        }
    }

}
