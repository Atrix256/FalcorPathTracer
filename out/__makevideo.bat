
-- this makes a corrupt file??
.\ffmpeg.exe -i frame.%d.png __out.webm


-- these 2 commands need to be run (??) to make a webm file
-- from https://gist.github.com/Vestride/278e13915894821e1d6f

.\ffmpeg.exe -i frame.%d.png -c:v libvpx-vp9 -pass 1 -b:v 1000K -threads 1 -speed 4 -tile-columns 0 -frame-parallel 0 -auto-alt-ref 1 -lag-in-frames 25 -g 9999 -aq-mode 0 -an -f webm __blah.webm

.\ffmpeg.exe -i frame.%d.png -c:v libvpx-vp9 -pass 2 -b:v 1000K -threads 1 -speed 0 -tile-columns 0 -frame-parallel 0 -auto-alt-ref 1 -lag-in-frames 25 -g 9999 -aq-mode 0 -c:a libopus -b:a 64k -f webm __out.webm




-- this makes a video i can view in chrome but no where else

.\ffmpeg.exe -framerate 30 -i frame.%d.png __out.mp4



-- this seems to actually work (chrome / firefox / windows)
-- a bit low quality though.
.\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v libvpx -crf 10 -b:v 1M -c:a libvorbis __out.webm


-- this makes a 3.6mb file which is a bit large
.\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v libvpx -crf 4 -b:v 0 -c:a libvorbis __out.webm

