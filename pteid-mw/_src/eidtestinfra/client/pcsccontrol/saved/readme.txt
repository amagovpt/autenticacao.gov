The pcsccontrol.jar can only be built on JDK 1.6 (or up)
because the javax.smartcardio isn't present below 1.6.

So we keep store the latests pcsccontrol.jar here
(built with JDK 1.6 with target = 1.5) for OS-es
where no JRE/JDK 1.6 is present for now.

TODO: to be removed once JDK1.6 is present on all OS-es.

CAREFULL: if the java sources (or schemas) changed,
the pcsccontrol.jar must be put to svn again!

STH, 19.01.2008
