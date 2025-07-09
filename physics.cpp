#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>


class balltexture
{
public:
    balltexture(float x, float y, int rand) : centerX(x), centerY(y), seed(rand) {
        start = std::chrono::steady_clock::now();
    }
    
    ~balltexture(){
        // No custom cleanup needed
    }

    void drawCircle(SDL_Renderer* renderer, int radius) {
        for (int y = -radius; y <= radius; y++) {
            int width = (int)sqrt(radius*radius - y*y);
            SDL_RenderLine(renderer, centerX - width, centerY + y, centerX + width, centerY + y);
        }
    }

    // Handles gravity for the ball and ensures it stays in the window
    void getCirclePos(float windowHeight, float windowWidth){
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - start).count();
        
        
        centerY += velocityY * elapsed;
        centerX += velocityX * elapsed;

        if (centerX - radius <= 0) {
            centerX = radius;
            velocityX *= -0.8f;
        }
        if (centerX + radius >= windowWidth) {
            centerX = windowWidth - radius;
            velocityX *= -0.8f;
        }

        if((centerY + radius) <= 0.0f){
            centerY = 0.0f + radius;
            velocityY *= -0.8f;
            start = std::chrono::steady_clock::now();

        }

        else if( (centerY+16) <  windowHeight ){
            
            velocityY += gravity * elapsed;

            //centerY += velocityY * elapsed;
            if (velocityY == 0.0f){
                start = std::chrono::steady_clock::now();
            } 
        }

        else if ((centerY + radius) >= windowHeight){
            
            centerY = windowHeight - radius;
            velocityY *= -0.8f;
            start = std::chrono::steady_clock::now();
        }
    }

    // Checks if the ball is colliding with another ball
    bool isCollidingWith(const balltexture& other) const {
        float dx = centerX - other.centerX;
        float dy = centerY - other.centerY;
        float distance = sqrt(dx*dx + dy*dy);
        return distance < (radius + other.radius);
    }
    
    int getSeed(){
        return seed;
    }

    //Handels collisions with other balls
    void resolveCollision(balltexture& other) {
        float dx = other.centerX - centerX;
        float dy = other.centerY - centerY;
        float distance = sqrt(dx*dx + dy*dy);
        float nx, ny;
        float relativeVelocityX, relativeVelocityY;
        float velocityAlongNormal;
        float restitution;
        float impulseX, impulseY;

        if (distance == 0) return;
        
        // Normalize collision vector
        nx = dx / distance;
        ny = dy / distance;
        
        // Separate overlapping balls
        float overlap = (radius + other.radius) - distance;
        if (overlap > 0) {
            float separation = overlap * 0.5f;
            centerX -= nx * separation;
            centerY -= ny * separation;
            other.centerX += nx * separation;
            other.centerY += ny * separation;
        }
        
        // Calculate relative velocity
        relativeVelocityX = other.velocityX - velocityX;
        relativeVelocityY = other.velocityY - velocityY;
        
        // Calculate relative velocity along collision normal
        velocityAlongNormal = relativeVelocityX * nx + relativeVelocityY * ny;
        
        //If velocities are separating
        if (velocityAlongNormal > 0) return;
        
        // Calculate restitution (bounciness)
        restitution = 0.8f;
        
        // Calculate impulse scalar (assuming equal mass)
        float impulseMagnitude = -(1 + restitution) * velocityAlongNormal;
        impulseMagnitude /= 2; // Divide by 2 for equal masses
        //impulseMagnitude *= .60; //I used this for more intresting collisions but isn't realistic        

        // Apply impulse
        impulseX = impulseMagnitude * nx;
        impulseY = impulseMagnitude * ny;
        
        velocityX -= impulseX;
        velocityY -= impulseY;
        other.velocityX += impulseX;
        other.velocityY += impulseY;
    }

private:
    std::chrono::steady_clock::time_point start;
    float centerX;
    float centerY;
    float velocityY = 0.0f;
    float gravity = 9.8f;
    float velocityX = 0.0f;
    float radius = 16.5f;
    int seed;
};


void updatePhysicsAndCollisions(std::vector<balltexture>& balls, float windowHeight, float windowWidth);

int main(){
    
    // Initialize window dimensions
    const float windowWidth = 800.0f;
    const float windowHeight = 600.0f;
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Physics simulator", (int)windowWidth, (int)windowHeight, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    bool running = true;
    SDL_Event event;

    // Initializes balls vector
    std::vector<balltexture> balls;

    // This is the main game loop
    while(running){

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            
            // Handle mouse click and spawns new ball
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
                    std::uniform_int_distribution<> distrib(1, 5);
                    int randomValue = distrib(gen);
                    balls.push_back(balltexture(event.button.x, event.button.y, randomValue));
                }
            }
        }

        // Sets the background to be black and clears it
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);
        
        // Update physics and handle collisions
        updatePhysicsAndCollisions(balls, windowHeight, windowWidth);

        for (size_t i = 0; i < balls.size(); i++) {
            
            // Renders ball with its own colour determined by the seed
            switch (balls[i].getSeed()){
                case 1:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
                    break;
            }
            balls[i].drawCircle(renderer, 16);
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void updatePhysicsAndCollisions(std::vector<balltexture>& balls, float windowHeight, float windowWidth) {
    // First handle gravity for all balls
    for (size_t i = 0; i < balls.size(); ++i) {
        balls[i].getCirclePos(windowHeight, windowWidth);
    }
    
    // Then handle all collisions between balls
    for (size_t i = 0; i < balls.size(); ++i) {
        for (size_t j = i+1; j < balls.size(); ++j) {
            if (balls[i].isCollidingWith(balls[j])) {
                balls[i].resolveCollision(balls[j]);
            }
        }
    }
}
