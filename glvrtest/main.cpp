#include <unistd.h>
#ifdef __arm__
#include <GLES2/gl2.h>
// #include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#endif
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <libserialport.h>
#include <ostream>
#include <thread>
#include "cmdline.h"

#ifdef BLE
#include <simpleble/SimpleBLE.h>


#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e" 
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#endif
std::atomic<bool> needDataFlag{};
std::atomic<bool> running{};
uint8_t read_buf[8];
bool inbox = false;
uint8_t wrk_mode = 0;

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
  if (wrk_mode == 0) {
    struct sp_port *port;
    check(sp_get_port_by_name(port_name, &port));
    check(sp_open(port, SP_MODE_READ_WRITE));
    check(sp_set_baudrate(port, 115200));
    check(sp_set_bits(port, 8));
    check(sp_set_parity(port, SP_PARITY_NONE));
    check(sp_set_stopbits(port, 2));
    check(sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE));
    // check(sp_set_flowcontrol(port, SP_FLOWCONTROL_RTSCTS));
    check(sp_nonblocking_write(port, "S", 1));
    while (running.load()) {
      if (needDataFlag.load()) {

        uint8_t read_buf_tmp[8];
        int num_bytes = check(sp_blocking_read(port, read_buf_tmp, 8, 2));
        if (num_bytes < 0 ||
            !(read_buf_tmp[0] == 0xFE && read_buf_tmp[7] == 0xFF)) {
         // if (read_buf[0] == 0xFE && read_buf[7] == 0xFF) {
        } else if (read_buf_tmp[0] == 0xFE && read_buf_tmp[7] == 0xFF) {
          memset(&read_buf, 0, sizeof(read_buf));
          for (int x = 0; x < 8; x++) {
            read_buf[x] = read_buf_tmp[x];
          }
        }
        for (int x = 0; x < 8; x++) {
            printf("%d ", read_buf_tmp[x]);
        }
        printf("\n");

        needDataFlag.store(false);
      }
    }
    // close(serial_port);
    check(sp_close(port));
    sp_free_port(port);


  } 
  #ifdef BLE
  else if (wrk_mode == 1) {
    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
      std::cout << "Bluetooth is not enabled" << std::endl;
      abort();
    }
    auto adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty()) {
      std::cout << "No Bluetooth adapters found" << std::endl;
      abort();
    }

    SimpleBLE::Adapter adapter = adapters[0];
    adapter.scan_for(5000);

    std::vector<SimpleBLE::Peripheral> peripherals = adapter.scan_get_results();
    SimpleBLE::Peripheral peripheral;
    for (SimpleBLE::Peripheral periph : peripherals) {
      if (periph.address() == port_name){
          peripheral = periph;
          std::cout << "Peripheral identifier: " << peripheral.identifier() << std::endl;
          std::cout << "Peripheral address: " <<  peripheral.address() << std::endl;
      }
    }
    if (!peripheral.initialized()){
      std::cout << "Try resetting the tracker with the reset button and start tracking again..." << std::endl;
      abort();
    }
    
    peripheral.connect();
    peripheral.write_request(SERVICE_UUID, CHARACTERISTIC_UUID_RX, SimpleBLE::ByteArray("S"));
    std::cout << "Connected" << std::endl;

      
    peripheral.notify(SERVICE_UUID, CHARACTERISTIC_UUID_TX,
                      [&](SimpleBLE::ByteArray bytes) { 
                        // if(needDataFlag.load()){
                        memcpy(read_buf, bytes.c_str(), 8);
                        needDataFlag.store(false);
                        // }

    });
        while (running.load()) {

        }
    // peripheral.write_request(uuid.first, uuid.second, SimpleBLE::ByteArray("s"));
    std::cout << "Disconnecting..." << std::endl;
    peripheral.disconnect();
    std::cout << "Disconnected!" << std::endl;
  }
  #endif
  else { 
    std::cout << "Unsupported mode. Try to compile an application with support for this mode." << std::endl;
    exit(2);
  }
}

// void window_size_callback(GLFWwindow* window, int width, int height){
//   gluPerspective(90.f, (GLfloat)width/(GLfloat)height, 1.f, 300.0f);
// }
void exitApp(GLFWwindow* window){
  running.store(false);
}

void renderingThread() {
  GLFWwindow *window;
  if (!glfwInit()) {
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
  glfwWindowHint(GLFW_SAMPLES, 4);
  // glfwSetWindowSizeCallback(window, window_size_callback);
  glClearDepth(1.f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);  
  glDepthMask(GL_TRUE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glfwSetWindowCloseCallback(window, exitApp);
  gluPerspective(90.f, 1.f, 1.f, 300.0f);
  // glEnable(GL_MULTISAMPLE);
  while (!glfwWindowShouldClose(window)) {
    needDataFlag.store(true);
  while (needDataFlag.load()) {
  }
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
  gengetopt_args_info args_info;
  if (cmdline_parser (argc, argv, &args_info) != 0)
    exit(1);
   if (args_info.ble_flag) wrk_mode = 1;

  running.store(true);

  std::thread render(renderingThread);
  std::thread thread(readingThread, args_info.device_arg);
  render.join();
  // thread.join();
  thread.detach();
  return 0;
}
