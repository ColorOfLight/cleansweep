/*
 * MIT License
 *
 * Copyright (c) 2024 Seongho Park
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <GLES3/gl3.h>
#include <emscripten.h>

#include <functional>

#include "./Component/ClientEventComponent.h"
#include "./Component/ClientInputComponent.h"
#include "./Entity/GameEntity.h"
#include "./Entity/PlayerEntity.h"
#include "./Entity/WashableEntity.h"
#include "./System/ClientSyncSystem.h"

static std::function<void(float, float)> static_main_loop;
static double start_time = emscripten_get_now();
static double prev_time = start_time;

void renderFrame() {
  // TODO: activate after init GL context
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  double current_time = emscripten_get_now();
  float elapsed_time = static_cast<float>(current_time - start_time);
  float delta_time = static_cast<float>(current_time - prev_time);

  if (static_main_loop) {
    static_main_loop(elapsed_time, delta_time);
  }

  prev_time = current_time;
}

int main() {
  auto client_input_component = std::make_unique<ClientInputComponent>();
  auto client_event_component = std::make_unique<ClientEventComponent>();
  auto game_entity = std::make_unique<GameEntity>();
  auto player_entity = std::make_unique<PlayerEntity>();
  auto washable_entity = std::make_unique<WashableEntity>();

  auto main_loop = [client_input_component = std::ref(*client_input_component),
                    client_event_component = std::ref(*client_event_component),
                    game_entity = std::ref(*game_entity)](float elapsed_time,
                                                          float delta_time) {
    // main loop
    ClientSyncSystem::syncInput(client_input_component,
                                std::ref(*game_entity.get().input_component));
  };

  static_main_loop = main_loop;

  emscripten_set_main_loop(renderFrame, 0, 1);

  return 0;
}
