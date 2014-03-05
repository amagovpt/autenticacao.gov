#!/usr/bin/env python3

import sys
sys.path.append("../GeneratedModule")
import pteid

def init():
  # First of all you always have to initialize the sdk.
  print("CC sdk example, starting...")
  pteid.PTEID_ReaderSet.initSDK()

def release():
  print("CC sdk example, releasing resources...")
  # At the end, always release the sdk
  pteid.PTEID_ReaderSet.releaseSDK()

def start():
  # Obtain the instance of readerSet
  readers = pteid.PTEID_ReaderSet.instance()

  count = readers.readerCount()
  print("Number of connected card readers to the system:", count)

  # Check if we have connected smart card readers
  if count:
    devices_names = []

    # Obtain the names of all connected readers
    for idx in range(count):
      print("Obtaining card name for index:", idx)

      name = readers.getReaderName(idx)
      devices_names.append(name)

      print("Connected device:", name)

    ctx_with_card = None
    # try to find a smart card in one of the detected readers
    for name in devices_names:
      reader_ctx = readers.getReaderByName(name)
      
      if reader_ctx.isCardPresent():
        print("The reader <", name, "> has a card in it.")
       
        ctx_with_card = reader_ctx
        # found a card connected, don't look around in other readers
        break

    # Now check if we have a card to access we can keep going
    if ctx_with_card:
      card = ctx_with_card.getEIDCard()
      card_id = card.getID()

      dump_card_info(card_id)
    else:
      print("There's no card present in the reader, terminating...")

def dump_card_info(id):
  print("Card ID: ", id)
  print("-------------------------------------------------------") 
  print("deliveryEntity             ", id.getIssuingEntity())
  print("country                    ", id.getCountry())
  print("documentType               ", id.getDocumentType())
  print("cardNumber                 ", id.getDocumentNumber())
  print("cardNumberPAN              ", id.getDocumentPAN())
  print("cardVersion                ", id.getDocumentVersion())
  print("deliveryDate               ", id.getValidityBeginDate())
  print("locale                     ", id.getLocalofRequest())
  print("validityDate               ", id.getValidityEndDate())
  print("name                       ", id.getSurname())
  print("firstname                  ", id.getGivenName())
  print("sex                        ", id.getGender())
  print("nationality                ", id.getNationality())
  print("birthDate                  ", id.getDateOfBirth())
  print("height                     ", id.getHeight())
  print("numBI                      ", id.getCivilianIdNumber())
  print("nameFather                 ", id.getSurnameFather())
  print("firstnameFather            ", id.getGivenNameFather())
  print("nameMother                 ", id.getSurnameMother())
  print("firstnameMother            ", id.getGivenNameMother())
  print("numNIF                     ", id.getTaxNo())
  print("numSS                      ", id.getSocialSecurityNumber())
  print("numSNS                     ", id.getHealthNumber())
  print("Accidental indications     ", id.getAccidentalIndications())
  print("mrz1                       ", id.getMRZ1())
  print("mrz2                       ", id.getMRZ2())
  print("mrz3                       ", id.getMRZ3())
  print("-------------------------------------------------------") 

if __name__ == "__main__":
  init()
  start()
  release()
