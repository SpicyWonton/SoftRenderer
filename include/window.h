#ifndef SOFTRENDERER_INCLUDE_WINDOW_H_
#define SOFTRENDERER_INCLUDE_WINDOW_H_

#include "camera.h"
#include "global_config.h"
#include "pipeline.h"
#include "matrix.h"
#include "scene.h"
#include "SDL.h"

class Window {
 public:
  Window(int width, int height);
  ~Window();

  void Show();

 private:
  void EventResponse(Camera *camera);
  void ShowFPS();
  void LoadScene();
  void UpdateScene();
  void UnLoadScene();

  void SetCamera();
  void SetLights();
  void SetMeshes();

  void SwitchRenderMode();

 private:
  char title_[20];
  int width_, height_;
  SDL_Window *window_;
  SDL_Renderer *renderer_;
  SDL_Texture *texture_;
  SDL_Event event_;
  Pipeline *pipeline_;
  Scene *scene_;
  double curr_time_, prev_time_;
  double delta_time_;	// The amount of time that went from the previous frame to this frame
  int frame_;
  RenderMode mode_;
  Matrix4d project_matrix_;
};

#endif //SOFTRENDERER_INCLUDE_WINDOW_H_
