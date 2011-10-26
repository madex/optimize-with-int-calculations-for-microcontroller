set key right nobox
set samples 1000

s_opt(x) = 32767*sin(x*pi/32767)
tab_index(x) = int(x/256)*256
tab1(x)  = int(s_opt(tab_index(x)))
tab2(x)  = int(s_opt(tab_index(x)+256))
part(x)  = int(x)%256
s_tab(x) = tab1(x) + ((tab2(x) - tab1(x))*part(x))/256

plot [1250:1750] s_opt(x), s_tab(x), tab1(x), tab2(x)
