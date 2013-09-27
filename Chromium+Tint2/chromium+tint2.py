#!/usr/bin/env python
import gtk
import os
import signal
import subprocess
from wallpaper import set_wallpaper
import wnck

class App(object):
    def __init__(self):
        self.wallpapers = {
            "none":             "/home/themylogin/Images/Wallpapers/3200x1200/pure-none.jpg",
            "frame":            "/home/themylogin/Images/Wallpapers/3200x1200/pure-frame.jpg",
            "frame-inactive":   "/home/themylogin/Images/Wallpapers/3200x1200/pure-frame-inactive.jpg",
        }

        self.tint2_signals = {
            "none":             signal.SIGUSR1,
            "frame":            signal.SIGUSR2,
            "frame-inactive":   signal.SIGUSR1,
        }

        self.screen = wnck.screen_get_default()
        self.screen.force_update()

        self.tint2_pid = None

        self.last_maximized_window = None
        for window in self.screen.get_windows():
            if self.is_maximized_window(window):
                self.last_maximized_window = window
                break

        self.current_environment = None
        self.fit_environment()

        self.screen.connect("active-window-changed", self.on_active_window_changed)
        self.screen.connect("showing-desktop-changed", self.on_showing_desktop_changed)

    def is_chromium_window(self, window):
        if window:
            application = window.get_application()
            if application:
                if application.get_name() == "Chromium":
                    return True
        return False

    def is_maximized_window(self, window):
        return window.is_maximized()

    def fit_environment(self):
        environment = self.calculate_environment()
        if environment != self.current_environment:
            self.current_environment = environment

            self.kill_tint2(self.tint2_signals[environment])

            set_wallpaper(self.wallpapers[environment])

    def calculate_environment(self):
        if self.screen.get_showing_desktop():
            return "none"

        active_window = self.screen.get_active_window()
        if active_window:
            if self.is_chromium_window(active_window):
                if self.is_maximized_window(active_window):
                    return "frame"
                else:
                    return "none"
            else:
                if self.is_maximized_window(active_window):
                    return "none"
                else:
                    if self.is_chromium_window(self.last_maximized_window):
                        return "frame-inactive"
                    else:
                        return "none"
        else:
            return "none"

    def kill_tint2(self, signal):        
        if self.tint2_pid is None:
            try:
                self.tint2_pid = int(subprocess.check_output(["pidof", "tint2"]))
            except:
                return

        if self.tint2_pid:
            try:
                os.kill(self.tint2_pid, signal)
            except:
                self.tint2_pid = None
                self.kill_tint2(signal) # Try again

    def on_active_window_changed(self, screen, previous_window):
        self.fit_environment()

        active_window = self.screen.get_active_window()
        if active_window:
            if self.is_maximized_window(active_window):
                self.last_maximized_window = active_window

            active_window.connect("state-changed", self.on_window_state_changed)

    def on_showing_desktop_changed(self, screen):
        self.fit_environment()

    def on_window_state_changed(self, window, changed_mask, new_state):
        if changed_mask & 6:
            self.fit_environment()

App()
gtk.main()
