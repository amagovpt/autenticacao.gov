#!/usr/bin/env node

var pteid = require('../build/Release/pteid');

function init() {
  console.log('CC sdk example, starting...');
  pteid.PTEID_ReaderSet.initSDK();
}

function release() {
  console.log('CC sdk example, releasing resources...');
  pteid.PTEID_ReaderSet.releaseSDK();
}

function start() {
  var readers,
      count,
      device_names,
      idx_reader_name,
      ctx_with_card,
      reader_ctx,
      card,
      card_id,
      idx;

  readers = pteid.PTEID_ReaderSet.instance();

  count = readers.readerCount();
  console.log('Number of connected card readers to the system:', count);

  if (count) {
    device_names = [];
    for (idx = 0; idx < count; idx++) {
      console.log('Obtaining card name for index:', idx);

      idx_reader_name = readers.getReaderName(idx);
      device_names.push(idx_reader_name);

      console.log('Connected device:', idx_reader_name);
    }

    ctx_with_card = null;
    for (idx = 0; idx < device_names.length; idx++) {
      reader_ctx = readers.getReaderByName(device_names[idx]);

      if (reader_ctx.isCardPresent()) {
        console.log('The reader <', device_names[idx], '> has a card in it.');

        ctx_with_card = reader_ctx;
        break;
      }
    }

    if (ctx_with_card !== null) {
      card = ctx_with_card.getEIDCard();
      card_id = card.getID();

      dump_card_info(card_id);
    } else {
      console.log("There's no card present in the reader, terminating...");
    }
  }
}

function dump_card_info(id) {
  console.log('Card ID:', id);
  console.log("-------------------------------------------------------");
  console.log("deliveryEntity             ", id.getIssuingEntity());
  console.log("country                    ", id.getCountry());
  console.log("documentType               ", id.getDocumentType());
  console.log("cardNumber                 ", id.getDocumentNumber());
  console.log("cardNumberPAN              ", id.getDocumentPAN());
  console.log("cardVersion                ", id.getDocumentVersion());
  console.log("deliveryDate               ", id.getValidityBeginDate());
  console.log("locale                     ", id.getLocalofRequest());
  console.log("validityDate               ", id.getValidityEndDate());
  console.log("name                       ", id.getSurname());
  console.log("firstname                  ", id.getGivenName());
  console.log("sex                        ", id.getGender());
  console.log("nationality                ", id.getNationality());
  console.log("birthDate                  ", id.getDateOfBirth());
  console.log("height                     ", id.getHeight());
  console.log("numBI                      ", id.getCivilianIdNumber());
  console.log("nameFather                 ", id.getSurnameFather());
  console.log("firstnameFather            ", id.getGivenNameFather());
  console.log("nameMother                 ", id.getSurnameMother());
  console.log("firstnameMother            ", id.getGivenNameMother());
  console.log("numNIF                     ", id.getTaxNo());
  console.log("numSS                      ", id.getSocialSecurityNumber());
  console.log("numSNS                     ", id.getHealthNumber());
  console.log("Accidental indications     ", id.getAccidentalIndications());
  console.log("mrz1                       ", id.getMRZ1());
  console.log("mrz2                       ", id.getMRZ2());
  console.log("mrz3                       ", id.getMRZ3());
  console.log("-------------------------------------------------------");
}

init();
start();
release();
