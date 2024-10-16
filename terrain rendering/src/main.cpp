#include <iostream>
using namespace std;
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <vector>
#include <limits>
#include "common/utils.hpp"
#include <fstream>
#include <sstream>
#include <common/controls.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
GLFWwindow* window;
static const int window_width = 1920;
static const int window_height = 1080;
const unsigned int width = 800;
const unsigned int height = 800;
static const int n_points = 200; //minimum 2
static const float m_scale = 5;
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint skyboxVAO;
GLuint skyboxVBO;
GLuint skyboxEBO;
GLuint uvbuffer;
GLuint normalmapID;
GLuint elementbuffer;
GLuint tangentbuffer;
GLuint textureID;
GLuint heightTextureID;
GLuint programID;
GLuint skyboxprogramID;
GLuint billboardprogramID;
GLuint stextureID;
GLuint shadingmapID;
GLuint grassID;
GLuint snowID;
GLuint grassnormalID;
GLuint grassrID;
GLuint snownID;
GLuint snowrID;
unsigned int nIndices;


bool initializeGL()
{// Initialise GLFW
	if (!glfwInit())
	{
		cerr << "Failed to initialize GLFW" << endl;
		return false;
	}
	glfwWindowHint(GLFW_SAMPLES, 1); //no anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(window_width, window_height, "OpenGLRenderer", NULL, NULL);
	if (window == NULL)
	{
		cerr << "Failed to open GLFW window. If you have an Intel GPU, they may not be 4.5compatible." << endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	// Initialize GLEW
	glewExperimental = true;// Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		glfwTerminate();
		return false;
	}
	//without
	int NumberOfExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
	for (int i = 0; i < NumberOfExtensions; i++) {
		const GLubyte* ccc = glGetStringi(GL_EXTENSIONS, i);
		if (strcmp(reinterpret_cast<const char*>(ccc), "GL_ARB_debug_output") != 0)//1.2
		{
			return -1;
		}
	}
	//with
	if (!GLEW_ARB_debug_output)
		return -1;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
}

void LoadHeight()
{
	int heightWidth, heightHeight;
	unsigned char* heightData = nullptr;
	loadBMP_custom("mountains_height.bmp", heightWidth, heightHeight, heightData);
	glGenTextures(1, &heightTextureID);
	glBindTexture(GL_TEXTURE_2D, heightTextureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, heightWidth, heightHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, heightData);
	delete[] heightData;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadNormalMap()
{
	int normalmapWidth, normalmapHeight;
	unsigned char* normalmapData = nullptr;
	loadBMP_custom("rocks-n.bmp", normalmapWidth, normalmapHeight, normalmapData);
	glGenTextures(1, &normalmapID);
	glBindTexture(GL_TEXTURE_2D, normalmapID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalmapWidth, normalmapHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, normalmapData);
	delete[] normalmapData;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void LoadModel()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	for (int i = 0; i < n_points; i++)
	{
		float x = (m_scale) * ((i / float(n_points - 1)) - 0.5f) * 2.0f;
		for (int j = 0; j < n_points; j++)
		{
			float z = (m_scale) * ((j / float(n_points - 1)) - 0.5f) * 2.0f;
			float u = float(i + 0.5f) * 0.99 / float(n_points - 1);
			float v = float(j + 0.5f) * 0.99 / float(n_points - 1);
			vertices.push_back(vec3(x, 0, z));
			uvs.push_back(vec2(u, v));
		}
	}
	//glEnable(GL_PRIMITIVE_RESTART);
	//constexpr unsigned int restartIndex = std::numeric_limits<std::uint32_t>::max();
	//glPrimitiveRestartIndex(restartIndex);
	//int n = 0;

	for (int i = 0; i < n_points-1; i++)
	{
		for (int j = 0; j < n_points-1; j++)
		{
			int index = i * n_points + j;
			//unsigned int topLeft = n;
			//unsigned int bottomLeft = topLeft + n_points;
			indices.push_back(index);
			indices.push_back(index + 1);
			indices.push_back(index + n_points + 1);
			indices.push_back(index + n_points);
			//n++;
		}
		//indices.push_back(restartIndex);
	}
	nIndices = indices.size();
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0, // attribute
		3, // size (we have x,y,z)
		GL_FLOAT, // type of each individual element
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void LoadTextures()
{
	int width, height;
	unsigned char* data = nullptr;
	loadBMP_custom("rocks.bmp", width, height, data);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	delete[] data;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int shadingmapwidth, shadingmapheight;
	unsigned char* shadingmapdata = nullptr;
	loadBMP_custom("rocks-r.bmp", shadingmapwidth, shadingmapheight, shadingmapdata);
	glGenTextures(1, &shadingmapID);
	glBindTexture(GL_TEXTURE_2D, shadingmapID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, shadingmapwidth, shadingmapheight, 0, GL_BGR, GL_UNSIGNED_BYTE, shadingmapdata);
	delete[] shadingmapdata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int grasswidth, grassheight;
	unsigned char* grassdata = nullptr;
	loadBMP_custom("grass.bmp", grasswidth, grassheight, grassdata);
	glGenTextures(1, &grassID);
	glBindTexture(GL_TEXTURE_2D, grassID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grasswidth, grassheight, 0, GL_BGR, GL_UNSIGNED_BYTE, grassdata);
	delete[] grassdata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int grassnwidth, grassnheight;
	unsigned char* grassndata = nullptr;
	loadBMP_custom("grass-n.bmp", grassnwidth, grassnheight, grassndata);
	glGenTextures(1, &grassnormalID);
	glBindTexture(GL_TEXTURE_2D, grassnormalID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassnwidth, grassnheight, 0, GL_BGR, GL_UNSIGNED_BYTE, grassndata);
	delete[] grassndata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int grassrwidth, grassrheight;
	unsigned char* grassrdata = nullptr;
	loadBMP_custom("grass-r.bmp", grassrwidth, grassrheight, grassrdata);
	glGenTextures(1, &grassrID);
	glBindTexture(GL_TEXTURE_2D, grassrID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassrwidth, grassrheight, 0, GL_BGR, GL_UNSIGNED_BYTE, grassrdata);
	delete[] grassrdata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int snowwidth, snowheight;
	unsigned char* snowdata = nullptr;
	loadBMP_custom("snow.bmp", snowwidth, snowheight, snowdata);
	glGenTextures(1, &snowID);
	glBindTexture(GL_TEXTURE_2D, snowID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snowwidth, snowheight, 0, GL_BGR, GL_UNSIGNED_BYTE, snowdata);
	delete[] snowdata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int snownwidth, snownheight;
	unsigned char* snowndata = nullptr;
	loadBMP_custom("snow-n.bmp", snownwidth, snownheight, snowndata);
	glGenTextures(1, &snownID);
	glBindTexture(GL_TEXTURE_2D, snownID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snownwidth, snownheight, 0, GL_BGR, GL_UNSIGNED_BYTE, snowndata);
	delete[] snowndata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	int snowrwidth, snowrheight;
	unsigned char* snowrdata = nullptr;
	loadBMP_custom("snow-r.bmp", snowrwidth, snowrheight, snowrdata);
	glGenTextures(1, &snowrID);
	glBindTexture(GL_TEXTURE_2D, snowrID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snowrwidth, snowrheight, 0, GL_BGR, GL_UNSIGNED_BYTE, snowrdata);
	delete[] snowrdata;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void setSkybox() {
	float skyboxVertices[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//0     
		 1.0f, -1.0f,  1.0f,//1      
		 1.0f, -1.0f, -1.0f,//2      
		-1.0f, -1.0f, -1.0f,//3     
		-1.0f,  1.0f,  1.0f,//4      
		 1.0f,  1.0f,  1.0f,//5  
		 1.0f,  1.0f, -1.0f,//6  
		-1.0f,  1.0f, -1.0f //7
	};
	unsigned int skyboxIndices[] =
	{
		7,3,2,2,6,7,
		0,3,7,7,4,0,
		2,1,5,5,6,2,
		0,4,5,5,1,0,
		7,6,5,5,4,7,
		3,0,1,1,2,3
	};
	//vao & vbo
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glEnableVertexAttribArray(2);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glGenBuffers(1, &skyboxEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
}

void LoadSkybox() {
	vector<std::string> faces{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg",
	};
	glGenTextures(1, &stextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, stextureID);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool readAndCompileShader(const char* shader_path, const GLuint& id)
{
	string shaderCode;
	ifstream shaderStream(shader_path, std::ios::in);
	if (shaderStream.is_open())
	{
		stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	}
	else
	{
		cout << "Impossible to open " << shader_path << ". Are you in the rightdirectory?" << endl;
		return false;
	}
	cout << "Compiling shader :" << shader_path << endl;
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(id, 1, &sourcePointer, NULL);
	glCompileShader(id);
	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		vector<char> shaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(id, InfoLogLength, NULL, &shaderErrorMessage[0]);
		cout << &shaderErrorMessage[0] << endl;
	}
	cout << "Compilation of Shader: " << shader_path << " " << (Result == GL_TRUE ? "Success" : "Failed!") << endl;
	return Result == 1;
}

void LoadShadersT(GLuint& program, const char* vertex_file_path, const char* fragment_file_path, const char* tesc_file_path, const char* tese_file_path)
{
	// Create the shaders - tasks 1 and 2
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint TesselationControlID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint TesselationEvalID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	//GLuint GeometryID = glCreateShader(GL_GEOMETRY_SHADER);
	bool vok = readAndCompileShader(vertex_file_path, VertexShaderID);
	bool fok = readAndCompileShader(fragment_file_path, FragmentShaderID);
	bool tescok = readAndCompileShader(tesc_file_path, TesselationControlID);
	bool teseok = readAndCompileShader(tese_file_path, TesselationEvalID);
	//bool geomok = readAndCompileShader(geom_file_path, GeometryID);
	if (vok && fok && tescok && teseok)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		cout << "Linking program" << endl;
		program = glCreateProgram();
		glAttachShader(program, VertexShaderID);
		glAttachShader(program, FragmentShaderID);
		glAttachShader(program, TesselationControlID);
		glAttachShader(program, TesselationEvalID);
		//glAttachShader(program, GeometryID);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &Result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cout << &ProgramErrorMessage[0];
		}
		std::cout << "Linking program: " << (Result == GL_TRUE ? "Success" : "Failed!") << std::endl;
	}
	else
	{
		std::cout << "Program will not be linked: one of the shaders has an error" << std::endl;
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(TesselationControlID);
	glDeleteShader(TesselationEvalID);
	//glDeleteShader(GeometryID);
}

void LoadShadersB(GLuint& program, const char* vertex_file_path, const char* fragment_file_path, const char* geom_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint GeometryID = glCreateShader(GL_GEOMETRY_SHADER);
	bool vok = readAndCompileShader(vertex_file_path, VertexShaderID);
	bool fok = readAndCompileShader(fragment_file_path, FragmentShaderID);
	bool geomok = readAndCompileShader(geom_file_path, GeometryID);
	if (vok && fok && geomok)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		cout << "Linking program" << endl;
		program = glCreateProgram();
		glAttachShader(program, VertexShaderID);
		glAttachShader(program, FragmentShaderID);
		glAttachShader(program, GeometryID);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &Result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cout << &ProgramErrorMessage[0];
		}
		std::cout << "Linking program: " << (Result == GL_TRUE ? "Success" : "Failed!") << std::endl;
	}
	else
	{
		std::cout << "Program will not be linked: one of the shaders has an error" << std::endl;
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	glDeleteShader(GeometryID);
}

void LoadShadersS(GLuint& program, const char* vertex_file_path, const char* fragment_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	bool vok = readAndCompileShader(vertex_file_path, VertexShaderID);
	bool fok = readAndCompileShader(fragment_file_path, FragmentShaderID);
	if (vok && fok)
	{
		GLint Result = GL_FALSE;
		int InfoLogLength;
		cout << "Linking program" << endl;
		program = glCreateProgram();
		glAttachShader(program, VertexShaderID);
		glAttachShader(program, FragmentShaderID);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &Result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			cout << &ProgramErrorMessage[0];
		}
		std::cout << "Linking program: " << (Result == GL_TRUE ? "Success" : "Failed!") << std::endl;
	}
	else
	{
		std::cout << "Program will not be linked: one of the shaders has an error" << std::endl;
	}
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

void UnloadModel()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &skyboxEBO);
}


void UnloadTextures()
{
	glDeleteTextures(1, &textureID);
}


void UnloadShaders()
{
	glDeleteProgram(programID);
	glDeleteProgram(skyboxprogramID);
	glDeleteProgram(billboardprogramID);
}


int main() {
	if (!initializeGL())
		return -1;
	LoadHeight();
	LoadModel();
	LoadTextures();
	setSkybox();
	LoadSkybox();

	programID = glCreateProgram();
	LoadShadersT(programID, "Basic.vert", "Texture.frag", "terrain.tesc", "terrain.tese");

	billboardprogramID = glCreateProgram();
	LoadShadersB(billboardprogramID, "billboard.vert", "billboard.frag","billboard.geom");

	skyboxprogramID = glCreateProgram();
	LoadShadersS(skyboxprogramID, "skybox.vert", "skybox.frag");

	glClearColor(0.7f, 0.8f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glm::vec3 LightPosition = glm::vec3(-1.0, 0, 0.5);
	float scaleNum = 0.0f;
	float offsetNum = 1.0f / n_points;

	do {
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			LoadShadersT(programID, "Basic.vert", "Texture.frag", "terrain.tesc", "terrain.tese");
			LoadShadersS(skyboxprogramID, "skybox.vert", "skybox.frag");
			LoadShadersB(billboardprogramID, "billboard.vert", "billboard.frag", "billboard.geom");
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glm::vec3 camerapos = getCameraPosition();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			LightPosition = LightPosition + vec3(0, 0, 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			LightPosition = LightPosition + vec3(0.1, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			LightPosition = LightPosition - vec3(0, 0, 0.1);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			LightPosition = LightPosition - vec3(0.1, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			LightPosition = LightPosition + vec3(0, 0.1, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			LightPosition = LightPosition - vec3(0, 0.1, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
			scaleNum = scaleNum - 0.01;
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			scaleNum = scaleNum + 0.01;
		}


		//First pass: Base mesh
		//shader : ------------------------------------------------terrain---------------------------------------------
		glUseProgram(programID);
		glBindVertexArray(VertexArrayID);
		// Get a handle for our uniforms
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		GLuint LighPosID = glGetUniformLocation(programID, "lightPos");
		glUniform3fv(LighPosID, 1, &LightPosition[0]);
		GLuint cameraPosID = glGetUniformLocation(programID, "cameraPos");
		glUniform3fv(cameraPosID, 1, &camerapos[0]);
		GLuint scaleID = glGetUniformLocation(programID, "scale");
		glUniform1f(scaleID, scaleNum);
		GLuint offsetID = glGetUniformLocation(programID, "offset");
		glUniform1f(offsetID, offsetNum);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, heightTextureID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadingmapID);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, grassID);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, snowID);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, normalmapID);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, grassnormalID);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, grassrID);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, snownID);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, snowrID);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(
			GL_PATCHES, // mode
			(GLsizei)nIndices, // count
			GL_UNSIGNED_INT, // type
			(void*)0 // element array buffer offset
		);
		glBindVertexArray(0);
		glUseProgram(0);


		//shader : -------------------------------------------------billboard---------------------------------------------
		glUseProgram(billboardprogramID);
		glBindVertexArray(VertexArrayID);

		GLuint bMatrixID = glGetUniformLocation(billboardprogramID, "MVP");
		glUniformMatrix4fv(bMatrixID, 1, GL_FALSE, &MVP[0][0]);
		GLuint bscaleID = glGetUniformLocation(billboardprogramID, "scale");
		glUniform1f(bscaleID, scaleNum);
		GLuint bcameraPosID = glGetUniformLocation(programID, "cameraPos");
		glUniform3fv(bcameraPosID, 1, &camerapos[0]);
		glActiveTexture(GL_TEXTURE11);
		glBindTexture(GL_TEXTURE_2D, snowID);
		glActiveTexture(GL_TEXTURE12);
		glBindTexture(GL_TEXTURE_2D, heightTextureID);
		glDrawArrays(GL_POINTS, 0, nIndices);
		glBindVertexArray(0);
		glUseProgram(0);


		//shader : ---------------------------------------------------skybox----------------------------------------------
		glDepthFunc(GL_LEQUAL);
		glUseProgram(skyboxprogramID);
		glBindVertexArray(skyboxVAO);
		glm::mat4 sprojection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		glm::mat4 sViewMatrix = getViewMatrix();
		sViewMatrix[3][0] = 0;
		sViewMatrix[3][1] = 0;
		sViewMatrix[3][2] = 0;
		GLuint sModelMatrixID = glGetUniformLocation(programID, "M");
		glUniformMatrix4fv(sModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		GLuint SViewMatrixID = glGetUniformLocation(skyboxprogramID, "V");
		glUniformMatrix4fv(SViewMatrixID, 1, GL_FALSE, &sViewMatrix[0][0]);
		GLuint SProjectMatrixID = glGetUniformLocation(skyboxprogramID, "P");
		glUniformMatrix4fv(SProjectMatrixID, 1, GL_FALSE, &sprojection[0][0]);
		// Activate the cubemap texture
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, stextureID);
		// Disable depth writing to draw the skybox behind everything else
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		// Re-enable depth writing
		glDepthFunc(GL_LESS);
		glBindVertexArray(0);
		glUseProgram(0);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	UnloadModel();
	UnloadShaders();
	UnloadTextures();

	glfwTerminate();
	return 0;
}
