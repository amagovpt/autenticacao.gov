A tokend (Mac OS 10.4 and up) is like a CSP for
Windows' CryptoAPI frramework: it provides crypto
support (authentication/signatures in the case of
an eID card) to applications that use the Apple's
CDSA framework (Safari, Entourage, ..)

So we have:
 - pkcs11: Firefox, Thunderbird, Acrobat, OOo
 - tokend: Safari, Mail, Login, Entourage, VPN, ...

The BEID.tokend is completely separated from the
other eidmw sources, as it mostly a fix (+ some
speed improvements) of the Apple's BELPIC.tokend.

Everything needed to build it is in a Builds.dmg
(see the docs/ dir) that isn't in svn because it's
to big (> 3 GB) so you have to fetch it first from
/home/depot/BEID_tokend on the svn server (currently
212.35.97.189, later perhaps on the new svn server
 10.2.250.30).
So just put that Builds.dmg in this dir and run
./buildtokend.sh
