#include "Scene.h"
#include <AssetManager.h>
#include "cubeWithNormals.h"

Scene::Scene(OpenGLWindow * window) :
        m_window(window)
{
    assert(window != nullptr);
}

Scene::~Scene()
{}

bool Scene::init()
{
    try
    {
        //Load shader
        m_assets.addShaderProgram("shader", AssetManager::createShaderProgram("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl"));
        m_shader = m_assets.getShaderProgram("shader");
        m_shader->use();

        // Create Vertex Buffer Object for cubeVertWithNormals
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertWithNormals), cubeVertWithNormals, GL_STATIC_DRAW);

        // Create Vertex Array Object for cubeVertWithNormals
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        // Define vertex attributes for cubeVertWithNormals
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);


        // Unbind VAO
        glBindVertexArray(0);

        // Unbind VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Only the front sides of the triangles are rendered
         glEnable(GL_CULL_FACE);
         glCullFace(GL_BACK);

         // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);       // Default-Value, könnte auch weggelassen werden
        glClearDepth(1.0);         // Default-Value, könnte auch weggelassen werden


        std::cout << "Scene initialization done\n";
        return true;
    }
    catch (std::exception& ex)
    {
        throw std::logic_error("Scene initialization failed:\n" + std::string(ex.what()) + "\n");
    }
}

void Scene::render(float dt)
{
    // Hintergrund löschen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Aktivieren des Shaders
    m_shader->use();

    glBindVertexArray(vaoID);

    // Lichtquelle
    m_shader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    float time = glfwGetTime();
    float radius = 2.0f; // Radius of the circular path
    glm::vec3 lightPos = glm::vec3(sin(time) * radius, 1.0f, cos(time) * radius); // Circular path
    m_shader->setUniform("lightPos", lightPos);


    // Rendere cube an der Position der Lichtquelle
    Transform cubeVertWithNormalsTransform;
    cubeVertWithNormalsTransform.scale(glm::vec3(0.1f, 0.1f, 0.1f));
    cubeVertWithNormalsTransform.translate(lightPos);
    m_shader->setUniform("modelMatrix", cubeVertWithNormalsTransform.getMatrix(), false);
    m_shader->setUniform("applyLighting", false); // Disable lighting for the light source
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // View-Matrix automatisch berechnen mit lookAt (extrinsische Kameraparameter: Position und Ausrichtung)
    // Komplette Szene transformieren, dass die Kamera im Ursprung ist und entlang der negativen Z-Achse schaut
    glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.5f, 2.0f), // Kamerazentrum c (Position)
            glm::vec3(0.0f, 0.0f, 0.0f), // Betrachtungspunkt a (Blickpunkt auf Ursprung)
            glm::vec3(0.0f, 1.0f, 0.0f)  // Up-Vektor u
    );
    m_shader->setUniform("viewMatrix", viewMatrix, false);

    // Projektionsmatrix berechnen (intrinsische Kameraparameter: Sichtfeld, Seitenverhältnis, Nah- und Fern-Clipping Ebenen)
    glm::mat4 projMatrix = glm::perspective(
            glm::radians(60.0f),       // Sichtfeld (Field of View)
            1.0f,                            // Seitenverhältnis Kamera und des Fensters sollten gleich
            0.1f,                            // Nah-Clipping Ebenen (Mindestentfernung zur Kamera)
            100.0f                            // Fern-Clipping Ebenen (Maximale Entfernung zur Kamera)
            );
    m_shader->setUniform("projMatrix", projMatrix, false);

    m_shader->setUniform("applyLighting", true); // Enable lighting for the robot

    // Rotieren um die X- und Y-Achse
    glm::vec3 rotationEulerAngles(dt * glm::radians(10.0f), dt * glm::radians(20.0f), 0.0f);

    // Transformationsmatrix für den gesamten Roboter
    glm::mat4 robotTransform = glm::mat4(1.0f);
    robotTransform = glm::translate(robotTransform, glm::vec3(0.0f, 0.0f, 0.0f)); // Translation
    robotTransform = glm::rotate(robotTransform, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation
    robotTransform = glm::scale(robotTransform, glm::vec3(0.5f, 0.5f, 0.5f)); // Skalierung

    // Transformationsmatrix für den Körper
    Transform bodyTransform;
    bodyTransform.scale(glm::vec3(1.0f, 1.5f, 0.5f));
    bodyTransform.setMatrix(robotTransform * bodyTransform.getMatrix());
    m_shader->setUniform("modelMatrix", bodyTransform.getMatrix(), false);
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Transformationsmatrix für den Kopf
    Transform headTransform;
    headTransform.scale(glm::vec3(0.5f, 0.5f, 0.5f)); // Kleine Skalierung
    headTransform.translate(glm::vec3(0.0f, 1.25f, 0.0f)); // Kopfposition nach oben verschieben
    headTransform.rotate(rotationEulerAngles);
    headTransform.setMatrix(robotTransform * headTransform.getMatrix());
    m_shader->setUniform("modelMatrix", headTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Swinging leg animation
    float elapsedTime = glfwGetTime(); // Gesamtzeit seit Start
    float swingAngle = sin(elapsedTime) * glm::radians(30.0f);

    // Transformationsmatrix für das linke Bein
    Transform leftLegTransform;
    leftLegTransform.scale(glm::vec3(0.5f, 1.0f, 0.5f)); // Bein ist lang und dünn
    leftLegTransform.translate(glm::vec3(-0.25f, -1.25f, 0.0f)); // Position des linken Beins
    leftLegTransform.rotateAroundPoint(glm::vec3(-0.25f, 0.0f, 0.0f), glm::vec3(swingAngle, 0.0f, 0.0f)); // Rotate around hip joint
    leftLegTransform.setMatrix(robotTransform * leftLegTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftLegTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Transformationsmatrix für das rechte Bein
    Transform rightLegTransform;
    rightLegTransform.scale(glm::vec3(0.5f, 1.0f, 0.5f)); // Rechts genauso wie links
    rightLegTransform.translate(glm::vec3(0.25f, -1.25f, 0.0f)); // Position des rechten Beins
    rightLegTransform.rotateAroundPoint(glm::vec3(0.25f, 0.0f, 0.0f), glm::vec3(-swingAngle, 0.0f, 0.0f)); // Rotate around hip joint
    rightLegTransform.setMatrix(robotTransform * rightLegTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightLegTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Swinging arm animation
    float armSwingAngle = sin(elapsedTime) * glm::radians(20.0f);

    // Transformationsmatrix für den linken Oberarm
    Transform leftUpperArmTransform;
    leftUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f));
    leftUpperArmTransform.translate(glm::vec3(-0.75f, 0.35f, 0.0f)); // Position des linken Oberarms
    leftUpperArmTransform.rotateAroundPoint(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    leftUpperArmTransform.setMatrix(robotTransform * leftUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftUpperArmTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Transformationsmatrix für den rechten Oberarm
    Transform rightUpperArmTransform;
    rightUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f)); // Rechts genauso wie links
    rightUpperArmTransform.translate(glm::vec3(0.75f, 0.35f, 0.0f)); // Position des rechten Oberarms
    rightUpperArmTransform.rotateAroundPoint(glm::vec3(0.75f, 0.75f, 0.0f), glm::vec3(-armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    rightUpperArmTransform.setMatrix(robotTransform * rightUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightUpperArmTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Transformationsmatrix für den linken Unterarm
    Transform leftLowerArmTransform;
    leftLowerArmTransform.scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Unterarm ist gleich groß wie der Oberarm
    leftLowerArmTransform.translate(glm::vec3(0.0f, -1.0f, 0.0f)); // Position des linken Unterarms
    leftLowerArmTransform.rotateAroundPoint(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around elbow joint
    leftLowerArmTransform.setMatrix(leftUpperArmTransform.getMatrix() * leftLowerArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftLowerArmTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Transformationsmatrix für den rechten Unterarm
    Transform rightLowerArmTransform;
    rightLowerArmTransform.scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Rechts genauso wie links
    rightLowerArmTransform.translate(glm::vec3(0.0f, -1.0f, 0.0f)); // Position des rechten Unterarms
    rightLowerArmTransform.rotateAroundPoint(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-armSwingAngle, 0.0f, 0.0f)); // Rotate around elbow joint
    rightLowerArmTransform.setMatrix(rightUpperArmTransform.getMatrix() * rightLowerArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightLowerArmTransform.getMatrix(), false);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}


void Scene::update(float dt)
{

}

OpenGLWindow * Scene::getWindow()
{
    return m_window;
}

void Scene::onKey(Key key, Action action, Modifier modifier)
{

}

void Scene::onMouseMove(MousePosition mouseposition)
{

}

void Scene::onMouseButton(MouseButton button, Action action, Modifier modifier)
{

}

void Scene::onMouseScroll(double xscroll, double yscroll)
{

}

void Scene::onFrameBufferResize(int width, int height)
{

}
void Scene::shutdown()
{

}
