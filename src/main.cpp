#include <iostream>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <rg/MyTexture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/mesh.h>
#include <learnopengl/shader.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// sto veci ekran
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// izmena pocetne lokacije kamere -> kasnije ce nam trebati da bi stavili u centar muzeja
Camera ourCamera(glm::vec3(-1.09837f, 2.23947f, 6.99487f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // zamena kasicnih f-ja koje rade sa misom i tastaturom umesto nas
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // bez ove komande ide ono cudno stanje -> zakoemntarisi i promeni sensitivity misa na 0.1 ako ti treba lasksi rad
    // sa virtuelnom i clionom jelte
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // shaderi za kocku svetla kao i za kocke koje se rotiraju -> OVO SE MENJA PRI IZBACIVANJU KOCKI
    Shader cubeShader("resources/shaders/vertexCube.vs", "resources/shaders/fragmentCube.fs");
    Shader lightShader("resources/shaders/vertexLight.vs", "resources/shaders/fragmentLight.fs");
    Shader flashlightShader("resources/shaders/vertexFlashlight.vs", "resources/shaders/fragmentFlashlight.fs");
    Shader mainRoomShader("resources/shaders/vertexMainroom.vs", "resources/shaders/fragmentMainroom.fs");

    // pravimo teksture
    std::string path1 = "resources/textures/container2.png";
    MyTexture ourTexture1(path1);
    std::string path2 = "resources/textures/container_specular.png";
    MyTexture ourTexture2(path2);

    cubeShader.use();
    cubeShader.setFloat("material.difuse", 0);
    cubeShader.setFloat("material.specular", 1);


    // dodavanje flashlight-a "resources/objects/flashlight/Flashlight_Idle.obj.glb"
    Model flashlight(FileSystem::getPath("resources/objects/Flashlight/Flashlight_Idle.obj"));
    Model mainRoom(FileSystem::getPath("resources/objects/Lavirint/Lavirint4_1.obj"));

    // dodajemo zbog efekta slabljenja baterijske lampe
    glm::vec3 ambientLighting = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 diffuseLighting = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 specularLighting = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuseDiff = glm::vec3(0.01f, 0.01f, 0.01f);
    glm::vec3 specularDiff = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 ambientDiff = glm::vec3(0.1f, 0.1f, 0.1f);
    bool batteryNotDead = true;
    int batteryLife = 100;
    float shininess = 32.0f;
    // da ne bi pokupili npr 2.1, 2.2, 2.3 kao 2, nego cim smo jednom 2 pokupili, nemoj vise uzimas time = 2
    float lastValueTaken = 0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // ukoliko korisnih pokusa da izadje izvan sobe, vracamo ga na ivicu (PLAFON SOBE)
        if (ourCamera.Position.y > 5.87922)
            ourCamera.Position.y = 5.87922;
        // (POD SOBE)
        else if (ourCamera.Position.y < 0.4942135)
            ourCamera.Position.y = 0.4942135;
        // ukoliko korisnik pokusa da izadje van uvodne sobe, vracamo ga na ivicu(LEVI ZID UVODNE SOBE)
        else if (ourCamera.Position.x < -8.34407 && ourCamera.Position.z < 7.42418 && ourCamera.Position.z > -7.70539)
            ourCamera.Position.x = -8.34407;
        // (DESNI ZID UVODNE SOBE)
        else if (ourCamera.Position.x > 6.44959 && ourCamera.Position.z < 7.42418 && ourCamera.Position.z > -7.70539)
            ourCamera.Position.x = 6.44959;
        // (ZADNJI ZID UVODNE SOBE)
        else if (ourCamera.Position.z > +7.42418)
            ourCamera.Position.z = +7.42418;
        // (LEVI ZID LAVIRINTA)
        else if (ourCamera.Position.x < -13.584 && ourCamera.Position.z > -33.8069 && ourCamera.Position.z < -7.70539)
            ourCamera.Position.x = -13.584;
        // (DESNA IVICA LAVIRINTA)
        else if (ourCamera.Position.x > 11.5185 && ourCamera.Position.z > -33.8069 && ourCamera.Position.z < -7.70539)
            ourCamera.Position.x = 11.5185;
        // (PREDNJA STRANA LAVIRINTA)
        else if (ourCamera.Position.z < -33.4069)
            ourCamera.Position.z = -33.4069;
         //(RUPA DESNA I LEVA STRANA) - moraju da se stave malo manje granice od onih za blizi i dalji zid jer ce inace lepiti za levi i desni zid
        else if((ourCamera.Position.x < 5.48954 || ourCamera.Position.x > -7.69192) && ourCamera.Position.z > -27.0 && ourCamera.Position.z < -14.6){
            if(ourCamera.Position.x > 5.48954 || ourCamera.Position.x < -7.69192) {}
            else {
                if (abs(6.44959 - ourCamera.Position.x) < abs(-8.34407 - ourCamera.Position.x))
                    ourCamera.Position.x = 5.48954;
                else
                    ourCamera.Position.x = -7.69192;
            }
        }
        //RUPA BLIZI I DALJI ZID 
        else if((ourCamera.Position.z < -14.0583 || ourCamera.Position.z > -27.5423 ) && ourCamera.Position.x > -7.69192 && ourCamera.Position.x < 5.48954){
            if(ourCamera.Position.z > -14.0583 || ourCamera.Position.z < -27.5423) {}
            else {
                if (abs( -7.70539 - ourCamera.Position.z) < abs(-33.8069 - ourCamera.Position.z))
                    ourCamera.Position.z = -14.0583;
                else
                    ourCamera.Position.z = -27.5423;
            }

        }
        // (ZADNJI I PREDNJI DEO RUPE) -> PITAJ DIVNU KAKO RUPU DA OGRANICIMO(MOZEMO AKO JE U TOJ RUPI DA DODAMO RAZLIK UIZMEDJU PRETHODNOG Z I X)
        //else if (ourCamera.Position.x > -7.47682 && ourCamera.Position.x < 5.38767 && ourCamera.Position.z < -13.9424 && ourCamera.Position.z > -27.9438){
        //    float diffFar = std::fabs(ourCamera.Position.z - -27.9438);
        //    float diffNear = std::fabs(ourCamera.Position.z - -13.9424);
        //    if (diffFar < diffNear)
        //        ourCamera.Position.z =  -27.9438;
        //    else
        //        ourCamera.Position.z = -13.9424;
        //}
        //else if (ourCamera.Position.x > -7.47682 && ourCamera.Position.x < 5.38767 && ourCamera.Position.z < -13.9424 && ourCamera.Position.z > -27.9438){
        //    float diffLeft = std::fabs(ourCamera.Position.x - -7.47682);
        //    float diffRight = std::fabs(ourCamera.Position.x - 5.38767);
        //    if (diffLeft < diffRight)
        //        ourCamera.Position.x =  -7.47682;
        //    else
        //        ourCamera.Position.x = 5.38767;
        //}

        // racunanje vremena izmedju 2 frame-a
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        int time = (int)lastFrame;

        // SLABLJENJE BATERISJKE LAMPE
        if (time % 3 == 0 && batteryNotDead && time != lastValueTaken){
            if (batteryLife % 20 == 0){
                //ambientLighting -= ambientDiff;
                //specularDiff -= specularDiff;
                shininess  /= 2;
            }
            diffuseLighting -= diffuseDiff;
            batteryLife--;
            lastValueTaken = time;
            if (batteryLife == 0) {
                batteryNotDead = false;
                //shininess = 0;
            }
            //std::cout << batteryLife << "\n";
            //std::cout << diffuseLighting[0] << " " << diffuseLighting[1] << " " << diffuseLighting[2] << "\n";
        }
        //std::cout << currentFrame << "\n";


        processInput(window);

        // aktivacija textura
        ourTexture1.activateTexture(0);
        ourTexture2.activateTexture(1);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float zNear = 0.1f;
        float zFar = 100.0f;
        // racunanje projection matrice
        glm::mat4 projection = glm::perspective(glm::radians(ourCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear, zFar);
        // racunanje view matrice
        glm::mat4 view = ourCamera.GetViewMatrix();


        // Racunanje normalMatrix na CPU, ne GPU, ali ima problema -> Pitaj marka
        //glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        //cubeShader.setUniform3fMatrix("normalMatrix", normalMatrix);

        // main room crtanje

        // saljemo sve potrebne informacije za baterijsku lampu
        mainRoomShader.use();
        mainRoomShader.setVec3("light.position", ourCamera.Position);
        mainRoomShader.setVec3("light.direction", ourCamera.Front);
        mainRoomShader.setFloat("light.cutOff", glm::cos(glm::radians(20.5f)));
        mainRoomShader.setFloat("light.outerCutOff", glm::cos(glm::radians(25.5f)));
        mainRoomShader.setVec3("viewPos", ourCamera.Position);

        // light properties
        //ako oces svetlije modele, ovde gledaj!
        mainRoomShader.setVec3("light.ambient", ambientLighting);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        mainRoomShader.setVec3("light.diffuse", diffuseLighting);
        mainRoomShader.setVec3("light.specular", specularLighting);
        mainRoomShader.setFloat("light.constant", 1.0f);
        mainRoomShader.setFloat("light.linear", 0.09f);
        mainRoomShader.setFloat("light.quadratic", 0.032f);
        mainRoomShader.setFloat("shininess", shininess);

        // material properties

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
        mainRoomShader.setMat4("view", view);
        mainRoomShader.setMat4("projection", projection);
        mainRoomShader.setMat4("model", model);
        mainRoom.Draw(mainRoomShader);

        // JAKO BITNA LINIJA -> ODREDJUJEMO GDE CEMO TACNO SLIKE SA OVIME, JER DODJES KAMEROM GDE IH OS, I GLEDAJ OVO
        //std::cout << ourCamera.Position[0] << " " << ourCamera.Position[1] << " " << ourCamera.Position[2] << "\n";

        //FIRE HAZARD

        view = glm::mat4(1.0f);
        flashlightShader.setMat4("view", view);
        flashlightShader.setMat4("projection", projection);

        glm::vec3 lightPosition = glm::vec3(+3.8f, 0.0f, -3.0f);
        model = glm::mat4(1.0f);
        float flashlightDistance = zNear;
        // ourCamera.Position + flashlightDistance * ourCamera.Front
        glm::vec3 flashlightVector = ( ourCamera.Position + (flashlightDistance * ourCamera.Front));
        flashlightVector += glm::vec3(3.0f, 0.0f, 0.0f);
        // glm:`:vec3(-1.0f, 0.0f, -1.0f)
        model = glm::translate(model, lightPosition);
        model = glm::rotate(model, glm::radians(+180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::rotate(model, glm::radians((float)(2 * M_PI / ourCamera.MouseSensitivity)), ourCamera.Front);
        // razlika ugla novogfront i starog front

        //model = glm::rotate(model, diffrencePitch, glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, diffrenceYaw, glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        flashlightShader.setMat4("model", model);


        flashlight.Draw(flashlightShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(RIGHT, deltaTime);

}

// f-ja pomeranja misa -> za koliko se pomerio od pocetne lokacije jelte
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // saljemo razliku da bi nam se kamera pomerila
    ourCamera.ProcessMouseMovement(xoffset, yoffset);
}

// f-ja pri skrolovanju
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ourCamera.ProcessMouseScroll(yoffset);
}

// izmena velicine window-a
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // sama promena
    glViewport(0, 0, width, height);
}
