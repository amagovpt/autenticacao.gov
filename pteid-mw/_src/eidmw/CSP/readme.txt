Microsoft CSP implementation
============================

1. Intro.

A CSP (Cryptographic Service Provider) is a plugin (DLL) that is loaded
by Microsoft's CryptoAPI framework (implemented in advapi32.dll).
It can take care of crypto functions such as randomgen, hash,
sign, verify, sign, encrypt.

    Application (e.g. Word or IE)
        |
    advapi32.dll
    |    |     |
  CSP1  CSP1  ...

The CryptoAPI functions start with "Crypt", e.g. CryptSignHash(),
the corresponding CSP functions start with "CP", e.g. CPSignHash().

Note: MacOS has different but similar crypto platform, "CDSA" and
plugin libs that are called a "tokend".
Linux doesn't have a comparable crypto platform.

The CSP have to be signed by Microsoft (advapi32.dll checks these
signatures before using the CSP), and have to be 'installed' by
setting some Registry entries (HKLM\SOFTWARE\Microsoft\
Cryptography\Defaults\Provider).
What typically happens: certificates are 'registered' (put the the
Windows certificate store) in advance, meaning that they are linked
with a CSP name and a container name. When an application (e.g. Word)
wants to sign something with a key corresponding to the cert, the
CSP name is used by adavapi32.dll to load the correct CSP and the
container name is given to the CSP so it knows which cert/key pair
to address.

For more info, see "The Smart Card Cryptographic Service Provider Cookbook":
  http://msdn2.microsoft.com/en-us/library/ms953432.aspx


2. Our CSP

Our CSP is  built on top of the CAL and supports randomgen, hash,
sign and verify functionality.
The container name specified during the registration of certs
contains the card serial number and and indication about the key/cert.

Implemented functions:
  CPAcquireContext
  CPReleaseContext
  CPSetProvParam
  CPGetProvParam
  CPCreateHash
  CPDuplicateHash
  CPHashData
  CPSetHashParam
  CPGetHashParam
  CPDestroyHash
  CPSignHash
  CPGetUserKey
  CPGetKeyParam
  CPExportKey
  CPDestroyKey
  CPVerifySignature
  CPGenRandom
Not implemented functions (we don't do en/decryption):
  CPHashSessionKey
  CPGenKey
  CPDeriveKey
  CPSetKeyParam
  CPImportKey
  CPDuplicateKey
  CPEncrypt
  CPDecrypt


3. CSP signing

To sign a CSP, send a mail to Microsoft with your contact info
and info about the CSP (name, version, algos, ...).
Mail address + sample mail available with Patrick and Stef.

UPDATE: we now made a 'wrapper' (see the wrapper/ dir): this
is the real CSP in the sense that it export the CPxxx()
functions listed above..
However, the only thing this CSP does is call the CSP lib
(made in this dir), which exports corresponding functions
eidmwxxx() in which everyting is done.
So now that we signed the wrapper, we no longer have to
ask MS to sign each small modification in CSP because the
wrapper (which doesn't do anything) won't have to be modified.
See also wrapper\ms_signed\readme.txt


4. Testing

Dir ..\CSPTest\ contains unit tests that call the CSP
functions directly (not via advapi32.dll) so the CSP
doesn't have to be signed to this.

Dir ..\CSPTest\Tool\ contains tests that use the CryptoAPI
functions (in advapi32.dll) so for this tool the CSP has
to be signed and installed (or see below).

As a final test, try to sign a Word doc with both
keys on a card; and authenticate to a https website
with IE (Internet Explorer).

We bought a CSP Tracing Utility from The Soft Pedal Shop
(of consultant Doug Barlow, the ex-microsofty who seems
to be have been the main implementor of CryptoAPI) that
can be used to sniff the commands between an application
and advapi32.dll.


5. Circumventing the CSP signing requirement for testing

UPDATE: now that we have a wrapper, this is only needed
if the wrapper is modified.

- On NT, you could install Cryptographic Service Provider
Developer's Kit (CSPDK)that contains an advapi32.dll that
accepts CSP signed with a test tool.

- On other platforms, use a kernel debugger, see the CSPDK
or the cookbook

- Patch the advapi32.dll yourself (by ignoring the result
of the CSP signature verification). To replace the real
advapi32.dll (which is protected by the OS) by a patch,
you could boot from an W2K or WXP boot installation CD
and then select the Recovery Console (instead of doing
an installation). In the Recovery Console, you can 'cd'
to the System32 dir and use 'del' en 'copy' (make sure
you have a backup of the original advapi32.dll).
Patches of advapi32.dll for various versions and SPs
can be found by googling for "advapi32.dll patch Laszlo
Elteto" or so. If you can't find it for your OS, you
could kindly ask Laszlo on the microsoft.public.platformsdk.security
newsgroup.
E.g. for advapi32.dll version 5.1.2600.2180  (XP SP2):
  offset    old    new
  17C19     75     90
  17C1A     0C     90
  17C21     0F     90
  17C22     84     E9
 (use a hex editor to mnaually apply this patch)
