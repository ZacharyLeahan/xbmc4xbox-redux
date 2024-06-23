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

CDVDInputStreamFFmpeg::CDVDInputStreamFFmpeg() 
  : CDVDInputStream(DVDSTREAM_TYPE_FFMPEG)
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

bool CDVDInputStreamFFmpeg::Open(const char* strFile, const std::string& content)
{
  CFileItem item(strFile, false);
  std::string selected;
  if (item.IsInternetStream() && (item.IsType(".m3u8") || content == "application/vnd.apple.mpegurl"))
  {
    // get the available bandwidth and  determine the most appropriate stream
    int bandwidth = CSettings::Get().GetInt("network.bandwidth");
    if(bandwidth <= 0)
      bandwidth = INT_MAX;
    selected = PLAYLIST::CPlayListM3U::GetBestBandwidthStream(strFile, bandwidth);
    if (selected.compare(strFile) != 0)
    {
      CLog::Log(LOGINFO, "CDVDInputStreamFFmpeg: Auto-selecting %s based on configured bandwidth.", selected.c_str());
      strFile = selected.c_str();
    }
  }

  if (!CDVDInputStream::Open(strFile, content)) 
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

