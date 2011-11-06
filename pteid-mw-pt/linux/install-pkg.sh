if [ "$(id -u)" != "0" ]
then
   echo "[Error] This script must be run as root" 1>&2
   exit 1
fi

if [ -e "/etc/fedora-release" ]
then
	DISTRO=fedora
else 
	if [ -e "/etc/SuSE-release" ]
	then
		DISTRO=opensuse
	else 
		if [ -e "/etc/debian_version" ]
		then
			DISTRO=debian
		else
			echo "[Error] Unknown linux distribution."
			exit -1
		fi
	fi
fi
./install-pkg-${DISTRO}.sh
