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
 
#include "DVDInputStreamFFmpeg.h"
#include "playlists/PlayListM3U.h"
#include "settings/Settings.h"
#include "Util.h"
#include "utils/log.h"

using namespace XFILE;

CDVDInputStreamFFmpeg::CDVDInputStreamFFmpeg(CFileItem& fileitem)
  : CDVDInputStream(DVDSTREAM_TYPE_FFMPEG, fileitem)
{

}

CDVDInputStreamFFmpeg::~CDVDInputStreamFFmpeg()
{
  Close();  
}

bool CDVDInputStreamFFmpeg::IsEOF()
{
  return false;
}

bool CDVDInputStreamFFmpeg::Open()
{
  std::string selected;
  if (m_item.IsInternetStream() && (m_item.IsType(".m3u8") || m_item.GetMimeType() == "application/vnd.apple.mpegurl"))
  {
    // get the available bandwidth and  determine the most appropriate stream
    int bandwidth = CSettings::Get().GetInt("network.bandwidth");
    if(bandwidth <= 0)
      bandwidth = INT_MAX;
    selected = PLAYLIST::CPlayListM3U::GetBestBandwidthStream(m_item.GetPath(), bandwidth);
    if (selected.compare(m_item.GetPath()) != 0)
    {
      CLog::Log(LOGINFO, "CDVDInputStreamFFmpeg: Auto-selecting %s based on configured bandwidth.", selected.c_str());
      m_item.SetPath(selected.c_str());
    }
  }

  if (!CDVDInputStream::Open())
    return false;

  return true;
}

// close file and reset everyting
void CDVDInputStreamFFmpeg::Close()
{
  CDVDInputStream::Close();
}

int CDVDInputStreamFFmpeg::Read(BYTE* buf, int buf_size)
{
  return -1;
}

int64_t CDVDInputStreamFFmpeg::GetLength()
{
  return 0;
}

int64_t CDVDInputStreamFFmpeg::Seek(int64_t offset, int whence)
{
  return -1;
}

