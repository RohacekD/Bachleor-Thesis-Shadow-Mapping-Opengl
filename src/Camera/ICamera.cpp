#include "Camera/ICamera.h"

#include <algorithm>

//=================================================================================
void I_Camera::Subscribe(std::shared_ptr<I_CameraObserver> observer)
{
	m_observers.push_back(observer);
}

//=================================================================================
void I_Camera::Unsubscribe(std::shared_ptr<I_CameraObserver> observer)
{
	m_observers.erase(
		std::remove_if(
			m_observers.begin(),
			m_observers.end(),
			[&](const std::weak_ptr<I_CameraObserver>& wptr)
			{
				return wptr.expired() || wptr.lock() == observer;
			}
		),
		m_observers.end()
		);

}

//=================================================================================
void I_Camera::Notify(E_CameraValues value)
{
	for (auto observWP : m_observers)
	{
		if (!observWP.expired())
		{
			auto observer = observWP.lock();
			observer->Notify(value);
		}
	}
}
