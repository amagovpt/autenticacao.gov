#!/usr/bin/php -dextension=../GeneratedModule/pteid.so
<?php

require "../GeneratedModule/pteid.php";

function init() {
  print "CC SDK example, starting...\n";
  PTEID_ReaderSet::initSDK();
}

function release() {
  print "CC SDK example, releasing resources...\n";
  PTEID_ReaderSet::releaseSDK();
}

function start() {
  $readers = PTEID_ReaderSet::instance();

  $count = $readers->readerCount();
  print "Number of connected card readers to the system: $count\n";

  if ($count) {
    $devices_names = array();

    for ($idx = 0; $idx < $count; $idx++) {
      print "Obtaining card name for index: $idx\n";

      $name = $readers->getReaderName($idx);
      array_push($devices_names, $name);

      print "Connected device: $name\n";
    }

    $ctx_with_card = null;
    foreach ($devices_names as $name) {
      $reader_ctx = $readers->getReaderByName($name);

      if ($reader_ctx->isCardPresent()) {
        print "The reader <$name> has a card in it.\n";

        $ctx_with_card = $reader_ctx;
        break;
      }
    }
    
    if (isset($ctx_with_card)) {
      $card = $ctx_with_card->getEIDCard();
      $card_id = $card->getID();

      dump_card_info($card_id);
    }
    else {
      print "There's no card present in the reader, terminating...\n";
    }
  }
}

function dump_card_info($id) {
  print "-------------------------------------------------------\n";
  print "deliveryEntity             " . $id->getIssuingEntity() .         "\n";
  print "country                    " . $id->getCountry() .               "\n";
  print "documentType               " . $id->getDocumentType() .          "\n";
  print "cardNumber                 " . $id->getDocumentNumber() .        "\n";
  print "cardNumberPAN              " . $id->getDocumentPAN() .           "\n";
  print "cardVersion                " . $id->getDocumentVersion() .       "\n";
  print "deliveryDate               " . $id->getValidityBeginDate() .     "\n";
  print "locale                     " . $id->getLocalofRequest() .        "\n";
  print "validityDate               " . $id->getValidityEndDate() .       "\n";
  print "name                       " . $id->getSurname() .               "\n";
  print "firstname                  " . $id->getGivenName() .             "\n";
  print "sex                        " . $id->getGender() .                "\n";
  print "nationality                " . $id->getNationality() .           "\n";
  print "birthDate                  " . $id->getDateOfBirth() .           "\n";
  print "height                     " . $id->getHeight() .                "\n";
  print "numBI                      " . $id->getCivilianIdNumber() .      "\n";
  print "nameFather                 " . $id->getSurnameFather() .         "\n";
  print "firstnameFather            " . $id->getGivenNameFather() .       "\n";
  print "nameMother                 " . $id->getSurnameMother() .         "\n";
  print "firstnameMother            " . $id->getGivenNameMother() .       "\n";
  print "numNIF                     " . $id->getTaxNo() .                 "\n";
  print "numSS                      " . $id->getSocialSecurityNumber() .  "\n";
  print "numSNS                     " . $id->getHealthNumber() .          "\n";
  print "Accidental indications     " . $id->getAccidentalIndications() . "\n";
  print "mrz1                       " . $id->getMRZ1() .                  "\n";
  print "mrz2                       " . $id->getMRZ2() .                  "\n";
  print "mrz3                       " . $id->getMRZ3() .                  "\n";
  print "-------------------------------------------------------\n";
}

init();
start();
release();
