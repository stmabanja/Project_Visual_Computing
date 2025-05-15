#include "Scene.h"
#include <AssetManager.h>

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

		/*float vertices[] = {-0.5, -0.5, 0.0, 0.0, 1.0,
							0.5, -0.5, 0.0, 0.0, 1.0,
							0.5, 0.5, 0.0, 1.0, 0.0,
							0.0, 1.0, 1.0, 0.0, 0.0,
							-0.5, 0.5, 0.0, 1.0, 0.0};

		int indices[] = {0, 1, 2,
						 0, 2, 4,
						 4, 2, 3};

		glGenBuffers(1, &vboID); //ID generieren
		glBindBuffer(GL_ARRAY_BUFFER, vboID ); //Buffer aktivieren
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); // Hochladen der Daten auf die GPU

		 glGenVertexArrays(1, &vaoID); //ID generieren
		 glBindVertexArray(vaoID); //VAO aktivieren

		// Define vertex attributes
		// Positions
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0); //Einschalten Attribute for Position.

		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(2* sizeof(float)));
		glEnableVertexAttribArray(1); //Einschalten Attribute for Colors.

		//Create Index Buffer Object
		GLuint iboID;
		glGenBuffers(1, &iboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

		//Unbind VAO
		glBindVertexArray(0);
		//Unbind VBO
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Only the front sides of the triangles are rendered
		glEnable(GL_CULL_FACE); // enables face culling
		glFrontFace(GL_CCW); // counterclockwise //OpenGL considers triangles drawn as CCW as front facing, // all CW are considered here as backfacing.
		glCullFace(GL_BACK); // says OpenGL to enable backface culling means the side not facing towards camera will be hidden.*/



		float vertices[] = {
			// x,     y,      r, g, b
			0,  0,		1, 0, 0,
			0.1f, 0,		1, 0, 0,
            0.1f, 0.2f,		1, 0, 0,
			0 , 0.3f,		1, 0, 0,
			0.4f , 0.3f,		0, 1, 0,
			0.4f , 0.2f,		0, 1, 0,
			0.4f , 0,		0, 0, 1,
			0.3f , 0,		0, 0, 1,
			0.3f, 0.2f,		0, 0, 1,
			0.1f, 0.1f,		1, 0, 0,
			0.1f, 0.5f,		1, 0, 0,
			0.1f, 0.3f,		1, 0, 0,
			0, 0.6f,		0, 1, 0,
			0.4f, 0.6f,		0, 1, 0,
			0.4f, 0.5f,		0, 1, 0,
			0.3f, 0.5f,		0, 0, 1,
			0.3f, 0.3f,		0, 0, 1,
			0.3f, 0.4f,		0, 0, 1,
			0.1f, 0.4f,		0, 0, 1,
			0.3f ,0.1f,      0, 0, 1
		};

		int indices[]= {

			0, 1, 2,
			0, 2, 3,
			2, 4, 3,
			2, 4, 5,
			5, 8, 7,
			7, 6, 5,
			1, 19, 9,
			1, 7, 19,
			3, 10, 12,
			3, 11, 10,
			10, 12, 13,
			10, 14, 13,
			15, 14, 16,
			14, 16, 4,
			11, 17, 18,
			11, 16, 17,
		};





		glGenBuffers(1, &vboID); //ID generieren
		glBindBuffer(GL_ARRAY_BUFFER, vboID ); //Buffer aktivieren
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); // Hochladen der Daten auf die GPU

		glGenVertexArrays(1, &vaoID); //ID generieren
		glBindVertexArray(vaoID); //VAO aktivieren

		// Define vertex attributes
		// Positions
		glVertexAttribPointer(0, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0); //Einschalten Attribute for Position.

		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 5 * sizeof(float), (void*)(2* sizeof(float)));
		glEnableVertexAttribArray(1); //Einschalten Attribute for Colors.

		//Create Index Buffer Object
		GLuint iboID;
		glGenBuffers(1, &iboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

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
	m_shader->use();


	// Bind VAO
	glBindVertexArray(vaoID);

	// Draw triangle
	glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, 0);


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
