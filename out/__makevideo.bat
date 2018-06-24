
-- this seems to actually work (chrome / firefox / windows)
-- a bit low quality though.
.\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v libvpx -crf 10 -b:v 1M -c:a libvorbis __out.webm


-- this makes a 3.6mb file which is a bit large
.\ffmpeg.exe -framerate 30 -i frame.%d.png -c:v libvpx -crf 4 -b:v 0 -c:a libvorbis __out.webm

