/*-****************************************************************************

 * Copyright (C) 2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2018 Adriano Campos - <adrianoribeirocampos@gmail.com>
 *
 * Licensed under the EUPL V.1.1

****************************************************************************-*/
#ifndef TOTP_GEN_H
#define TOTP_GEN_H

//Implemented in totp_gen.cpp
std::string generateTOTP(std::string secretKey);
#endif // TOTP_GEN_H
