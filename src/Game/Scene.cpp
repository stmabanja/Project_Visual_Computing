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
	try {

		//Load shader
		m_assets.addShaderProgram("shader", AssetManager::createShaderProgram("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl"));
		m_shader = m_assets.getShaderProgram("shader");
		m_shader->use();

		glGenBuffers(1, &vboID); //ID generieren
		glBindBuffer(GL_ARRAY_BUFFER, vboID ); //Buffer aktivieren
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVert), &cubeVert, GL_STATIC_DRAW); // Hochladen der Daten auf die GPU

		glGenVertexArrays(1, &vaoID); //ID generieren
		glBindVertexArray(vaoID); //VAO aktivieren

		// Define vertex attributes
		// Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0); //Einschalten Attribute for Position.

		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3* sizeof(float)));
		glEnableVertexAttribArray(1); //Einschalten Attribute for Colors.

		//Create Index Buffer Object
		GLuint iboID;
		glGenBuffers(1, &iboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInd), &cubeInd, GL_STATIC_DRAW);

		//Unbind VAO
		glBindVertexArray(0);
		//Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Only the front sides of the triangles are rendered
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glClearDepth(0.0);
		

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

	m_shader->use(); // Shader aktivieren

	static float angle = 0.0f; // track of rotation angle.
	angle += dt; // increment the angle based on delta time;

	//create a rotation vector
	glm::vec3 rotation( angle * 0.0f, angle * - 0.1f, 0.0f); // rotate on x and y axis

	/*
	 // Apply the rotation to the transformation matrix
	cubeTrans->setRotation(rotation);

	// Pass the transform matrix to the shader
	m_shader->setUniform("modelMatrix", cubeTrans->getMatrix(), false);
	*/

	// Transformationsmatrix für den gesamten Roboter
	glm::mat4 robotTransform = glm::mat4(1.0f);
	robotTransform = glm::translate(robotTransform, glm::vec3(0.0f, 0.0f, 0.0f)); // Translation move robot ( not moving now. So, lies on the middle)
	robotTransform = glm::rotate(robotTransform, glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation in Y axis
	robotTransform = glm::scale(robotTransform, glm::vec3(0.4f, 0.4f, 0.4f)); // Skalierung make the whole robot smaller.

	// Transformationsmatrix für den Körper
	Transform bodyTransform;
	bodyTransform.scale(glm::vec3(1.0f, 1.5f, 0.5f)); //skalierung: stretch taller und thinner.
	bodyTransform.setMatrix(robotTransform * bodyTransform.getMatrix()); // combines all transformation with the main robot transformation
	m_shader->setUniform("modelMatrix", bodyTransform.getMatrix(), false); // send headmatrix to shader
	glBindVertexArray(vaoID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // draw

	//Transformationsmatrix für den Kopf
	Transform headTransform;
	headTransform.scale(glm::vec3(0.5f, 0.5f, 0.5f)); // skalierung : make head smaller
	headTransform.translate(glm::vec3(0.0f, 1.25f, 0.0f)); //Translation: Move the head above body
	headTransform.rotate(rotation); // rotation: rotate head
	headTransform.setMatrix(robotTransform * headTransform.getMatrix()); // combines all transformation with the main robot transformation
	m_shader->setUniform("modelMatrix", headTransform.getMatrix(), false);// send headmatrix to shader
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // draw

	// Swinging leg animation
	static float totalTime = 0.0f;
	totalTime += dt;
	float swingAngle = sin(totalTime) * glm::radians(30.0f); // swing angle for the legs.

	// Transformationsmatrix für das linke Bein
	Transform leftLegTransform;
	leftLegTransform.scale(glm::vec3(0.5f, 1.0f, 0.5f)); // Bein ist lang und dünn
	leftLegTransform.translate(glm::vec3(-0.25f, -1.25f, 0.0f)); // Move the left leg to the left and downward
	leftLegTransform.rotateAroundPoint(glm::vec3(-0.25f, 0.0f, 0.0f), glm::vec3(swingAngle, 0.0f, 0.0f)); // Rotate around hip joint
	leftLegTransform.setMatrix(robotTransform * leftLegTransform.getMatrix());
	m_shader->setUniform("modelMatrix", leftLegTransform.getMatrix(), false);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Transformationsmatrix für das rechte Bein
	Transform rightLegTransform;
	rightLegTransform.scale(glm::vec3(0.5f, 1.0f, 0.5f)); // Rechts genauso wie links
	rightLegTransform.translate(glm::vec3(0.25f, -1.25f, 0.0f)); //Move the right leg to the left and downward
	rightLegTransform.rotateAroundPoint(glm::vec3(0.25f, 0.0f, 0.0f), glm::vec3(-swingAngle, 0.0f, 0.0f)); // Rotate around hip joint
	rightLegTransform.setMatrix(robotTransform * rightLegTransform.getMatrix());
	m_shader->setUniform("modelMatrix", rightLegTransform.getMatrix(), false);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Swinging arm animation
	static float time = 0.0f;
	time += dt;
	float armSwingAngle = sin(totalTime) * glm::radians(20.0f);
	// Transformationsmatrix für den linken Oberarm
    Transform leftUpperArmTransform;
    leftUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f)); // Oberarm ist dick und kurz
    leftUpperArmTransform.translate(glm::vec3(-0.75f, 0.75f, 0.0f)); // Position des linken Oberarms
    leftUpperArmTransform.rotateAroundPoint(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    leftUpperArmTransform.setMatrix(robotTransform * leftUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", leftUpperArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den rechten Oberarm
    Transform rightUpperArmTransform;
    rightUpperArmTransform.scale(glm::vec3(0.2f, 0.75f, 0.25f)); // Rechts genauso wie links
    rightUpperArmTransform.translate(glm::vec3(0.75f, 0.75f, 0.0f)); // Position des rechten Oberarms
    rightUpperArmTransform.rotateAroundPoint(glm::vec3(0.75f, 0.75f, 0.0f), glm::vec3(-armSwingAngle, 0.0f, 0.0f)); // Rotate around shoulder joint
    rightUpperArmTransform.setMatrix(robotTransform * rightUpperArmTransform.getMatrix());
    m_shader->setUniform("modelMatrix", rightUpperArmTransform.getMatrix(), false);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Transformationsmatrix für den linken Unterarm
    Transform leftLowerArmTransform;
    leftLowerArmTransform.scale(glm::vec3(1.0f, 1.0f, 1.0f)); // Unterarm ist gleich groß wie der Oberarm
    leftLowerArmTransform.translate(glm::vec3(0.0f, -1.0f, 0.0f)); // Position des linken Unterarms
    //leftLowerArmTransform.rotateAroundPoint(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(armSwingAngle, 0.0f, 0.0f)); // Rotate around elbow joint
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

	// Bind VAO
	glBindVertexArray(vaoID);

	// Draw triangle
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Unbind VAO
	glBindVertexArray(0);
}

/*void Scene::render(float dt)
{
	// Hintergrund löschen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

     m_shader->use(); // Shader aktivieren
	static float angle = 0.0f; // track of rotation angle.
	angle += dt; // increment the angle based on delta time;

	//create a rotation vector
	glm::vec3 rotation( angle * 0.9f, angle * 0.5f, 0.0f); // rotate on x and y axis

	// Apply the rotation to the transformation matrix
	cubeTrans->setRotation(rotation);

	// Pass the transform matrix to the shader
	m_shader->setUniform("modelMatrix", cubeTrans->getMatrix(), false);

	// Bind VAO
	glBindVertexArray(vaoID);

	// Draw triangle
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// Unbind VAO
	glBindVertexArray(0);
}*/

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
