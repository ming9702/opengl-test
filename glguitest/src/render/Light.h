#pragma once
class Light {
  public:
    Light();
    ~Light();

    void Init();
    void Drow();

  private:
    unsigned int light_vao_ = 0;
    unsigned int light_vbo_ = 0;
};
