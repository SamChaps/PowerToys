﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace PeekUI.Helpers
{
    public static class FileExplorerHelper
    {
        [DllImport("user32.dll")]
        internal static extern IntPtr GetForegroundWindow();

        public static IEnumerable<string> GetSelectedItems(IntPtr handle)
        {
            var selectedItems = new List<string>();
            var shell = new Shell32.Shell();
            foreach (SHDocVw.InternetExplorer window in shell.Windows())
            {
                if (window.HWND == (int)handle)
                {
                    Shell32.FolderItems items = ((Shell32.IShellFolderViewDual2)window.Document).SelectedItems();
                    foreach (Shell32.FolderItem item in items)
                    {
                        selectedItems.Add(item.Path);
                    }
                }
            }

            return selectedItems;
        }
    }
}