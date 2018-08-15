Generic Monitor plugin (GenMon)
===========================================


**DESCRIPTION**

The GenMon plugin cyclically spawns the indicated script/program, captures its output and displays the result into the panel.


**INSTALLATION**

  * Clone this repository to your `home` directory by typing:
  `git clone https://github.com/rilian-la-te/xfce4-genmon-plugin.git` then `cd` into the directory.
  * type `git submodule init && git submodule update` to download the submodules (this will download the cmake submodule you see in the repository, above)
  * type `mkdir build && cd build` (to keep things tidy)
  * You're **almost** ready to run `cmake`. First, review the following flags:
    * CMAKE FLAGS:
      * `-DENABLE_XFCE=[ON/OFF]` Use `ON` to compile applet for XFCE Panel
      * `-DENABLE_BUDGIE=[ON/OFF]` Use `ON` to compile for budgie (experimental)
      * `-DENABLE_VALAPANEL=[ON/OFF]` Use `ON` to compile for Vala Panel
      * `-DENABLE_MATE=[ON/OFF]` Use `ON` to compile for MATE Panel
      * `-DCMAKE_INSTALL_PREFIX=[path]` By default, GenMon will install in the `/usr/local` directory. You can use this flag to change that. For some DEs (XFCE, for example), it is required to match install prefix with panel prefix (`/usr` in most distros), so, do not forget it.
  * once you've decided on any flags you want to include, type (from your build directory) `cmake [flags] ..`
  * once the build is successful, you can compile and install Vala-Panel-Appmenu by typing `make && sudo make install`

**TESTING**

Use GenMon for invoking the provided "datetime" shell script.
Then the current date/time should be continuously updated in your panel.

If the plugin displays XXX it means that the spawn of the command has failed.

**EXTRA**

There are simple example scripts provided with plugin, installed into system libexec dir.

You can try to use it as is, or copy and modify to suit your own needs.

**XML TAGS**

GenMon plugin can display an icon (which can be a
clickable icon), a tooltip, a text and a bar.

To display these items, the script called by genmon should return a simple
XML string.

The XML tags which can be used are :
```
<txt>Text to display</txt>
<img>Path to the image to display</img>
<tool>Tooltip text</tool>
<bar>Pourcentage to display in the bar</bar>
<click>The command to be executed when clicking on the image</click>
```

If None of the tags are detected in the result of the command, the plugin
returns to its original behaviour (displaying the result of the command).

No tag is mandatory: for instance, you can display a tooltip without and image...

The text between the <txt> and </txt>, and the <tool> and </tool> tags can use pango markup for style and color.

Here's a simple example of a plugin displaying the Temp of the CPU in the panel
(with an icon) and the Freq in a Tooltip:

```
#!/bin/bash
echo "<img>/usr/share/icons/Bluecurve/16x16/apps/gnome-monitor.png</img>"

declare -i cpuFreq
cpuFreq=$(cat /proc/cpuinfo | grep "cpu MHz" | sed 's/\ \ */ /g' | cut -f3 -d" " | cut -f1 -d".")

if [ $cpuFreq -ge 1000 ]
then
  cpu=$(echo $cpuFreq | cut -c1).$(echo $cpuFreq | cut -c2)GHz
else
  cpu=${cpuFreq}MHz
fi
echo "<txt>"$(cat /proc/acpi/thermal_zone/THM/temperature | sed 's/\ \ */ /g' | cut -f2 -d" ")" C</txt>"
echo "<tool>Freq: "$cpu"</tool>"
```

PS: Depending on your configuration, you should change the image path.

You can find more scripts in system libexec dir or in [xtonousou's repo](https://github.com/xtonousou/xfce4-genmon-scripts). 

**REMOTE COMMANDS**

*XFCE*

The xfce4-genmon-plugin now supports xfce4-panel plugin-event functionality. Currently it only supports the 
"refresh" action which resets the timer and causes the associated command to execute again. You can use this 
command remotely or at the end of a script to force the panel plugin to refresh.

To refresh the plugin, issue the command:
```
    xfce4-panel --plugin-event=genmon-X:refresh:bool:true
``` 
where "genmon-X" is the widget name of the particular genmon instance. To get this name, go to the panel 
properties screen and on the Items tab, hover your mouse over the genmon plugin to get it's internal name.

*Vala Panel*

You can also use refresh in ValaPanel. To refresh a plugin, use command:
```
	vala-panel --remote-command=*uuid*,refresh
```
You can find and set applet's UUID in Vala Panel's config file (corresponded to current ValaPanel profile).

**MISC**

Applet can be styled via GTK3 CSS. Please refer to the 'CSS Styling.txt' document for more information.


**BUGS**

If you find a bug, fix it, then tell me how you did it :-)


Enjoy!
Roger Seguin
roger_seguin@msn.com

Julien Devemy <jujucece@gmail.com>
