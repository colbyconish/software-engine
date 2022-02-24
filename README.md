<h1>software-engine</h1>
<h3>Dependincies</h3>
<ul>
	<li>glad</li>
	<li>GLFW3</li>
	<li>GLFW3native</li>
	<li>GLM</li>
	<li>STB</li>
	<li>LUA</li>
	<li>RTTR (editted .h files)</li>
	<li>FREETYPE</li>
</ul>

<p>We have a <a href="https://trello.com/b/nJ8ITIK7/software-engine-swe">trello board</a> with all our current milestones.</p>

#Starter Example : main.cpp
	```c++
	int main()
	{
		//create window
		initGLFW();
		window_ptr main = Window::createWindow(800, 600, "Game Test");
		main->makeCurrent();//glad needs a current context to init
		initGLAD();

		//load textures
		Texture defaultTexture = Texture::loadTexture("textures/default.jpg", textureType::diffuse);
		Texture containerTexture = Texture::loadTexture("textures/container.jpg", textureType::diffuse);

		//create vertexs and indicies
		std::vector<Vertex> verts =
		{
			Vertex{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)}, Vertex{glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
			Vertex{glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f)}, Vertex{glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f)}
		};

		std::vector<uint32_t> indis =
		{
			0, 1, 2,
			1, 3, 2
		};

		//create mesh
		mesh_ptr mesh = Mesh::createMesh(verts, indis, defaultTexture);

		//create model
		model_ptr model = Model::createModel();
		model->addMesh(mesh);

		//create objects
		object_ptr object = Object::createObject();
		object->getComponent<Transform>()->position->z = -3.0f;
		object->addComponent(model);

		//create scene
		scene_ptr scene = Scene::createScene();
		scene->background_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		scene->addObject(object);

		//load scene
		main->loadScene(scene);

		LuaController::init();
		int32_t err = LuaController::runFile("script.lua");
		LuaController::close();

		Window::Main();
		return 0;
	}
	```

# NOTES 
Tab separators can be added to the top level menu but are not displayed.
