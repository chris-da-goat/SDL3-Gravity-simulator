# SDL3-Gravity-simulator

A simple 2D physics simulator written in C++ using SDL3.
The program lets you click to spawn bouncey balls with different colors and simulates realistic gravity, collisions with window boundaries, and ball-to-ball collisions.


##Features
  Real-time physics: Gravity, velocity, and bounce restitution.
  Ball spawning: Left-click anywhere in the window to add a ball.
  Randomized colors: Each ball gets a color based on a random seed.
  Collision detection: Handles both wall collisions and ball-to-ball collisions.
  Elastic bounce: Balls lose a small amount of speed after each collision for a more realistic feel.

##Requirements
C++17 or higher
SDL3 development libraries

##How It Works
balltexture class:
  Handles ball position, gravity effects, collision detection, and drawing.
Main loop:
  Processes events (quit, mouse clicks).
  Updates ball positions based on physics.
  Checks and resolves collisions.
  Renders the scene.
Randomized ball colors:
  A random number generator assigns each new ball one of several predefined colors.
  
##Future Improvements
  Incorporate OpenGl for optimized performance with instance rendering
  Add more functions for the player to control
    ( ex: press space to send all the balls to the center of the screen)
  Allow adjustable gravity and bounce settings.
  Add UI to control simulation parameters.

##License
This project is licensed under the MIT License.

