Libraries dependencies: libxml2, libconfig, libpython-dev
Step to build and install:
  git clone
  cd Smart_Classroom/Media_Hub/src
  make
  sudo make install
  
to start app at boot, edit /etc/rc.local:
  sudo nano /etc/rc.local
  
add following line:
  startMediaHub.sh &
  
save file then reboot

clean source built:
  make clean
  
uninstall app from file systems:
  sudo make uninstall
