#pragma once

class Angle 
{
public:
	void Add(float Degrees)
	{
		m_angle += Degrees;

		if (Degrees > 0)
		{
			while (m_angle > 180.f)
			{
				m_angle -= 360;
			}
		}
		if (Degrees < 0)
		{
			while (m_angle < -180.f)
			{
				m_angle += 360;
			}		
		}		
	}

	float Get() const { return m_angle; }
private:
	float m_angle;
};


class Rotation
{
	float AddYaw(float Degrees) { m_yaw.Add(Degrees); }
	float AddPitch(float Degrees) { m_pitch.Add(Degrees); }
	float AddRoll(float Degrees) { m_roll.Add(Degrees); }

	float GetYaw() const { return m_yaw.Get(); }
	float GetPitch() const { return m_pitch.Get(); }
	float GetRoll() const { return m_roll.Get(); }

private:
	Angle m_yaw;
	Angle m_pitch;
	Angle m_roll;
};