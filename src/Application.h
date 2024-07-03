#pragma once
class Application {
  public:
    /**
     * Singleton accessor
     */
    static Application& getInstance() {
      if (instance == nullptr) {
        instance = new Application();
      }
      return *instance;
    }

    void setup();
    /**
     * Called from main.cpp loop()
     */
    void loop();

  private:
    Application() {}
    static Application *instance;
};
