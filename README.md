Justin Herring
828006789
jherring29@tamu.edu

# Final Project - Spiderman Game

This is the rough plan for the project.

### tasks
* player
* ground

- [x] player can move in either direction indefinitely and camera tracks them

* background buildings to swing from
* foreground buildings
    * can walk on top of
    * maybe climb up?

* Load and save a specific level or generate randomly?

* physics objects to interact with
    * push off buildings?
    * pull towards you?

* building sprites
* object sprites
* character sprite/animation

### architecture
* Particle
    * rip from A5
* Polygon class (for collision detection)
    * void addVertex(Particle p)
    * void addVertex(double x, double y, double z=0.0)
        * assume shape is convex
    * bool collide(Particle p)
    * bool collide(double x, double y, double z=0.0) // for convenience?
    * bool collide(Polygon p) // for v in p collide(v) 
* Ray
    * Eigen version of Ray from 441/raytracer
* Web
    * ray for collision
    * spring for forces

* Player (for now)
    * Player(int width, int height, Texture texture)
    * Polygon bounding box
    * texture
    * x, v, etc.

    * shootRay

* EnvironmentObject
    * collide()
    * bool isBackground()
    * bool shootable() // can be shot with a web 
* PhysicsObject : EO
* Ground : EO // could be cool to pull yourself towards the ground
    * bool collide(...)
    * height(double y)
* BGBuilding : EO
    * bb (for web collision)
    * "floating" particle for where to attach web/spring
    * texture
    * draw()
* FGBuilding : EO
    * bb
    * texture
    * draw()
* Crate : PO

* Billboard // for ground and sky, maybe built in parallax?
    * texture
    * draw()
* Scene
    * ground
    * objects :: vector of EOs
    * ground billboard
    * sky texture
    * draw()
    * step()

P to pause (instead of spacebar)
F in direction of cursor to shoot web

