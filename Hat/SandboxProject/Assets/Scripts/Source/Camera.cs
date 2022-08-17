using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Pistachio;


namespace Sandbox
{

    public class Camera : Entity
    {
        const float SPEED = 10.0f;  // Units/second

        void OnUpdate(float timestep)
        {
            Vector3 velocity = SPEED * new Vector3(
                (Input.IsKeyDown(KeyCode.RIGHT) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.LEFT) ? 1.0f : 0.0f),
                (Input.IsKeyDown(KeyCode.UP) ? 1.0f : 0.0f) - (Input.IsKeyDown(KeyCode.DOWN) ? 1.0f : 0.0f),
                0.0f
            ).Normalised();

            Translation += velocity * timestep;
        }
    }

}
