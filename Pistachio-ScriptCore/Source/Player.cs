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
        const float ROTATIONAL_SPEED = (float)Math.PI / 2;  // Radians/second  

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate: {UUID}");
        }

        void OnUpdate(float timestep)
        {
            if (!Input.IsKeyDown(KeyCode.LEFT_CONTROL))
            {
                Vector3 velocity = SPEED * new Vector3(
                    (Input.IsKeyDown(KeyCode.D) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.A) ? 1.0f : 0.0f),
                    (Input.IsKeyDown(KeyCode.W) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.S) ? 1.0f : 0.0f),
                    0.0f
                ).Normalised();

                Translation += velocity * timestep;
            }


            Vector3 rotationalVelocity = ROTATIONAL_SPEED * new Vector3(
                0.0f,
                0.0f,
                (Input.IsMouseButtonDown(MouseButtonCode.LEFT) ? 1.0f : 0.0f) - (Input.IsMouseButtonDown(MouseButtonCode.RIGHT) ? 1.0f : 0.0f)
            );

            Rotation += rotationalVelocity * timestep;


            if (Input.IsKeyDown(KeyCode.LEFT_CONTROL))
            {
                Vector3 scalarVelocity = new Vector3(
                    (Input.IsKeyDown(KeyCode.D) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.A) ? 1.0f : 0.0f),
                    (Input.IsKeyDown(KeyCode.W) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.S) ? 1.0f : 0.0f),
                    0.0f
                );

                Scale += scalarVelocity * timestep;
            }
        }
    }

}
