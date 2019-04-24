#!/bin/bash
## Script to prepare a source tarball for Linux build

set -e
#REVISION=`svnversion .`
REVISION=6072
APP_VERSION=3.0.17
LOCALREPO=false
REWRITE=false
QUIET=false 

CURRENT_DIRECTORY=$(pwd)

die() {
    printf '%s\n' "$1" >&2
    exit 1
}

while :; do
	case $1 in
		-v|-version)	
			if [ "$2" ]; then
				APP_VERSION="$2"
				shift
			else
				die 'ERROR: "-v or -revision" requires a non-empty option argument.'
			fi
			;;
		-r|-revision)	
			if [ "$2" ]; then
				REVISION="$2"
				shift
			else
				die 'ERROR: "-r or -revision" requires a non-empty option argument.'
			fi
			;;
		-repo)
			if [ "$2" ]; then
				REPODIR="$2" 
				shift
			fi
			;;
		-o|--out)       # Takes an option argument; ensure it has been specified.
			if [ "$2" ]; then
				DESTDIR="$2"
				shift
			else
				die 'ERROR: "--file" requires a non-empty option argument.'
			fi
			;;
		-out=?*)
			DESTDIR=${1#*=} # Delete everything up to "=" and assign the remainder.
			;;
		-out=)         # Handle the case of an empty --file=
			die 'ERROR: "--file" requires a non-empty option argument.'
			;;
		-q|--quiet)
			QUIET=true 
			;;
		--force)
			REWRITE=true 
			;;
		-l|--local)
			LOCALREPO=true 
			;;
		--)              # End of all options.
			shift
			break
			;;
		-?*)
			printf 'WARN: Unknown option (ignored): %s\n' "$1" >&2
			;;
		*)               # Default case: No more options, so break out of the loop.
			break
			;;
	esac
	shift
done

if [ $DESTDIR ]; then
	if [ !$REPODIR ]; then
		REPODIR=$DESTDIR
	fi
else
	DESTDIR="$HOME/pteid-deb-packaging/pteid-mw_${APP_VERSION}svn${REVISION}"
fi

echo "SVN checkout on "
echo $DESTDIR
echo "OSC Repository on "
echo $REPODIR
echo "Revision"
echo $REVISION

if [ $LOCALREPO ]; then
	echo "NOTE: checking out local repository"
	URL='.'
else
	URL='https://projects.caixamagica.pt/cartaocidadao/repo/middleware-offline/trunk/pteid-mw-pt/_src/eidmw'
fi

if [ $REWRITE ]; then
	OPTS=" --force"
fi

if [ $QUIET ]; then
	OPTS+=" --quiet"
fi

svn export -r $REVISION $URL $DESTDIR $OPTS

cd $DESTDIR

# remove unused files
rm -rf cryptoAPI-Test minidriver misc/DSS/dss-standalone.exe eidlibCS eidlibCS_Wrapper *.bat misc/Wix_MW35 misc/scap misc/setup_win misc/mac ffpteidcertinstall pcscEmulation lib/* bin/* pkcs11Test xpi applayerTest pkcs11Test commonTest clean_mdrv_keys dss-standalone-app eidlibNodeJS_Wrapper eidlibPerl_Wrapper eidlibPhp_Wrapper eidlibPython_Wrapper eidlibTest eidlibTestBWC 

CHANGELOG_UPDATED=false
PACKAGE_BUILT=false

function ask_changelog {
	echo "Do you want to update the changelog?"
	select yn in "Yes" "No"; do
		case $yn in
			Yes ) 	CHANGELOG_UPDATED=true; break;;
			No ) 	CHANGELOG_UPDATED=false; break;;
		esac
	done
}

ask_changelog

function build {

	if [ $REPODIR != $DESTDIR ]; then
		cd $REPODIR
	fi

	dpkg-source -b $DESTDIR;
	PACKAGE_BUILT=true;
	echo "Debian package created in $REPODIR";
}

function ask_to_package {
	echo "Do you wish to package this program?"
	select yn in "Yes" "No"; do
		case $yn in
			Yes ) 	build ;
					break;;
			No ) exit;;
		esac
	done
}

function substitute_svn_version {
	sed -i 's/%define svn_revision.*/%define svn_revision '"$2"'/' $1
}

function substitute_app_version {
	sed -i 's/%define app_version.*/%define app_version '"$2"'/' $1
}

function update_changelog {
	
	# only ask if these variables are not set

	if [ -z ${NAME+x} ] || [ -z ${DEBFULLNAME+x} ]; then
		echo "What's your name? (press Enter to submit):"
		read -r name

		export DEBFULLNAME="$name"
		export NAME="$name"
	fi
	
	if [ -z ${DEBEMAIL+x} ]; then
		echo "What's your email? (press Enter to submit):"
		read -r email

		export DEBEMAIL="$email"
	fi

	# update the macros on spec file
	f1='./pteid-mw.spec'
	f2='/debian/changelog'

	substitute_svn_version $f1 $REVISION
	substitute_app_version $f1 $APP_VERSION
	
	echo "Changelog: What's new ? (press CTRL-D to submit): "
	i=0
	while read changelog
	do
		if [ $i == 0 ]; then
			dch --newversion=$APP_VERSION"svn"$REVISION "$changelog"

		else
			dch -a "$changelog"
		fi
		
		i+=1
	done

	dch --release --force-save-on-release

	# copies the new spec file to OBS repository folder
	cp $f1 $REPODIR 

	# updates the changelog on current repository
	cp $DESTDIR$f2 $CURRENT_DIRECTORY$f2
}

function inform_commit {
	echo "**NOTE**"
	echo "Do not forget to commit the updated files to OBS Server"
	echo "These file have been saved in $REPODIR"
	echo "The following files have been updated"
	echo -e "\t pteid-mw.spec"
	echo -e "\t pteid-mw_${APP_VERSION}svn${REVISION}.tar.xz"
	echo -e "\t pteid-mw_${APP_VERSION}svn${REVISION}.dsc"
}

if [ $CHANGELOG_UPDATED == true ]; then
	update_changelog
	ask_to_package
	inform_commit
else
	ask_to_package
	inform_commit
fi


