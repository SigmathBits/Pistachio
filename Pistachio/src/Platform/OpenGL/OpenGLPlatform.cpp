#include "pstpch.h"
#include "OpenGLPlatform.h"
#include "pstpch.h"

#include "OpenGLPlatform.h"

#include <GLFW/glfw3.h>


namespace Pistachio {
    
    float OpenGLPlatform::TimeImpl() const {
        return (float)glfwGetTime();
    }

}
