#!/bin/sh 

while true; do
        ./home/pi/Smart_Classroom/Media_Hub/src/audio_app &
        killall /home/pi/Smart_Classroom/Media_Hub/src/audio_app
done

