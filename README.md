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

<h1>Starter Example : main.cpp</h1>

```cpp
#include <SWE/swe.h>

using namespace swe;

int main()
{
	Application::Init();
	window_ptr main = Application::createWindow(1000, 800, "Game Test", 0, 0, false);

	main->makeCurrent();//glad needs a current context to init
	Application::initGLAD();

	//Create test cube
	object_ptr cube = Object::createPrimitive(primitiveType::Cube);
	transform_ptr t = cube->getComponent<Transform>();
	t->position->z = -3.0f;

	//Create scene
	scene_ptr scene = Scene::createScene();
	scene->background_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	scene->addObject(cube);

	//Load scene
	main->loadScene(scene);

	Application::Main();
	return 0;
}
```
