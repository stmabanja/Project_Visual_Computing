#include "Scene.h"
#include <AssetManager.h>
#include "Cube.h"


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


        // Create Vertex Buffer Object
        glGenBuffers(1, &vboID);

        // Activate Vertex Buffer Object
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        // Copy vertices to VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVert), &cubeVert, GL_STATIC_DRAW);


        // Create Vertex Array Object
        glGenVertexArrays(1, &vaoID);

        // Activate Vertex Array Object
        glBindVertexArray(vaoID);

        // Define vertex attributes
        // Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 24, (void*)0);
        glEnableVertexAttribArray(0);

        // Colors
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 24, (void*)12);
        glEnableVertexAttribArray(1);

        // Create Index Buffer Object
        GLuint iboID;
        glGenBuffers(1, &iboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), &cubeInd, GL_STATIC_DRAW);

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


        //////////////////////////////////////////


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

    // Zeitvariable setzen
    float time = glfwGetTime();
    m_shader->setUniform("time", time);

    // View-Matrix automatisch berechnen mit lookAt (extrinsische Kameraparameter: Position und Ausrichtung)
    // Komplette Szene transformieren, dass die Kamera im Ursprung ist und entlang der negativen Z-Achse schaut
    glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f), // Kamerazentrum c (Position)
            glm::vec3(0.0f, 0.0f, -1.0f), // Betrachtungspunkt a (Blickpunkt auf Ursprung)
            glm::vec3(0.0f, 1.0f, 0.0f)  // Up-Vektor u
    );
    m_shader->setUniform("viewMatrix", viewMatrix, false);

    // Projektionsmatrix berechnen (intrinsische Kameraparameter: Sichtfeld, Seitenverhältnis, Nah- und Fern-Clipping Ebenen)
    glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(90.0f),       // Sichtfeld (Field of View)
            1.0f,                            // Seitenverhältnis Kamera und des Fensters sollten gleich
            0.1f,                            // Nah-Clipping Ebenen (Mindestentfernung zur Kamera)
            100.0f                            // Fern-Clipping Ebenen (Maximale Entfernung zur Kamera)
            );
    m_shader->setUniform("projectionMatrix", projectionMatrix, false);

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
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den Kopf
    Transform headTransform;
    headTransform.scale(glm::vec3(0.5f, 0.5f, 0.5f)); // Kleine Skalierung
    headTransform.translate(glm::vec3(0.0f, 1.25f, 0.0f)); // Kopfposition nach oben verschieben
    headTransform.rotate(rotationEulerAngles);
    headTransform.setMatrix(robotTransform * headTransform.getMatrix());
    m_shader->setUniform("modelMatrix", headTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

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
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für das rechte Bein
    Transform rightLegTransform;
    rightLegTransform.scale(glm::vec3(0.5f, 1.0f, 0.5f)); // Rechts genauso wie links
    rightLegTransform.translate(glm::vec3(0.25f, -1.25f, 0.0f)); // Position des rechten Beins
    rightLegTransform.rotateAroundPoint(glm::vec3(0.25f, 0.0f, 0.0f), glm::vec3(-swingAngle, 0.0f, 0.0f)); // Rotate around hip joint
    rightLegTransform.setMatrix(robotTransform * rightLegTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightLegTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Swinging arm animation
    float armSwingAngle = sin(elapsedTime) * glm::radians(20.0f);

    // Transformationsmatrix für den linken Oberarm
    Transform leftUpperArmTransform;
    leftUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f));
    leftUpperArmTransform.translate(glm::vec3(-0.75f, 0.35f, 0.0f)); // Position des linken Oberarms
    leftUpperArmTransform.rotateAroundPoint(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    leftUpperArmTransform.setMatrix(robotTransform * leftUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftUpperArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den rechten Oberarm
    Transform rightUpperArmTransform;
    rightUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f)); // Rechts genauso wie links
    rightUpperArmTransform.translate(glm::vec3(0.75f, 0.35f, 0.0f)); // Position des rechten Oberarms
    rightUpperArmTransform.rotateAroundPoint(glm::vec3(0.75f, 0.75f, 0.0f), glm::vec3(-armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    rightUpperArmTransform.setMatrix(robotTransform * rightUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightUpperArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den linken Unterarm
    Transform leftLowerArmTransform;
    leftLowerArmTransform.scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Unterarm ist gleich groß wie der Oberarm
    leftLowerArmTransform.translate(glm::vec3(0.0f, -1.0f, 0.0f)); // Position des linken Unterarms
    leftLowerArmTransform.rotateAroundPoint(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around elbow joint
    leftLowerArmTransform.setMatrix(leftUpperArmTransform.getMatrix() * leftLowerArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftLowerArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den rechten Unterarm
    Transform rightLowerArmTransform;
    rightLowerArmTransform.scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Rechts genauso wie links
    rightLowerArmTransform.translate(glm::vec3(0.0f, -1.0f, 0.0f)); // Position des rechten Unterarms
    rightLowerArmTransform.rotateAroundPoint(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(-armSwingAngle, 0.0f, 0.0f)); // Rotate around elbow joint
    rightLowerArmTransform.setMatrix(rightUpperArmTransform.getMatrix() * rightLowerArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightLowerArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Unbind VAO
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
