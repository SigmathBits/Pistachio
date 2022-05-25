#include "pstpch.h"
#include "Platform.h"
#include "pstpch.h"

#include "Platform.h"

#include "Platform/OpenGL/OpenGLPlatform.h"


namespace Pistachio {

    Platform* Platform::m_Platform = new OpenGLPlatform;

    float Platform::Time() {
        return m_Platform->TimeImpl();
    }

}
