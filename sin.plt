reset
set key right nobox
set samples 1000

plot [0:65535] 32767*sin(x*pi/32767), (int(x)&16384)

