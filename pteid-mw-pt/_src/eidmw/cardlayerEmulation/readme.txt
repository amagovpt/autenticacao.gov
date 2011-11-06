To test libs/apps on top of the cardlayerlib , you could link
against pteidcardlayerEmulation.lib instead of pteidcardlayer.lib.

This way the cardlayer is linked against a PCSC emulation
layer instead of the real PCSC. And this PCSC emulation layer
gives you 10 virtual readers, with for the moment the following
virtual cards:
 - in "Emulated reader 0": a copy of a test Belpic V1 card
 - in "Emulated reader 1": no card
 - in "Emulated reader 2": a copy of real Belpic V1 card (except for
     the private keys, so signing doesn't work -- but see the PS) 
 - in "Emulated reader 3": a copy of test Belpic V2 card
 - in "Emulated reader 4": a copy of real SIS card
 - in the other readers: no card

PS: Internal Auth. and (for V2 cards) Get Signed PIN status and
Get Signed Card Data are implemented, but a fake signature is
returned. But since we can't verify it anyway because only Govmnt
has the public 'base' key, this is no problem.

 All PINs are "1234". No unblocking/activation has been implemented
 because it's not needed for Belpic. (-> TODO for other cards)

 (Note: the cardlayer lib limits the number of readers to 8)

You can programmatically remove/insert the same or other
virtual cards in each reader. Or tell the card what to return
(e.g. to simulate a non-activated card).
See pcscEmulation/EmulationPCSC.h and 
pcscEmulation/EmulationCardFactory.h

So (unit) tests those libs/apps becomes easier because there
is no need for a reader or a specific card; and manual
insertions/removals are no longer needed.

Note: things haven't been tested that much, especially
the virtual insertion/removal.
