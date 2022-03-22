﻿// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System;
using System.Runtime.CompilerServices;
using Microsoft.PowerToys.Settings.UI.Library.Helpers;
using Microsoft.PowerToys.Settings.UI.Library.Interfaces;

namespace Microsoft.PowerToys.Settings.UI.Library.ViewModels
{
    public class PeekViewModel : Observable
    {
        private GeneralSettings GeneralSettingsConfig { get; set; }

        private PeekSettings Settings { get; set; }

        private Func<string, int> SendConfigMSG { get; }

        public PeekViewModel(ISettingsRepository<GeneralSettings> settingsRepository, ISettingsRepository<PeekSettings> moduleSettingsRepository, Func<string, int> ipcMSGCallBackFunc)
        {
            // To obtain the general settings configurations of PowerToys Settings.
            if (settingsRepository == null)
            {
                throw new ArgumentNullException(nameof(settingsRepository));
            }

            GeneralSettingsConfig = settingsRepository.SettingsConfig;

            SendConfigMSG = ipcMSGCallBackFunc;

            Settings = moduleSettingsRepository.SettingsConfig;
        }

        public bool IsEnabled
        {
            get => _isEnabled;
            set
            {
                if (_isEnabled != value)
                {
                    _isEnabled = value;

                    GeneralSettingsConfig.Enabled.Awake = value;
                    OnPropertyChanged(nameof(IsEnabled));

                    OutGoingGeneralSettings outgoing = new OutGoingGeneralSettings(GeneralSettingsConfig);
                    SendConfigMSG(outgoing.ToString());
                    NotifyPropertyChanged();
                }
            }
        }

        public void NotifyPropertyChanged([CallerMemberName] string propertyName = null)
        {
            OnPropertyChanged(propertyName);
            if (SendConfigMSG != null)
            {
                SndPeekSettings outsettings = new SndPeekSettings(Settings);
                SndModuleSettings<SndPeekSettings> ipcMessage = new SndModuleSettings<SndPeekSettings>(outsettings);

                string targetMessage = ipcMessage.ToJsonString();
                SendConfigMSG(targetMessage);
            }
        }

        private bool _isEnabled;
    }
}
