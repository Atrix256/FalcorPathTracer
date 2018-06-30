
rem this seems to actually work (chrome / firefox / windows)
rem a bit low quality though.
rem .\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v libvpx -crf 10 -b:v 1M -c:a libvorbis __out.webm

rem .\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v mpeg2video -q:v 5 -c:a mp2 -f vob __out.mpg


rem this makes a 3.6mb file which is a bit large
rem .\ffmpeg.exe -framerate 30 -i frame.%%d.png -c:v libvpx -crf 4 -b:v 0 -c:a libvorbis __out.webm

.\ffmpeg.exe -framerate 30 -i frame.%%d.png -vcodec libx264 -vb 20M __out.mp4