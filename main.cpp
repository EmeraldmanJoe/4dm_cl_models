//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>
#include <glm/gtc/random.hpp>

using namespace fdm;

// Initialize the DLLMain
initDLL

glm::vec3 ironPickTUV[40];
glm::vec3 deadlyPickTUV[40];
MeshRenderer ironPickaxeRenderer;
MeshRenderer deadlyPickaxeRenderer;

std::unique_ptr<Item> dummy;
$hook(void, StateGame, init, StateManager& s)
{
	original(self, s);
	
	// generate custom colors
	glm::vec3* ironTUVPos = ironPickTUV;
	glm::vec3* deadlyTUVPos = deadlyPickTUV;

	// customizable color values
	glm::vec3 ironPickA{ 0.7, 0.35, 0.0 };
	glm::vec3 ironPickB{ 0.5, 0.25, 0.0 };
	glm::vec3 deadlyPickA{ 1, 1, 0 };
	glm::vec3 deadlyPickB{ 0, 1, 0 };
	glm::vec3 deadlyPickC{ 0, 1, 1 };
	glm::vec3 deadlyPickD{ 0, 0, 1 };

	// default color values
	//glm::vec3 ironPickA{ 0.7, 0.7, 0.7 };
	//glm::vec3 ironPickB{ 0.5, 0.5, 0.5 };
	//glm::vec3 deadlyPickA{ 1, 0, 1 };
	//glm::vec3 deadlyPickB{ 1, 0, 0 };
	//glm::vec3 deadlyPickC{ 1, 0, 1 };
	//glm::vec3 deadlyPickD{ 0, 1, 1 };

	for (int i = 0; i < 40; ++i)
	{
		*ironTUVPos = (glm::linearRand(0.0f, 1.0f) > 0.5 ? ironPickA : ironPickB);
		if (i < 20)
		{
			*deadlyTUVPos = (glm::linearRand(0.0f, 1.0f) > 0.5 ? deadlyPickA : deadlyPickB);
		}
		else
		{
			*deadlyTUVPos = (glm::linearRand(0.0f, 1.0f) > 0.5 ? deadlyPickC : deadlyPickD);
		}

		++ironTUVPos;
		++deadlyTUVPos;
	}

	MeshBuilder ironPickaxeMesh{ 40 };
	ironPickaxeMesh.addBuff(fdm::ItemTool::pickaxeVerts, 640);
	ironPickaxeMesh.addAttr(GL_FLOAT, 4, 0);
	ironPickaxeMesh.addBuff(ironPickTUV, sizeof(ironPickTUV));
	ironPickaxeMesh.addAttr(GL_FLOAT, 3, 0);
	ironPickaxeMesh.addBuff(fdm::ItemTool::pickaxeNormals, 160);
	ironPickaxeMesh.addAttr(GL_FLOAT, 1, 0);

	ironPickaxeRenderer.setMesh(&ironPickaxeMesh);

	MeshBuilder deadlyPickaxeMesh{ 40 };
	deadlyPickaxeMesh.addBuff(fdm::ItemTool::pickaxeVerts, 640);
	deadlyPickaxeMesh.addAttr(GL_FLOAT, 4, 0);
	deadlyPickaxeMesh.addBuff(deadlyPickTUV, sizeof(deadlyPickTUV));
	deadlyPickaxeMesh.addAttr(GL_FLOAT, 3, 0);
	deadlyPickaxeMesh.addBuff(fdm::ItemTool::pickaxeNormals, 160);
	deadlyPickaxeMesh.addAttr(GL_FLOAT, 1, 0);

	deadlyPickaxeRenderer.setMesh(&deadlyPickaxeMesh);

	nlohmann::json j{ { "name", "Stick" }, {"count", 1} };
	dummy = std::move(Item::createFromJson(j));
}

$hook(void, StateIntro, init, StateManager& s)
{
	original(self, s);

	// initialize opengl stuff
	glewExperimental = true;
	glewInit();
	glfwInit();
}

$hook(void, ItemTool, renderEntity, const m4::Mat5& mat, bool inHand, const glm::vec4& lightDir)
{
	std::string name = self->getName();
	if (name == "Iron Pick" || name == "Deadly Pick")
	{
		const Shader* normalShader = ShaderManager::get("tetColorNormalShader");
		normalShader->use();

		glUniform1fv(glGetUniformLocation(normalShader->id(), "MV"), sizeof(mat) / sizeof(float), &mat[0][0]);

		glUniform4fv(glGetUniformLocation(normalShader->id(), "lightDir"), 1, &lightDir[0]);

		if (name == "Iron Pick")
		{
			ironPickaxeRenderer.render();
		}
		else
		{
			deadlyPickaxeRenderer.render();
		}

		original((fdm::ItemTool*)dummy.get(), mat, inHand, lightDir);
	}
	else
	{

		original(self, mat, inHand, lightDir);
	}
}