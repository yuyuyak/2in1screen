This is an auto rotate program driven by the built-in gyro in "2 in 1" laptops.  All 4 orientations are serviced.  Since my
laptop is 4:9 wide screen, 1920x1080, it's necessary to switch desktop pictures for portrait vs. landscape modes.  Not just 
for esthetics but weird flickering and tearing with docky.It parses the default desktop picture from the spacefm config file
for the landscape picture and uses a picture linked to ~/Pictures/TALL-1080x1920 for the portrait mode.  It also 
restarts docky and spacefm as they don't work properly unless that is done.  Spacefm is the desktop manager, modifications
would be needed for a different desktop manager.  It's also important to replace

Wacom HID 4848 Finger touch 

with the proper name of your touchscreen as reported by xinput list for example.  It works perfectly here, many thanks to the
original sources.
