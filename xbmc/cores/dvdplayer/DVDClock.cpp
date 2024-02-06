/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "system.h"
#include "DVDClock.h"
#include "threads/SingleLock.h"
#include "utils/TimeUtils.h"

#include <math.h>

int64_t CDVDClock::m_systemFrequency;
CCriticalSection CDVDClock::m_systemsection;

CDVDClock::CDVDClock()
{
  CSingleLock lock(m_systemsection);
  CheckSystemClock();

  m_systemUsed = m_systemFrequency;
  m_pauseClock = 0;
  m_bReset = true;
  m_iDisc = 0;
  m_startClock = 0;
}

CDVDClock::~CDVDClock()
{}

// Returns the current absolute clock in units of DVD_TIME_BASE (usually microseconds).
double CDVDClock::GetAbsoluteClock()
{
  CheckSystemClock();

  int64_t current;
  current = CurrentHostCounter();

  return SystemToAbsolute(current);
}

double CDVDClock::GetClock()
{
  CSharedLock lock(m_critSection);
  int64_t current;
  current = CurrentHostCounter();
  return SystemToPlaying(current);
}

double CDVDClock::GetClock(double& absolute)
{
  int64_t current;
  current = CurrentHostCounter();
  {
    CSingleLock lock(m_systemsection);
    CheckSystemClock();
    absolute = SystemToAbsolute(current);
  }

  CSharedLock lock(m_critSection);
  return SystemToPlaying(current);
}

void CDVDClock::SetSpeed(int iSpeed)
{
  // this will sometimes be a little bit of due to rounding errors, ie clock might jump abit when changing speed
  CExclusiveLock lock(m_critSection);

  if(iSpeed == DVD_PLAYSPEED_PAUSE)
  {
    if(!m_pauseClock)
      m_pauseClock = CurrentHostCounter();
    return;
  }
  
  int64_t current;
  int64_t newfreq = m_systemFrequency * DVD_PLAYSPEED_NORMAL / iSpeed;
  
  current = CurrentHostCounter();
  if( m_pauseClock )
  {
    m_startClock += current - m_pauseClock;
    m_pauseClock = 0;
  }

  m_startClock = current - (int64_t)((double)(current - m_startClock) * newfreq / m_systemUsed);
  m_systemUsed = newfreq;
}

void CDVDClock::Discontinuity(double currentPts)
{
  CExclusiveLock lock(m_critSection);
  m_startClock = CurrentHostCounter();
  if(m_pauseClock)
    m_pauseClock = m_startClock;
  m_iDisc = currentPts;
  m_bReset = false;
}

void CDVDClock::Pause()
{
  CExclusiveLock lock(m_critSection);
  if(!m_pauseClock)
    m_pauseClock = CurrentHostCounter();
}

void CDVDClock::Resume()
{
  CExclusiveLock lock(m_critSection);
  if( m_pauseClock )
  {
    int64_t current;
    current = CurrentHostCounter();

    m_startClock += current - m_pauseClock;
    m_pauseClock = 0;
  }  
}

void CDVDClock::CheckSystemClock()
{
  if(!m_systemFrequency)
    m_systemFrequency = CurrentHostFrequency();
}

double CDVDClock::SystemToAbsolute(int64_t system)
{
  return DVD_TIME_BASE * (double)system / m_systemFrequency;
}

double CDVDClock::SystemToPlaying(int64_t system)
{
  int64_t current;

  if (m_bReset)
  {
    m_startClock = system;
    m_systemUsed = m_systemFrequency;
    m_pauseClock = 0;
    m_iDisc = 0;
    m_bReset = false;
  }
  
  if (m_pauseClock)
    current = m_pauseClock;
  else
    current = system;

  return DVD_TIME_BASE * (double)(current - m_startClock) / m_systemUsed + m_iDisc;
}

