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

<h1>Starter Example</h1>
<h2>main.cpp</h2>

```cpp
#include <SWE/SWE.h>

using namespace swe;

#ifdef _DEBUG
int main(int argc, char *argv[]){
#endif
#ifndef _DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
#endif
	
	Application::Init();

	//create window
	window_ptr main = Application::createWindow(1000, 800, "Game Test", 0, 0, true);
	main->makeCurrent();//glad needs a current context to init

	Application::initGLAD();

	//create scene
	scene_ptr scene = Scene::createScene();
	main->loadScene(scene);

	//create objects
	camera_ptr camera = Camera::createCamera();
	object_ptr lightCube = Object::createPrimitive(primitiveType::Cube);
	object_ptr cube = Object::createPrimitive(primitiveType::Cube);
	object_ptr plane = Object::createPrimitive(primitiveType::Cube);

	//add objects to scene
	scene->addObject(camera);
	scene->addLight(lightCube);
	scene->addObject(cube);
	scene->addObject(plane);

	//create components
	script_ptr controller = Script::createScript("scripts/playerController.lua");
	script_ptr rainbow = Script::createScript("scripts/rainbow.lua");
	//script_ptr circle = Script::createScript("scripts/circle.lua");
	light_ptr light = Light::createDirectional(glm::vec3(1.0f, 1.0f, 1.0f));
	//light_ptr light = Light::createSpot(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::cos(glm::radians(13.5f)), glm::cos(glm::radians(152.5f)));
	//light_ptr light = Light::createPoint(glm::vec3(1.0f, 1.0f, 1.0f), 0.00014f, 0.002f, 1.0f);
	script_ptr pulsate = Script::createScript("scripts/pulsate.lua");

	//add components to objects
	camera->addComponent(controller);
	lightCube->addComponent(light);
	//lightCube->addComponent(circle);
	lightCube->addComponent(rainbow);
	cube->addComponent(pulsate);
	
	//manipulate data
	transform_ptr t1 = camera->getComponent<Transform>();
	t1->position->z = 20.0f;
	
	transform_ptr t = plane->getComponent<Transform>();
	t->scale->x = 100;
	t->scale->z = 100;
	t->scale->y = 0.1f;
	t->position->y = -30;

	scene->background_color = glm::vec4(0.25f);
	scene->currentCamera = camera;

	Application::Main();
	return 0;
}
```
<h2>default.vs</h2>

```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 bTexCoord;
out vec3 bFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    bFragPos = vec3(model * vec4(aPos, 1.0));
    bTexCoord = aTexCoord;
}
```
<h2>default.gs</h2>

```glsl
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 bTexCoord[3];
in vec3 bFragPos[3];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 norm = cross(v1, v2);

    for(int i = 0;i < gl_in.length();i++)
    {
        TexCoord = bTexCoord[i];
        FragPos = bFragPos[i];
        Normal = norm;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}
```
<h2>default.fs</h2>

```glsl
#version 330 core
struct Material
{
    vec3 ambient;
    float shininess;
    sampler2D diffuse;
    sampler2D specular;
};

struct d_light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct p_light
{
    vec3 position;
    float quadratic;
    float linear;
    float constant;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct s_light
{
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Material material; 
uniform vec3 viewPos;

#define NUM_D_LIGHTS 5 //5 * 4 * vec3 = 48B
uniform d_light d_lights[NUM_D_LIGHTS];

#define NUM_P_LIGHTS 10//10 * 
uniform p_light p_lights[NUM_P_LIGHTS];

#define NUM_S_LIGHTS 10//10 * 
uniform s_light s_lights[NUM_S_LIGHTS];

vec3 CalcDirLight(d_light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
    return (ambient + diffuse + specular);
} 

vec3 CalcPointLight(p_light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance) + 0.000000001);      
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular) * attenuation;
} 

vec3 CalcSpotLight(s_light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);  

    vec3 diffuse = vec3(0,0,0), specular = vec3(0,0,0);
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    if(theta > light.innerCutOff) 
    {
        diffuse = light.diffuse * vec3(texture(material.diffuse, TexCoord));
        specular = light.specular * vec3(texture(material.specular, TexCoord));
    }

    diffuse *= intensity;
    specular *= intensity;
    
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result;

    for (int i = 0; i < NUM_D_LIGHTS;i++)
        result += CalcDirLight(d_lights[i], norm, viewDir);
    
    for (int i = 0; i < NUM_P_LIGHTS;i++)
        result += CalcPointLight(p_lights[i], norm, FragPos, viewDir);

    for (int i = 0; i < NUM_S_LIGHTS;i++)
        result += CalcSpotLight(s_lights[i], norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
```
