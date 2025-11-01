/*
 *  Copyright (C) 2025-2025 Team XBMC
 *  This file is part of XBMC - https://github.com/antonic901/xbmc4xbox-redux
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ContextMenus.h"

#include "FileItem.h"
#include "addons/Addon.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "dialogs/GUIDialogSelect.h"
#include "filesystem/AddonsDirectory.h"
#include "guilib/GUIWindowManager.h"
#include "interfaces/generic/ScriptInvocationManager.h"
#include "programs/dialogs/GUIDialogProgramInfo.h"
#include "utils/URIUtils.h"


namespace CONTEXTMENU
{

CProgramInfoBase::CProgramInfoBase()
  : CStaticContextMenuAction(19033)
{
}

bool CProgramInfoBase::IsVisible(const CFileItem& item) const
{
  return item.IsXBE();
}

bool CProgramInfoBase::Execute(const boost::shared_ptr<CFileItem>& item) const
{
  // CGUIDialogVideoInfo::ShowFor(*item);
  return true;
}

std::string CScriptLaunch::GetLabel(const CFileItem& item) const
{
  return g_localizeStrings.Get(247);
}

bool CScriptLaunch::IsVisible(const CFileItem& item) const
{
  if (item.m_bIsFolder)
    return URIUtils::IsDOSPath(item.GetPath());

  return item.IsDefaultXBE();
}

bool CScriptLaunch::Execute(const boost::shared_ptr<CFileItem>& item) const
{
  ADDON::VECADDONS addons;
  if (XFILE::CAddonsDirectory::GetScriptsAndPlugins("executable", addons) && addons.size())
  {
    CGUIDialogSelect *dialog = static_cast<CGUIDialogSelect*>(g_windowManager.GetWindow(WINDOW_DIALOG_SELECT));
    if (dialog)
    {
      dialog->SetHeading(247);
      dialog->Reset();
      for (ADDON::VECADDONS::const_iterator it = addons.begin(); it != addons.end(); ++it)
      {
        std::string strOption = StringUtils::Format("%s (%s)", (*it)->Name().c_str(), (*it)->Author().c_str());
        dialog->Add(strOption);
      }
      dialog->Open();

      int iSelected = dialog->GetSelectedItem();
      if (!dialog->IsConfirmed() || iSelected < 0)
        return true;

      std::string strPath = item->GetPath();
      std::string strParentPath = item->m_bIsFolder ? item->GetPath() : URIUtils::GetParentPath(strPath);

      std::vector<std::string> argv;
      argv.push_back(strPath);
      argv.push_back(strParentPath);

      ADDON::AddonPtr addon = addons[iSelected];
      CScriptInvocationManager::GetInstance().ExecuteAsync(addon->LibPath(), addon, argv);

      return true;
    }
  }

  CGUIDialogKaiToast::QueueNotification(StringUtils::Format(g_localizeStrings.Get(13328).c_str(), g_localizeStrings.Get(247).c_str()), g_localizeStrings.Get(161));
  return false;
};
}
