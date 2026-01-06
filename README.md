# 2D Physics Engine

A custom 2D rigid body physics engine written in C++ for learning and experimentation. This engine simulates realistic physics interactions including collision detection, impulse-based resolution, friction, restitution, and rotational dynamics.

## Design Goals

- Educational, readable implementation over absolute performance.
- Deterministic and stable behavior for common 2D physics scenarios.
- Clear separation between collision detection and resolution.
- Explicit validation via targeted test scenes.
<br><br>

**Status:** Feature-complete v1 (stable, tested, learning-focused)

## Features

- **Rigid Body Dynamics**: Semi-implicit Euler integration for stable physics simulation
- **Collision Detection**: Circle-to-Circle, Circle-to-Box, and AABB (Axis-Aligned Bounding Box) collision algorithms
- **Impulse-Based Collision Resolution**: Impulse-based collision resolution with iterative solving for multiple contacts per frame.
- **Friction Simulation**: Both static and dynamic friction with proper impulse application
- **Restitution (Elasticity)**: Configurable coefficient of restitution for bouncy or inelastic collisions
- **Rotational Dynamics**: Rotational dynamics for circular rigid bodies, including angular velocity, torque, and moment of inertia.
- **Penetration Correction**: Baumgarte stabilization to prevent objects from sinking
- **SFML Rendering**: Real-time visualization using SFML graphics library

## Physics Fundamentals

### Rigid Body Representation

Each rigid body is represented by:
- **Position** ($\vec{p}$): The center position in world space
- **Velocity** ($\vec{v}$): Linear velocity
- **Mass** ($m$): The mass of the object
- **Rotation** ($\theta$): Angular orientation in radians
- **Angular Velocity** ($\omega$): Rotational speed in rad/s
- **Inertia** ($I$): Moment of inertia (resistance to rotation)

### Core Physics Equations

#### 1. **Kinematics Integration** (Semi-Implicit Euler)

The engine uses semi-implicit Euler integration for stable numerical simulation:

$$\vec{a} = \frac{\vec{F}}{m}$$

$$\vec{v}_{new} = \vec{v}_{old} + \vec{a} \cdot \Delta t$$

$$\vec{p}_{new} = \vec{p}_{old} + \vec{v}_{new} \cdot \Delta t$$

$$\omega_{new} = \omega_{old} + \alpha \cdot \Delta t$$

$$\theta_{new} = \theta_{old} + \omega_{new} \cdot \Delta t$$

Where:
- $\vec{F}$ is the net force applied to the body
- $m$ is the mass
- $\Delta t$ is the timestep
- $\alpha$ is angular acceleration

#### 2. **Moment of Inertia**

For different shapes:

**Circle:**
$$I = \frac{1}{2}mr^2$$

**Box (Rectangle):**
$$I = \frac{1}{12}m(w^2 + h^2)$$
Where $r$ is radius, $w$ is width, and $h$ is height.

*Note: Box moment of inertia is documented for completeness but rotational dynamics are currently implemented only for circles.*

#### 3. **Collision Detection**

**Circle vs Circle:**
$$\text{colliding} = |\vec{p}_B - \vec{p}_A| \leq r_A + r_B$$

**Circle vs Box (AABB):**
Find the closest point on the box to the circle center, then check if distance ≤ radius.

**AABB vs AABB:**
$$\text{colliding} = (right_A \geq left_B) \land (left_A \leq right_B) \land (bottom_A \geq top_B) \land (top_A \leq bottom_B)$$

#### 4. **Impulse-Based Collision Resolution**

When objects collide, the engine computes an impulse to apply to both bodies.

**Relative Velocity at Contact:**
$$\vec{v}_{rel} = \vec{v}_B + \vec{\omega}_B \times \vec{r}_B - (\vec{v}_A + \vec{\omega}_A \times \vec{r}_A)$$

**Velocity Along Normal:**
$$v_n = \vec{v}_{rel} \cdot \hat{n}$$

Where $\hat{n}$ is the collision normal and $\vec{r}$ is the contact vector from center of mass.

**Normal Impulse Magnitude:**
$$j = -\frac{(1 + e) v_n}{m_A^{-1} + m_B^{-1} + (I_A^{-1}(\vec{r}_A \times \hat{n})^2 + I_B^{-1}(\vec{r}_B \times \hat{n})^2)}$$

Where $e$ is the coefficient of restitution.

**Impulse Application:**
$$\vec{J} = j \cdot \hat{n}$$

$$\vec{v}_A -= \vec{J} \cdot m_A^{-1}$$

$$\vec{v}_B += \vec{J} \cdot m_B^{-1}$$

$$\omega_A -= I_A^{-1} (\vec{r}_A \times \vec{J})$$

$$\omega_B += I_B^{-1} (\vec{r}_B \times \vec{J})$$

#### 5. **Friction**

Friction opposes relative motion along the contact tangent.

**Tangent Direction:**
$$\hat{t} = \vec{v}_{rel} - v_n \cdot \hat{n}$$

**Friction Impulse:**
- If static: $j_t = -\vec{v}_{rel} \cdot \hat{t}$ (clamped)
- If dynamic: $j_t = -|j| \cdot \mu_d$

Where:
- $\mu_s$ = coefficient of static friction
- $\mu_d$ = coefficient of dynamic friction

**Combined Friction Coefficient:**
$$\mu = \sqrt{\mu_1^2 + \mu_2^2}$$

#### 6. **Penetration Correction** (Baumgarte Stabilization)

To prevent objects from sinking into each other:

$$\text{correction} = \frac{\max(penetration - \text{slop}, 0) \cdot \beta}{m_A^{-1} + m_B^{-1}}$$

Where $\beta$ (typically 0.8) is the Baumgarte factor and slop is a small threshold.

## Project Structure

```
├── include/
│   ├── physics/          # Physics engine headers
│   │   ├── rigidBody.h
│   │   ├── physicsWorld.h
│   │   ├── colliders.h
│   │   └── collisions.h
│   ├── math/             # Math utilities
│   │   ├── Vec2.h
│   │   └── math_utils.h
│   ├── game/             # Game objects
│   │   └── objects.h
│   ├── render/           # Rendering
│   │   └── render.h
│   └── test/             # Test headers
│       └── tests.h
├── src/
│   ├── physics/          # Physics implementation
│   ├── math/             # Math implementation
│   ├── game/             # Game object implementation
│   ├── render_sfml/      # SFML rendering
│   ├── test/             # Test implementations
│   └── main.cpp          # Entry point
├── CMakeLists.txt
└── README.md
```

## Building

### Prerequisites
- C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.16+
- SFML 3

### Compilation

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running Tests

The engine includes comprehensive test scenarios to validate physics behavior:

### Available Tests

All tests can be run by uncommenting the desired test call in [src/main.cpp](src/main.cpp) and rebuilding.

#### 1. **Restitution Test** (`restitutionTest()`)

**Purpose**: Demonstrates the coefficient of restitution and how it affects bouncing behavior.

**Setup**: Three balls dropped from the same height with different restitution coefficients:
- **Left Ball**: $e = 0.0$ (perfectly inelastic - no bounce)
- **Middle Ball**: $e = 0.5$ (partially elastic)
- **Right Ball**: $e = 1.0$ (perfectly elastic - bounces indefinitely)

**Physics Observed**:
- Balls with higher restitution bounce higher
- Energy loss is proportional to $(1-e)^2$ per bounce
- Demonstrates the formula: $v_{final} = -e \cdot v_{initial}$ at collision

---

#### 2. **Rolling Test** (`rollingTest()`)

**Purpose**: Validates rotational dynamics and friction-induced rolling motion.

**Setup**: A ball with initial horizontal velocity rolls across a floor due to friction.

**Physics Observed**:
- Angular velocity is induced by friction: $\tau = \vec{r} \times \vec{f}$
- Angular acceleration: $\alpha = \tau \cdot I^{-1}$
- Eventually reaches rolling without slipping: $v = \omega \cdot r$
- Demonstrates torque application from friction forces

**Key Formulas**:
$$\vec{\tau} = \vec{r} \times \vec{F}_{friction}$$
$$\alpha = \tau \cdot I^{-1} = \frac{\tau}{I}$$

---

#### 3. **Penetration Test** (`penetrationTest()`)

**Purpose**: Tests the penetration correction and collision resolution system.

**Setup**: A diagonal stack of boxes with overlapping starting positions that must be separated.

**Physics Observed**:
- Baumgarte stabilization separates overlapping objects
- Boxes naturally settle and resolve penetration
- Demonstrates: $correction = \max(penetration - slop, 0) \cdot \beta / (m_A^{-1} + m_B^{-1})$
- Shows smooth, non-explosive separation

---

#### 4. **Stacking Test** (`stackingTest()`)

**Purpose**: Validates stable stacking and resting contact between objects.

**Setup**: Multiple boxes stacked on top of each other with platform support.

**Physics Observed**:
- Objects remain stable when stacked
- No unwanted bouncing or jittering
- Friction keeps boxes from sliding
- Demonstrates numerical stability of the integration scheme
- Shows proper handling of continuous contact

**Key Features**:
- Static friction prevents sliding: $f_s \leq \mu_s \cdot N$
- Normal impulse resolution: $j = -\frac{(1+e)v_n}{total\_inv\_mass}$

---

#### 5. **Multi-Contact Test** (`multiContactTest()`)

**Purpose**: Tests complex collision handling when an object contacts multiple bodies simultaneously.

**Setup**: A large fast-moving ball collides with a grid of smaller boxes arranged in a 2×5 pattern.

**Physics Observed**:
- Ball impacts multiple boxes simultaneously
- Each contact is resolved independently
- Demonstrates proper handling of multiple contact points in a single frame
- Shows how the iterative solver (4 iterations) converges to realistic results
- Large ball imparts momentum to the grid of boxes

**Implementation Details**:
- Contact point calculation: $\vec{c} = \vec{p}_A + \hat{n} \cdot r_A$
- Multi-iteration impulse solving for stability
- Proper combination of friction from multiple contacts

---

#### 6. **Falling Objects Demo** (`fallingObjectsDemo()`)

**Purpose**: Interactive demo where users can create objects dynamically.

**Controls**:
- **Left Mouse Click**: Drop a rectangular block at cursor position
- **Right Mouse Click**: Drop a circular ball at cursor position

**Features**:
- Real-time object creation
- Objects fall under gravity (800 m/s²)

    *Note: Gravity magnitude is intentionally exaggerated for visual clarity.*

- Collision with static obstacles and screen boundaries
- Demonstrates all physics systems working together

---

## Running Individual Tests

To run a specific test, modify [src/main.cpp](src/main.cpp):

```cpp
int main() {
    restitutionTest();    // Change to desired test
    return 0;
}
```

Available test functions:
- `penetrationTest()`
- `restitutionTest()`
- `rollingTest()`
- `stackingTest()`
- `multiContactTest()`
- `fallingObjectsDemo()`

## Key Implementation Details

### Collision Resolution Pipeline

1. **Broad Phase**: Collect all potential collisions (currently O(n²))
2. **Narrow Phase**: Precise collision detection based on shape types
3. **Resolution**: Apply impulses and correct penetration
4. **Iteration**: Repeat 4 times per frame for convergence

### Contact Points

For accurate physics:
- **Circle-Circle**: Contact point is midpoint between centers along normal
- **Circle-Box**: Contact point is closest point on box to circle center
- **Box-Box**: Contacts are assumed at penetration point

### Friction Model

The engine uses a **Coulomb friction model**:
- Combines static and dynamic friction
- Static friction prevents motion up to $\mu_s \cdot N$
- Dynamic friction applies constant magnitude $\mu_d \cdot N$
- Blended based on relative motion threshold (0.3 m/s)

## Performance Considerations

- **Time Complexity**: O(n²) for collision detection (broad phase)
- **Iteration Count**: 4 times per frame for impulse convergence

## Future Improvements

- Spatial partitioning (quadtree/BVH) for faster collision detection
- Constraint solver for joints and ragdolls
- Continuous collision detection for fast-moving objects
- Box rotation and oriented bounding boxes (OBB)
- Soft-body physics
- Particle systems

## License

MIT License - See [LICENSE](LICENSE) file for details.

## Author

Vansh Dangi - 2025
