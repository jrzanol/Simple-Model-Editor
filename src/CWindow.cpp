// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

glm::mat4 CWindow::m_MVP;
std::list<CModel*> CWindow::m_DrawModel;

CWindow::CWindow() : CCamera()
{
    g_Window = NULL;
    m_ProgramId = 0;
    m_PickingProgramId = 0;
}

CWindow::~CWindow()
{
}

const glm::mat4& CWindow::GetMVP() { return m_MVP; }
const std::list<CModel*>& CWindow::GetModels() { return m_DrawModel; }

bool CWindow::Initialize()
{
    std::cout << "Iniciando glfw...\n";

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    g_Window = glfwCreateWindow(g_WindowMaxX, g_WindowMaxY, "cg", NULL, NULL);
    if (!g_Window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(g_Window);
    glfwSwapInterval(1); // Enable vsync

    ::CEvent::Initialize();

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
    ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
    ImGui_ImplOpenGL3_Init();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    /* Compile and Link Shaders */
    std::cout << "Compilando o Vertex Shader...\n";
    GLuint vShaderId = CompileShader(CUtil::m_VertexShader, GL_VERTEX_SHADER);
    GLuint vPickShaderId = CompileShader(CUtil::m_PickingVertexShader, GL_VERTEX_SHADER);

    std::cout << "Compilando o Fragment Shader...\n";
    GLuint fShaderId = CompileShader(CUtil::m_FragmentShader, GL_FRAGMENT_SHADER);
    GLuint fPickShaderId = CompileShader(CUtil::m_PickingFragmentShader, GL_FRAGMENT_SHADER);

    m_ProgramId = LinkProgram(vShaderId, fShaderId);
    m_PickingProgramId = LinkProgram(vPickShaderId, fPickShaderId);

    // Use Shaders.
    glUseProgram(m_ProgramId);

    std::cout << "Carregando os modelos...\n";

    // Load Models.
    m_DrawModel.push_back(CModel::LoadModel("Model/main.obj"));

    // Configure the Lines.
    glLineWidth(2.f);
    glEnable(GL_LINE_SMOOTH);

    std::cout << "Iniciando...\n";
	return true;
}

void CWindow::Cleanup()
{
    /* Cleanup ImGui */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* Cleanup GLFW */
    glfwDestroyWindow(g_Window);
    glfwTerminate();
}

bool CWindow::Render()
{
    // Clear OpenGl frame.
    glClearColor(0.25f, 0.25f, 0.25f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(m_Zoom), (float)g_WindowMaxY / (float)g_WindowMaxX, 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();
    glm::mat4 model(1.f);

    model = glm::translate(model, glm::vec3(CUtil::m_SliderInfo.m_X, CUtil::m_SliderInfo.m_Y, 0.f));
    model = glm::scale(model, glm::vec3(CUtil::m_SliderInfo.m_ScaleX, CUtil::m_SliderInfo.m_ScaleY, 1.f));
    model = glm::rotate(model, glm::radians((float)CUtil::m_SliderInfo.m_Angle), glm::vec3(0.0f, 0.0f, 1.0f));

    m_MVP = (projection * view * model);
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramId, "u_mvp"), 1, GL_FALSE, glm::value_ptr(m_MVP));

    // Draw objects.
    for (const auto& it : m_DrawModel)
        it->Draw(m_ProgramId);

    // Start the Dear ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create ImGui Sliders.
    ImGui::Begin("Infos:");
        ImGui::SliderFloat("X", &CUtil::m_SliderInfo.m_X, -2.f, 2.f);
        ImGui::SliderFloat("Y", &CUtil::m_SliderInfo.m_Y, -2.f, 2.f);
        ImGui::SliderInt("Angle", &CUtil::m_SliderInfo.m_Angle, 0, 360);
        ImGui::SliderFloat("Scale X", &CUtil::m_SliderInfo.m_ScaleX, -5.f, 5.f);
        ImGui::SliderFloat("Scale Y", &CUtil::m_SliderInfo.m_ScaleY, -5.f, 5.f);
        ImGui::Separator();
        ImGui::RadioButton("Arrastar Vertices/Triangulos", &CUtil::m_EditorType, 0);
        ImGui::RadioButton("Criar novo Vertices", &CUtil::m_EditorType, 1);
        ImGui::Separator();
        ImGui::RadioButton("Textura Padrao", &CUtil::m_TextureType, 0);
        ImGui::RadioButton("Textura #02", &CUtil::m_TextureType, 1);
        ImGui::RadioButton("Textura #03", &CUtil::m_TextureType, 2);
    ImGui::End();

    // Rendering the ImGui.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(g_Window);

    // Poll for and process events.
    glfwPollEvents();

    // Process Input events.
    for (const auto& it : *g_EventList)
        it->ProcessInput(g_Window);

    return glfwWindowShouldClose(g_Window);
}

GLuint CWindow::CompileShader(const char* shaderCode, GLenum type)
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

GLuint CWindow::LinkProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
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

