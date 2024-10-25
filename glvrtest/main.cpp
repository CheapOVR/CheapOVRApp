// #include <GLES2/gl2.h>
// #include <GLES2/gl2ext.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <libserialport.h>
#include <mutex>
#include <atomic>
std::atomic<bool> needDataFlag{};
uint8_t read_buf[8];
bool inbox = false;


int check(enum sp_return result) {
  char *error_message;
  switch (result) {
  case SP_ERR_ARG:
    printf("Error: Invalid argument.\n");
    abort();
  case SP_ERR_FAIL:
    error_message = sp_last_error_message();
    printf("Error: Failed: %s\n", error_message);
    sp_free_error_message(error_message);
    abort();
  case SP_ERR_SUPP:
    printf("Error: Not supported.\n");
    abort();
  case SP_ERR_MEM:
    printf("Error: Couldn't allocate memory.\n");
    abort();
  case SP_OK:
  default:
    return result;
  }
}

void readingThread(char *port_name) {

  struct sp_port *port;
  check(sp_get_port_by_name(port_name, &port));
  check(sp_open(port, SP_MODE_READ_WRITE));
  check(sp_set_baudrate(port, 115200));
  check(sp_set_bits(port, 8));
  check(sp_set_parity(port, SP_PARITY_NONE));
  check(sp_set_stopbits(port, 1));
  check(sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE));
  check(sp_nonblocking_write(port, "S", 1));
  while (true) {
    if (needDataFlag.load()) {

      uint8_t read_buf_tmp[8];
      int num_bytes = check(sp_nonblocking_read(port, read_buf_tmp, 8));
      if (num_bytes < 0 ||
          !(read_buf_tmp[0] == 0xFE && read_buf_tmp[7] == 0xFF)) {

      } else if (read_buf_tmp[0] == 0xFE && read_buf_tmp[7] == 0xFF) {
        memset(&read_buf, '\0', sizeof(read_buf));
        for (int x = 0; x < 8; x++) {
          read_buf[x] = read_buf_tmp[x];
        }
      }

      needDataFlag.store(false);
    }
  }
  // close(serial_port);
  check(sp_close(port));
  sp_free_port(port);

}


// void window_size_callback(GLFWwindow* window, int width, int height){
//   gluPerspective(90.f, (GLfloat)width/(GLfloat)height, 1.f, 300.0f);
// }

void renderingThread() {
  GLFWwindow *window;
  if (!glfwInit()){
    abort();
  }
  window = glfwCreateWindow(600, 600, "CheapOVR Test Utility", NULL, NULL);
  if (!window) {
    printf("ERR");
    glfwTerminate();
    abort();
  }
  glfwSetWindowSizeLimits(window, 600, 600, 600, 600);
  glfwMakeContextCurrent(window);
  // glfwSetWindowSizeCallback(window, window_size_callback);
  glClearDepth(1.f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.f);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(90.f, 1.f, 1.f, 300.0f);
  // glEnable(GL_MULTISAMPLE);
  while (!glfwWindowShouldClose(window)) {

    needDataFlag.store(true);
    while (needDataFlag.load()) {
    }
    // if (read_buf[0] == 0xFE && read_buf[7] == 0xFF) {
    //   for (int x = 0; x < 8; x++) {
    //     printf("%d ", read_buf[x]);
    //   }
    //   printf("\n");
    // }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //
    if (inbox) {
      // glTranslatef(0.f, 0.f, -200.f);
      //       float x, y, z;

      //       sphericalToCartesian(1.f, (float)-(read_buf[2] == 1 ?
      //       -read_buf[1] + 360 : read_buf[1]), (float)-(read_buf[4] == 1 ?
      //       -read_buf[3] + 360 : read_buf[3]), x, y, z);
      // // glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
      // //   		   glm::vec3(x, y, z),
      // //   		   glm::vec3(0.0f, 1.0f, 0.0f));
      //       gluLookAt(0, 0, 0, x, y, z, 0, 1, 0);

      // gluLookAt(0, 0, 0, x, y, z, 0, 0, 0);

      glRotatef((float)(read_buf[2] == 1 ? -read_buf[1] + 360 : read_buf[1]),
                0.f, 1.f, 0.f);
      glRotatef((float)-(read_buf[4] == 1 ? -read_buf[3] + 360 : read_buf[3]),
                1.f, 0.f, 0.f);
      glRotatef((float)(read_buf[6] == 1 ? -read_buf[5] + 360 : read_buf[5]),
                0.f, 0.f, 1.f);

    } else {
      glTranslatef(0.f, 0.f, -200.f);
      glRotatef((float)-(read_buf[2] == 1 ? -read_buf[1] + 360 : read_buf[1]),
                0.f, 1.f, 0.f);
      glRotatef((float)(read_buf[4] == 1 ? -read_buf[3] + 360 : read_buf[3]),
                1.f, 0.f, 0.f);
      glRotatef((float)-(read_buf[6] == 1 ? -read_buf[5] + 360 : read_buf[5]),
                0.f, 0.f, 1.f);
    }

    glBegin(GL_QUADS);

    glColor3f(0, 1.f, 1.f);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f(-50.f, 50.f, -50.f);
    glVertex3f(50.f, 50.f, -50.f);
    glVertex3f(50.f, -50.f, -50.f);

    glColor3f(0, 0, 1);
    glVertex3f(-50.f, -50.f, 50.f);
    glVertex3f(-50.f, 50.f, 50.f);
    glVertex3f(50.f, 50.f, 50.f);
    glVertex3f(50.f, -50.f, 50.f);

    glColor3f(1, 0, 1);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f(-50.f, 50.f, -50.f);
    glVertex3f(-50.f, 50.f, 50.f);
    glVertex3f(-50.f, -50.f, 50.f);

    glColor3f(0, 1, 0);
    glVertex3f(50.f, -50.f, -50.f);
    glVertex3f(50.f, 50.f, -50.f);
    glVertex3f(50.f, 50.f, 50.f);
    glVertex3f(50.f, -50.f, 50.f);

    glColor3f(1, 1, 0);
    glVertex3f(-50.f, -50.f, 50.f);
    glVertex3f(-50.f, -50.f, -50.f);
    glVertex3f(50.f, -50.f, -50.f);
    glVertex3f(50.f, -50.f, 50.f);

    glColor3f(1, 0, 0);
    glVertex3f(-50.f, 50.f, 50.f);
    glVertex3f(-50.f, 50.f, -50.f);
    glVertex3f(50.f, 50.f, -50.f);
    glVertex3f(50.f, 50.f, 50.f);

    glEnd();

    glfwSwapBuffers(window);

    glfwPollEvents();
  }
  glfwTerminate();
}

int main(int argc, char *argv[]) {

  if (argc == 1) {
    std::cout << "Required one or more arguments" << std::endl;
    return 1;
  }


  if (argc >= 3) {
    int num = std::atoi(argv[2]);
    if (num == 1) {
      inbox = true;
    } else {
      inbox = false;
    }
  } else {
    inbox = false;
  }

  
  std::thread render(renderingThread);
  std::thread thread(readingThread, argv[1]);
  render.join();
  // thread.join();
  thread.detach();
  return 0;
}
