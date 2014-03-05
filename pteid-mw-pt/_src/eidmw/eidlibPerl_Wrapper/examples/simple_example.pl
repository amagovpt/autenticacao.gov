#!/usr/bin/env perl

use strict;
use warnings;

use lib "../GeneratedModule";
use Pteid;

sub init {
  print("CC SDK example, starting...\n");
  Pteid::PTEID_ReaderSet::initSDK();
}

sub release {
  print("CC SDK example, releasing resources...\n");
  Pteid::PTEID_ReaderSet::releaseSDK();
}

sub start {
  my $readers = Pteid::PTEID_ReaderSet::instance();

  my $count = $readers->readerCount();
  print("Number of connected card readers to the system: $count\n");

  if ($count) {
    my @devices_names = ();

    for (my $idx = 0; $idx < $count; $idx++) {
      print("Obtaining card name for index: $idx\n");

      my $name = $readers->getReaderName($idx);
      push(@devices_names, $name);

      print("Connected device: $name\n");
    }

    my $ctx_with_card;
    foreach my $name (@devices_names) {
      my $reader_ctx = $readers->getReaderByName($name);

      if ($reader_ctx->isCardPresent()) {
        print("The reader <$name> has a card in it.\n");

        $ctx_with_card = $reader_ctx;
        last;
      }
    }

    if (defined($ctx_with_card)) {
      my $card = $ctx_with_card->getEIDCard();
      my $card_id = $card->getID();

      dump_card_info($card_id);
    }
    else {
      print("There's no card present in the reader, terminating...\n");
    }
  }
}

sub dump_card_info {
  my $id = shift;
  print("Card ID: $id\n");
  print("-------------------------------------------------------", "\n");
  print("deliveryEntity             ", $id->getIssuingEntity(),          "\n");
  print("country                    ", $id->getCountry(),                "\n");
  print("documentType               ", $id->getDocumentType(),           "\n");
  print("cardNumber                 ", $id->getDocumentNumber(),         "\n");
  print("cardNumberPAN              ", $id->getDocumentPAN(),            "\n");
  print("cardVersion                ", $id->getDocumentVersion(),        "\n");
  print("deliveryDate               ", $id->getValidityBeginDate(),      "\n");
  print("locale                     ", $id->getLocalofRequest(),         "\n");
  print("validityDate               ", $id->getValidityEndDate(),        "\n");
  print("name                       ", $id->getSurname(),                "\n");
  print("firstname                  ", $id->getGivenName(),              "\n");
  print("sex                        ", $id->getGender(),                 "\n");
  print("nationality                ", $id->getNationality(),            "\n");
  print("birthDate                  ", $id->getDateOfBirth(),            "\n");
  print("height                     ", $id->getHeight(),                 "\n");
  print("numBI                      ", $id->getCivilianIdNumber(),       "\n");
  print("nameFather                 ", $id->getSurnameFather(),          "\n");
  print("firstnameFather            ", $id->getGivenNameFather(),        "\n");
  print("nameMother                 ", $id->getSurnameMother(),          "\n");
  print("firstnameMother            ", $id->getGivenNameMother(),        "\n");
  print("numNIF                     ", $id->getTaxNo(),                  "\n");
  print("numSS                      ", $id->getSocialSecurityNumber(),   "\n");
  print("numSNS                     ", $id->getHealthNumber(),           "\n");
  print("Accidental indications     ", $id->getAccidentalIndications(),  "\n");
  print("mrz1                       ", $id->getMRZ1(),                   "\n");
  print("mrz2                       ", $id->getMRZ2(),                   "\n");
  print("mrz3                       ", $id->getMRZ3(),                   "\n");
  print("-------------------------------------------------------", "\n");
}

init();
start();
release();
