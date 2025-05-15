#include "Scene.h"
#include <AssetManager.h>
#include <Cube.h>

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
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0); //Einschalten Attribute for Position.

		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(2* sizeof(float)));
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
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		

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
cubeTrans = new Transform();
m_shader->use();
	

	glm::vec3 rotationEulerAngles(dt * glm::radians(10.0f), dt * glm::radians(20.0f), 0.0f);


	cubeTrans->rotate(rotationEulerAngles); // <--- apply rotation

	// Pass the transform matrix to the shader
	m_shader->setUniform("modelMatrix", cubeTrans->getMatrix(), false);

	// Bind VAO
	glBindVertexArray(vaoID);

	// Draw triangle
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
