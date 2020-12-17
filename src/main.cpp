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
void setBorder();

// kod koji se cesto ponavlja, pa smo funkcije pisali
void placePainting(Shader paintingShader,glm::vec3 translation, glm::vec3 scale,glm::mat4 view, glm::mat4 projection, Model painting, float angleX = 360.0f, float angleY = 360.0f, float angleZ = 360.0f);
void placeStatue(Shader statueShader,glm::vec3 translation, glm::vec3 scale,glm::mat4 view, glm::mat4 projection, Model statue, glm::vec3 rotation = glm::vec3(1.0f, 0.0f, 0.0f), float angle = 360.0f);

// sto veci ekran
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 700;

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
    glDepthFunc(GL_LESS);

    // shaderi za kocku svetla kao i za kocke koje se rotiraju -> OVO SE MENJA PRI IZBACIVANJU KOCKI
    Shader cubeShader("resources/shaders/vertexCube.vs", "resources/shaders/fragmentCube.fs");
    Shader lightShader("resources/shaders/vertexLight.vs", "resources/shaders/fragmentLight.fs");
    Shader flashlightShader("resources/shaders/vertexFlashlight.vs", "resources/shaders/fragmentFlashlight.fs");
    Shader mainRoomShader("resources/shaders/vertexMainroom.vs", "resources/shaders/fragmentMainroom.fs");
    Shader monkShader("resources/shaders/vertexMonk.vs", "resources/shaders/fragmentMonk.fs");
    Shader paintingShader("resources/shaders/vertexPainting.vs", "resources/shaders/fragmentPainting.fs");
    Shader statueShader("resources/shaders/vertexStatue.vs", "resources/shaders/fragmentStatue.fs");
    Shader depthTestShader("resources/shaders/vertexDepthTesting.vs", "resources/shaders/fragmentDepthTesting.fs");

    // pravimo teksture
    std::string path1 = "resources/textures/container2.png";
    MyTexture ourTexture1(path1);
    std::string path2 = "resources/textures/container_specular.png";
    MyTexture ourTexture2(path2);

    cubeShader.use();
    cubeShader.setFloat("material.difuse", 0);
    cubeShader.setFloat("material.specular", 1);


    // ucitavanje svih modela koji ce nam trebati
    Model flashlight(FileSystem::getPath("resources/objects/Flashlight/flashlight.obj"));
    Model mainRoom(FileSystem::getPath("resources/objects/Lavirint/Lavirint4_2.obj"));
    Model monkSculture(FileSystem::getPath("resources/objects/monkStatue/monk.obj"));
    Model paintingVanGogh(FileSystem::getPath("resources/objects/vanGogh/vanGogh.obj"));
    Model paintingWave(FileSystem::getPath("resources/objects/wave/wave.obj"));
    Model paintingTime(FileSystem::getPath("resources/objects/time/time.obj"));
    Model paintingWinter(FileSystem::getPath("resources/objects/winter/winter.obj"));
    Model statuaAngel(FileSystem::getPath("resources/objects/statuaAndjeo/statuaAndjeo.obj"));
    Model statuaDisk(FileSystem::getPath("resources/objects/statuaBacacDiska/statuaBacac.obj"));
    Model statuaWoman(FileSystem::getPath("resources/objects/statuaZena/statuaZene.obj"));

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
       //postavljenje granica lavirinta
        setBorder();

        // racunanje vremena izmedju 2 frame-a
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        int time = (int)lastFrame;

        // SLABLJENJE BATERISJKE LAMPE
        if (time % 300 == 0 && batteryNotDead && time != lastValueTaken){
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

        // JAKO BITNA LINIJA -> ODREDJUJEMO GDE CEMO TACNO SLIKE SA OVIME, JER DODJES KAMEROM GDE IH OS, I GLEDAJ OVO
        std::cout << ourCamera.Position[0] << "f, " << ourCamera.Position[1] << "f, " << ourCamera.Position[2] << "f\n";

        processInput(window);

        // aktivacija textura
        ourTexture1.activateTexture(0);
        ourTexture2.activateTexture(1);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
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

        // GLAVNA SOBA
        // light properties
        mainRoomShader.use();
        mainRoomShader.setVec3("light.position", ourCamera.Position);
        mainRoomShader.setVec3("light.direction", ourCamera.Front);
        mainRoomShader.setFloat("light.cutOff", glm::cos(glm::radians(20.5f)));
        mainRoomShader.setFloat("light.outerCutOff", glm::cos(glm::radians(25.5f)));
        mainRoomShader.setVec3("viewPos", ourCamera.Position);
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

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.3f, 1.3f, 1.3f));
        mainRoomShader.setMat4("view", view);
        mainRoomShader.setMat4("projection", projection);
        mainRoomShader.setMat4("model", model);
        mainRoom.Draw(mainRoomShader);

        // SVE SLIKE
        // light properties
        paintingShader.use();
        paintingShader.setVec3("light.position", ourCamera.Position);
        paintingShader.setVec3("light.direction", ourCamera.Front);
        paintingShader.setFloat("light.cutOff", glm::cos(glm::radians(20.5f)));
        paintingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(25.5f)));
        paintingShader.setVec3("viewPos", ourCamera.Position);
        //ako oces svetlije modele, ovde gledaj!
        paintingShader.setVec3("light.ambient", ambientLighting);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        paintingShader.setVec3("light.diffuse", diffuseLighting);
        paintingShader.setVec3("light.specular", specularLighting);
        paintingShader.setFloat("light.constant", 1.0f);
        paintingShader.setFloat("light.linear", 0.09f);
        paintingShader.setFloat("light.quadratic", 0.032f);
        paintingShader.setFloat("shininess", shininess);


        // crtanje svih slika

        glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        // VANGOGH SLIKA
        float angleX = 180.0f;
        float angleY = 90.0f; // 270
        float angleZ = 360.0f;
        placePainting(paintingShader, glm::vec3(-1.09837f, 5.0f, -14.3f), glm::vec3(1.0f, 7.0f, 5.0f), view, projection, paintingVanGogh, angleX, angleY, angleZ);

        // WAVE SLIKA
        angleX = 180.0f;
        angleY = 270.0f; // 270
        angleZ = 360.0f;
        placePainting(paintingShader, glm::vec3(-1.09837f, 5.0f, -27.35f), glm::vec3(1.0f, 7.0f, 5.0f), view, projection, paintingWave, angleX, angleY, angleZ);

        // TIME SLIKA
        angleX = 180.0f;
        angleY = 360.0f;
        angleZ = 360.0f;
        placePainting(paintingShader, glm::vec3(5.55f, 5.0f, -20.4944f), glm::vec3(1.0f, 7.0f, 5.0f), view, projection, paintingTime, angleX, angleY, angleZ);

        // WINTER SLIKA
        angleX = 180.0f;
        angleY = 180.0f;
        angleZ = 360.0f;
        placePainting(paintingShader, glm::vec3(-7.55f, 5.0f, -20.4944f), glm::vec3(1.0f, 7.0f, 5.0f), view, projection, paintingWinter, angleX, angleY, angleZ);


        // SKULPTURA KOJA PRATI
        // light properties
        monkShader.use();
        mainRoomShader.setVec3("light.position", ourCamera.Position);
        monkShader.setVec3("light.direction", ourCamera.Front);
        monkShader.setFloat("light.cutOff", glm::cos(glm::radians(20.5f)));
        monkShader.setFloat("light.outerCutOff", glm::cos(glm::radians(25.5f)));
        monkShader.setVec3("viewPos", ourCamera.Position);
        //ako oces svetlije modele, ovde gledaj!
        monkShader.setVec3("light.ambient", ambientLighting);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        monkShader.setVec3("light.diffuse", diffuseLighting);
        monkShader.setVec3("light.specular", specularLighting);
        monkShader.setFloat("light.constant", 1.0f);
        monkShader.setFloat("light.linear", 0.09f);
        monkShader.setFloat("light.quadratic", 0.032f);
        monkShader.setFloat("shininess", shininess);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.2185f, 0.23947f, -8.21736f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.3f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        monkShader.setMat4("view", view);
        monkShader.setMat4("projection", projection);
        monkShader.setMat4("model", model);
        monkSculture.Draw(monkShader);

        //OSTALE STATUE
        //light properties
        statueShader.use();
        mainRoomShader.setVec3("light.position", ourCamera.Position);
        statueShader.setVec3("light.direction", ourCamera.Front);
        statueShader.setFloat("light.cutOff", glm::cos(glm::radians(20.5f)));
        statueShader.setFloat("light.outerCutOff", glm::cos(glm::radians(25.5f)));
        statueShader.setVec3("viewPos", ourCamera.Position);
        //ako oces svetlije modele, ovde gledaj!
        statueShader.setVec3("light.ambient", ambientLighting);
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        statueShader.setVec3("light.diffuse", diffuseLighting);
        statueShader.setVec3("light.specular", specularLighting);
        statueShader.setFloat("light.constant", 1.0f);
        statueShader.setFloat("light.linear", 0.09f);
        statueShader.setFloat("light.quadratic", 0.032f);
        statueShader.setFloat("shininess", shininess);

        //crtanje statua
        float angle;
        rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        angle = 90.0f;
        placeStatue(statueShader, glm::vec3(-13.0f, 0.0f, -9.0f), glm::vec3(0.15f, 0.15f, 0.15f), view, projection, statuaAngel, rotation, angle);

        rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        angle = 90.0f;
        placeStatue(statueShader, glm::vec3(9.5f, 0.0f, -31.0f), glm::vec3(0.15f, 0.15f, 0.15f), view, projection, statuaDisk, rotation, angle);

        rotation = glm::vec3(0.0f, 1.0f, 0.0f);
        angle = 0.0f;
        placeStatue(statueShader, glm::vec3(-21.0f, 0.75f, -32.0f), glm::vec3(0.2f, 0.2f, 0.2f), view, projection, statuaWoman, rotation, angle);
      //  std::cout<<ourCamera.Position.x<<" "<<ourCamera.Position.y<<" "<<ourCamera.Position.z<<"\n";


        //FIRE HAZARD

        //view = glm::mat4(1.0f);
        flashlightShader.setMat4("view", view);
        flashlightShader.setMat4("projection", projection);

        //glm::vec3 lightPosition = glm::vec3(+3.8f, 0.0f, -3.0f);
        model = glm::mat4(1.0f);
        float flashlightDistance = zNear + 0.3f;
        // ourCamera.Position + flashlightDistance * ourCamera.Front
        glm::vec3 flashlightVector = ( ourCamera.Position + (flashlightDistance * ourCamera.Front));
        //flashlightVector += glm::vec3(3.0f, 0.0f, 0.0f);
        glm::vec3 right = glm::cross(flashlightVector, ourCamera.Up);
        glm::vec3 newVec = ourCamera.GetViewMatrix() * glm::vec4(right, 0.0);
        model = glm::translate(model, newVec);
        model = glm::rotate(model, glm::radians(+180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        //model = glm::rotate(model, glm::radians((float)(2 * M_PI / ourCamera.MouseSensitivity)), ourCamera.Front);
        // razlika ugla novogfront i starog front

        //model = glm::rotate(model, diffrencePitch, glm::vec3(1.0f, 0.0f, 0.0f));
        //model = glm::rotate(model, diffrenceYaw, glm::vec3(0.0f, 1.0f, 0.0f));

        //model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
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

void placePainting(Shader paintingShader,glm::vec3 translation, glm::vec3 scale,glm::mat4 view, glm::mat4 projection, Model painting, float angleX, float angleY, float angleZ)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    paintingShader.setMat4("view", view);
    paintingShader.setMat4("projection", projection);
    paintingShader.setMat4("model", model);
    painting.Draw(paintingShader);
}

void placeStatue(Shader statueShader,glm::vec3 translation, glm::vec3 scale,glm::mat4 view, glm::mat4 projection, Model statue, glm::vec3 rotation, float angle)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(angle), rotation);
    model = glm::scale(model, scale);
    statueShader.setMat4("view", view);
    statueShader.setMat4("projection", projection);
    statueShader.setMat4("model", model);
    statue.Draw(statueShader);
}

void setBorder(){
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
    else if((ourCamera.Position.x < 5.9 || ourCamera.Position.x > -8.0) && ourCamera.Position.z > -26.6 && ourCamera.Position.z < -14.2){
        if(ourCamera.Position.x > 5.9 || ourCamera.Position.x < -8.0) {}
        else {
            if (abs(6.44959 - ourCamera.Position.x) < abs(-8.34407 - ourCamera.Position.x))
                ourCamera.Position.x = 5.9;
            else
                ourCamera.Position.x = -8.0;
        }
    }
    else if((ourCamera.Position.z < -13.5 || ourCamera.Position.z > -27.3 ) && ourCamera.Position.x > -8.0 && ourCamera.Position.x < 5.9){
        if(ourCamera.Position.z > -13.5 || ourCamera.Position.z < -27.3) {}
        else {
            if (abs( -7.70539 - ourCamera.Position.z) < abs(-33.8069 - ourCamera.Position.z))
                ourCamera.Position.z = -13.5;
            else
                ourCamera.Position.z = -27.3;
        }

    }
}
