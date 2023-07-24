#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <learnopengl/shader_m.h>
#include <learnopengl/shader_c.h>
#include <learnopengl/camera.h>

#include <iostream>

void renderQuad()
{
  static unsigned int quadVAO = 0;
  static unsigned int quadVBO;

  if (quadVAO == 0)
    {
      float quadVertices[] = {
      	// positions        // texture Coords
      	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void framebuffer_size_callback (GLFWwindow* window, int width, int height)
{
  glViewport (0, 0, width, height);
}

int main (int argc, char* argv[])
{
  const unsigned int SCR_WIDTH = 800;
  const unsigned int SCR_HEIGHT = 600;

  const unsigned int TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;

  glfwInit ();
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* window = glfwCreateWindow (SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
  glfwMakeContextCurrent (window);
  glfwSetFramebufferSizeCallback (window, framebuffer_size_callback);
  glfwSwapInterval (0);
  
  if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
    {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
    }
  
  int max_compute_work_group_count[3];
  int max_compute_work_group_size[3];
  int max_compute_work_group_invocations;
  
  for (int idx = 0; idx < 3; idx++) 
    {
      glGetIntegeri_v (GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
      glGetIntegeri_v (GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
    }	
  glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);
  
  std::cout << "OpenGL Limitations: " << std::endl;
  std::cout << "maximum number of work groups in X dimension " << max_compute_work_group_count[0] << std::endl;
  std::cout << "maximum number of work groups in Y dimension " << max_compute_work_group_count[1] << std::endl;
  std::cout << "maximum number of work groups in Z dimension " << max_compute_work_group_count[2] << std::endl;
  
  std::cout << "maximum size of a work group in X dimension " << max_compute_work_group_size[0] << std::endl;
  std::cout << "maximum size of a work group in Y dimension " << max_compute_work_group_size[1] << std::endl;
  std::cout << "maximum size of a work group in Z dimension " << max_compute_work_group_size[2] << std::endl;
  
  std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << std::endl;
  
  Shader screenQuad ("screenQuad.vs", "screenQuad.fs");
  ComputeShader computeShader ("computeShader.cs");
  ComputeShader computeShader1 ("computeShader1.cs");
  
  screenQuad.use();
  screenQuad.setInt("tex", 0);
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  unsigned int texture;
  
  glGenTextures (1, &texture);
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, texture);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 
                0, GL_RGBA, GL_FLOAT, NULL);
  
  glBindImageTexture (0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  
  computeShader.use ();
  glDispatchCompute ((unsigned int)TEXTURE_WIDTH/32, (unsigned int)TEXTURE_HEIGHT/32, 1);
  glMemoryBarrier (GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      
  while (! glfwWindowShouldClose (window))
    {
      computeShader1.use();
      glDispatchCompute((unsigned int)TEXTURE_WIDTH/32, (unsigned int)TEXTURE_HEIGHT/32, 1);
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      screenQuad.use();
      renderQuad();
      
      glfwSwapBuffers (window);
      glfwPollEvents ();
      if (glfwGetKey (window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        break;
    }
  
  glDeleteTextures(1, &texture);
  glDeleteProgram(screenQuad.ID);
  glDeleteProgram(computeShader.ID);
  
  glfwTerminate();
  
  return EXIT_SUCCESS;
}

