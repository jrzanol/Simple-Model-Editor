// by jrzanol
//

#include "stdafx.h"
#include "CWindow.h"

glm::mat4 CWindow::m_VP;
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

const glm::mat4& CWindow::GetVP() { return m_VP; }
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
    CreateModel("Model/main.obj");

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

    // Set Transforms.
    glUniform1f(glGetUniformLocation(m_ProgramId, "u_textcoord"), CUtil::m_SliderInfo.m_TextCoord);

    glm::mat4 projection = glm::perspective(glm::radians(m_Zoom), (float)g_WindowMaxY / (float)g_WindowMaxX, 0.1f, 100.0f);
    glm::mat4 view = GetViewMatrix();

    m_VP = (projection * view);

    // Draw objects.
    for (const auto& it : m_DrawModel)
        it->Draw(m_ProgramId, m_VP);

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
        ImGui::RadioButton("Criar Vertices", &CUtil::m_EditorType, 1);
        ImGui::RadioButton("Remover Vertices", &CUtil::m_EditorType, 2);
        ImGui::Separator();
        ImGui::RadioButton("Textura Padrao", &CUtil::m_TextureType, 0);
        ImGui::RadioButton("Textura #02", &CUtil::m_TextureType, 1);
        ImGui::RadioButton("Textura #03", &CUtil::m_TextureType, 2);
        ImGui::Separator();
        ImGui::SliderFloat("Mover a Textura", &CUtil::m_SliderInfo.m_TextCoord, 0.f, 2.f);
        ImGui::Separator();
        if (ImGui::Button("Save"))
        {
            CWindow::SaveModel();

            ImGui::SameLine();
            ImGui::Text("Salvo!");
        }
        ImGui::Separator();
        if (ImGui::Button("Criar Modelo #1"))
            CreateModel("Model/main.obj");
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

void CWindow::CreateModel(const char* fileModel)
{
    static int s_ModelCounter = 0;

    CModel* m = CModel::LoadModel(fileModel);
    m->m_Position = glm::vec3(0.f, 0.f, -5.f * s_ModelCounter++);

    m_DrawModel.push_back(m);
}

void CWindow::SaveModel()
{
    CModel* currentModel = CWindow::GetModels().front();

    FILE* out = fopen("Model/main_out.obj", "wt");
    if (out)
    {
        fprintf(out, "# Simple 3D Editor\n");
        fprintf(out, "mtllib Crate1.mtl\n");

        int objId = 1;

        for (CMesh& mesh : currentModel->m_Meshes)
        {
            fprintf(out, "o Object.%03d\n", objId++);

            for (Vertex& v : mesh.m_Vertex)
                fprintf(out, "v %.6f %.6f %.6f\n", v.Position.x, v.Position.y, v.Position.z);

            for (Vertex& v : mesh.m_Vertex)
                fprintf(out, "vt %.6f %.6f\n", v.TexCoords.x, (1.f - v.TexCoords.y));
        }

        fprintf(out, "usemtl Material.001\n");
        fprintf(out, "s off\n");

        for (CMesh& mesh : currentModel->m_Meshes)
            for (size_t i = 0; i < mesh.m_Indices.size(); i += 3)
                fprintf(out, "f %u/%u %u/%u %u/%u\n",
                    mesh.m_Indices[i] + 1, mesh.m_Indices[i] + 1,
                    mesh.m_Indices[i + 1] + 1, mesh.m_Indices[i + 1] + 1,
                    mesh.m_Indices[i + 2] + 1, mesh.m_Indices[i + 2] + 1);

        fclose(out);
    }
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

