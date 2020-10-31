# dt2s-vita (Double Tap 2 Sleep for PS VITA)

This is a usermode plugin inspired by the famous Android feature "Double tap to sleep". It does precisely what the name suggests, puts your PS VITA to sleep if you double tap the touch screen.

# Notes:
There are two versions.

- dt2s_statusbar.suprx (Recommended)
  * This version only works if you touch the very top part of your touchscreen. In SceShell (homescreen) this would be the status bar. This works more similarly to Android's double tap to sleep feature which is used on the status bar.

- dt2s.suprx
  * This version works if you tap anywhere on the screen twice. The only reason this version is not recommended because ocassionally you may tap the touch screen twice when you didn't want to. 

# Installation:
Place the preferred plugin (dt2s_statusbar.suprx or dt2s.suprx) file into your tai folder in your PS Vita.

Open your tai config and add the following:

1) If you want to use the dt2s_statusbar.suprx only on the homescreen (Recommended)
```
*main
ux0:tai/dt2s_statusbar.suprx
```

2) If you want to use dt2s.suprx available for a specific application:
```
# titleid for your application
*ALL
ux0:tai/dt2s.suprx
```
