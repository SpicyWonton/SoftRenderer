#include "window.h"
#include <vector>

Window::Window(int width, int height)
	: width_(width),
	  height_(height),
	  curr_time_(.0),
	  prev_time_(.0),
	  delta_time_(.0),
	  frame_(0),
	  mode_(RenderMode::kFull) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(width, height, 0, &window_, &renderer_);
  texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
  // SDL_SetRelativeMouseMode(SDL_TRUE);
  project_matrix_.SetPerspective(60.0, static_cast<double>(width) / height, 1.0, 30.0);
  pipeline_ = new Pipeline(width, height);
  scene_ = new Scene();
  LoadScene();
  pipeline_->RenderShadowMap();
}

Window::~Window() {
  SDL_DestroyTexture(texture_);
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  if (pipeline_)
    delete pipeline_;
  pipeline_ = nullptr;
  if (scene_)
    delete scene_;
  scene_ = nullptr;
  SDL_Quit();
}

void Window::Show() {
  while (true) {
	EventResponse(scene_->camera());

	scene_->camera()->UpdateView();
	pipeline_->SetCamera(scene_->camera());

	pipeline_->ClearBuffer(Vector4d(0, 0, 0, 1.0));
	pipeline_->Draw(mode_);
	pipeline_->SwapBuffer();

	unsigned char *color_buffer = pipeline_->ColorBuffer();

    SDL_UpdateTexture(texture_, NULL, color_buffer, width_ * 4);
	SDL_RenderCopy(renderer_, texture_, NULL, NULL);
	SDL_RenderPresent(renderer_);

	ShowFPS();
  }
}

void Window::EventResponse(Camera *camera) {
  if (SDL_PollEvent(&event_)) {
	switch (event_.type) {
	  case SDL_QUIT: {
	    UnLoadScene();
		exit(0);
	  }
	  case SDL_KEYDOWN: {
		if (event_.key.keysym.sym == SDLK_w) {
		  scene_->camera()->MoveForward(delta_time_);
		} else if (event_.key.keysym.sym == SDLK_s) {
		  scene_->camera()->MoveBack(delta_time_);
		} else if (event_.key.keysym.sym == SDLK_a) {
		  scene_->camera()->MoveLeft(delta_time_);
		} else if (event_.key.keysym.sym == SDLK_d) {
		  scene_->camera()->MoveRight(delta_time_);
		} else if (event_.key.keysym.sym == SDLK_p) {
		  if (mode_ == RenderMode::kPBR)
			break;
		  else
			mode_ = RenderMode::kPBR;
		  SwitchRenderMode();
		} else if (event_.key.keysym.sym == SDLK_f) {
		  if (mode_ == RenderMode::kFull)
			break;
		  else
		    mode_ = RenderMode::kFull;
		  SwitchRenderMode();
		} else if (event_.key.keysym.sym == SDLK_l) {
		  if (mode_ == RenderMode::kLine)
			break;
		  else
			mode_ = RenderMode::kLine;
		  SwitchRenderMode();
		} else if (event_.key.keysym.sym == SDLK_ESCAPE) {
		  UnLoadScene();
		  exit(0);
		}
		break;
	  }
	  case SDL_MOUSEMOTION: {
		scene_->camera()->Rotate(event_.motion.xrel, event_.motion.yrel);
		break;
	  }
	}
  }
}

void Window::ShowFPS() {
  curr_time_ = SDL_GetTicks();
  delta_time_ = curr_time_ - prev_time_;
  prev_time_ = curr_time_;
  if (frame_ % 10 == 0) {
	sprintf_s(title_, " FPS: %d", static_cast<int>(1000.0 / delta_time_));
	SDL_SetWindowTitle(window_, title_);
  }
  frame_++;
}

void Window::LoadScene() {
  scene_->LoadScene();
  // set camera
  SetCamera();
  // set lights
  SetLights();
  // set meshes
  SetMeshes();
  // set skybox
  pipeline_->SetSkybox(scene_->skybox());
  // set project matrix
  pipeline_->SetProjectMatrix(&project_matrix_);
}

void Window::UpdateScene() {
  scene_->UpdateScene(frame_);
}

void Window::UnLoadScene() {
  scene_->UnLoadScene();
}

void Window::SetCamera() {
  pipeline_->SetCamera(scene_->camera());
}

void Window::SetLights() {
  std::vector<Light*> &lights = scene_->lights();
  for (int i = 0; i < lights.size(); i++) {
	lights[i]->shadow_buffer(width_, height_);
	lights[i]->shadow_buffer()->ClearBuffer();
	pipeline_->AddLight(lights[i]);
  }
}

void Window::SetMeshes() {
  std::vector<Mesh*> &meshes = scene_->meshes();
  for (int i = 0; i < meshes.size(); i++)
	pipeline_->AddMesh(meshes[i]);
}

void Window::SwitchRenderMode() {
  pipeline_->SwitchMode(mode_);
  SetCamera();
  std::vector<Light*> &lights = scene_->lights();
  for (int i = 0; i < lights.size(); i++) {
	pipeline_->shader()->AddLight(lights[i]);
  }
}
