namespace Pistachio
{

    public static class Input
    {
        public static bool IsKeyDown(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyDown(keyCode);
        }

        public static bool IsMouseButtonDown(MouseButtonCode button)
        {
            return InternalCalls.Input_IsMouseButtonDown(button);
        }
    }

}
