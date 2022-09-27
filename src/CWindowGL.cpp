// by jrzanol
//

#include "stdafx.h"
#include "CWindowGL.h"

SliderInfo CWindowGL::g_SliderInfo(0.f, 0.f, 0, 1.f, 1.f);

const char* CWindowGL::g_VertexShader = R"glsl(
    #version 130

    in vec3 a_position;

    uniform mat4 u_view;
    uniform mat4 u_proj;

    void main()
    {
        gl_Position = u_proj * u_view * vec4(a_position, 1.0);
    }
)glsl";

const char* CWindowGL::g_FragmentShader =   "#version 130\n"
                                            "void main() {"
                                                "gl_FragColor = vec4(1, 0, 0, 1);"
                                            "}";

CWindowGL::CWindowGL()
{
    m_Window = NULL;
    m_ProgramId = 0;
}

CWindowGL::~CWindowGL()
{
}

bool CWindowGL::Initialize()
{
    std::cout << "Iniciando glfw...\n";

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    m_Window = glfwCreateWindow(g_MaxX, g_MaxY, "cg", NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // Enable vsync

    std::cout << "Iniciando glew...\n";

    /* Initialize glew and OpenGL functions */
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;

        glfwTerminate();
        return false;
    }

    std::cout << "Iniciando imgui...\n";

    /* Initialize ImGui */
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();

    std::cout << "Compilando os Shaders...\n";

    /* Compile and Link Shaders */
    GLuint vShaderId = CompileShader(g_VertexShader, GL_VERTEX_SHADER);
    GLuint fShaderId = CompileShader(g_FragmentShader, GL_FRAGMENT_SHADER);
    m_ProgramId = LinkProgram(vShaderId, fShaderId);

    std::cout << "Carregando os modelos...\n";

    // Load Models.
    m_DrawObject.push_back(new CDrawableObject(m_ProgramId, "cube.obj"));

    std::cout << "Iniciando...\n";
	return true;
}

void CWindowGL::Cleanup()
{
    /* Cleanup ImGui */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* Cleanup GLFW */
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool CWindowGL::Render()
{
    // Clear OpenGl frame.
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use Shaders.
    glUseProgram(m_ProgramId);

    // Transforms.
    GLint projPos = glGetUniformLocation(m_ProgramId, "u_proj");
    GLint modelPos = glGetUniformLocation(m_ProgramId, "u_model");
    GLint viewPos = glGetUniformLocation(m_ProgramId, "u_view");

    glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)g_MaxY / (float)g_MaxX, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(g_SliderInfo.m_X, g_SliderInfo.m_Y, 0.f));
    model = glm::scale(model, glm::vec3(g_SliderInfo.m_ScaleX, g_SliderInfo.m_ScaleY, 0.f));
    model = glm::rotate(model, glm::radians((float)g_SliderInfo.m_Angle), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(projPos, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewPos, 1, GL_FALSE, glm::value_ptr(view));

    for (const auto& it : m_DrawObject)
        it->Draw();

    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui Sliders.
    ImGui::Begin("Infos:");
    ImGui::SliderFloat("X", &g_SliderInfo.m_X, -2.f, 2.f);
    ImGui::SliderFloat("Y", &g_SliderInfo.m_Y, -2.f, 2.f);
    ImGui::SliderInt("Angle", &g_SliderInfo.m_Angle, 0, 360);
    ImGui::SliderFloat("Scale X", &g_SliderInfo.m_ScaleX, -5.f, 5.f);
    ImGui::SliderFloat("Scale Y", &g_SliderInfo.m_ScaleY, -5.f, 5.f);
    ImGui::End();

    // Rendering the ImGui.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(m_Window);

    // Poll for and process events.
    glfwPollEvents();

    return glfwWindowShouldClose(m_Window);
}

GLuint CWindowGL::CompileShader(const char* shaderCode, GLenum type)
{
    GLuint shaderId = glCreateShader(type);

    if (shaderId == 0) { // Error: Cannot create shader object
        std::cout << "Error creating shaders";
        return 0;
    }

    // Attach source code to this object
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId); // compile the shader object

    GLint compileStatus;

    // check for compilation status
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (!compileStatus) { // If compilation was not successful
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* cMessage = new char[length];

        // Get additional information
        glGetShaderInfoLog(shaderId, length, &length, cMessage);
        std::cout << "Cannot Compile Shader: " << cMessage;
        delete[] cMessage;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

GLuint CWindowGL::LinkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
    GLuint programId = glCreateProgram(); // create a program

    if (programId == 0) {
        std::cout << "Error Creating Shader Program";
        return 0;
    }

    // Attach both the shaders to it
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Create executable of this program
    glLinkProgram(programId);

    GLint linkStatus;

    // Get the link status for this program
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

    if (!linkStatus) { // If the linking failed
        std::cout << "Error Linking program";
        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);
        glDeleteProgram(programId);

        return 0;
    }

    return programId;
}

