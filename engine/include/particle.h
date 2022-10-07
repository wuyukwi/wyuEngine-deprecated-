/*********************************************************************************

  *FileName: particle.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/10

**********************************************************************************/

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

// Velocity direction.
#define VEL_DIRX 0.0f
#define VEL_DIRY -0.4f
#define VEL_DIRZ 0.0f

// Change in velocity over time.
#define VEL_DELTAX 0.0f
#define VEL_DELTAY 0.1f
#define VEL_DELTAZ 0.0f

// Max number of new particles.
#define UPDATE_AMT 1000

class CParticle
{
public:
    CParticle()
    {
        m_pos[0] = 0; m_pos[1] = 0; m_pos[2] = 0;
        m_vel[0] = 0; m_vel[1] = 0; m_vel[2] = 0;
    }

    // Particle position and velocity.
    float m_pos[3];
    float m_vel[3];
};

class CRainPS
{
public:
    CRainPS();

    // Init system.
    bool Initialize(float x, float y, float z, int maxP,
        float w, float h, float d);

    // Update all particles based on time.
    void Update(float scalar);

    // Create a new particle(s).
    void CreateParticle(int amount);

    // Release all resources.
    void Shutdown();

    // Width, height, and depth of the area.
    float m_width;
    float m_height;
    float m_depth;

    // System position, list of particles, max and number of
    // particles, time counter.
    float m_pos[3];
    CParticle* m_particles;
    int m_maxParticles;
    int m_particleCount;
    float m_totalTime;
};

#endif // _PARTICLE_H_
